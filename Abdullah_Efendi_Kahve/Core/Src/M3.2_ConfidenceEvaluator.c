#include "M3.2_ConfidenceEvaluator.h"
#include <math.h>
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/*  Dahili: Tek kanal gÃ¼ven deÄŸerlendirmesi                                   */
/* -------------------------------------------------------------------------- */
static void EvaluateChannel(float curr_val, float *last_val, float *curr_conf,
                            uint32_t update_time, uint32_t curr_time, float max_spike)
{
    /* 1. ZAMAN AÅIMI KONTROLÃœ */
    // curr_time < update_time durumu, fonksiyon Ã§alÄ±ÅŸma sÃ¼resinden dolayÄ±
    // timestamp kaymalarÄ±nda oluÅŸabilir. Unsigned underflow'u Ã¶nlemek iÃ§in
    // sadece curr_time > update_time ise farka bakÄ±yoruz.
    if (curr_time > update_time && (curr_time - update_time) > CONFIDENCE_TIMEOUT_US) {
        *curr_conf = 0.0f;
        return;
    }

    /* 2. FÄ°ZÄ°KSEL SIÃ‡RAMA (SPIKE) KONTROLÃœ */
    float delta = fabs(curr_val - (*last_val));
    if (delta > max_spike) {
        /* Ä°mkansÄ±z sÄ±Ã§rama â†’ gÃ¼veni cezalandÄ±r */
        *curr_conf = (*curr_conf) * CONFIDENCE_SPIKE_PENALTY_FACTOR;
    } else {
        /* Veri saÄŸlÄ±klÄ± â†’ gÃ¼veni toparla */
        *curr_conf += CONFIDENCE_RECOVERY_RATE;
        if (*curr_conf > 1.0f) {
            *curr_conf = 1.0f;
        }
        *last_val = curr_val;
    }

    /* 3. EKF'ler zaten kendi iÃ§lerinde CONFIDENCE_MIN_VALID kontrolÃ¼ yaptÄ±klarÄ± iÃ§in 
       burada 0'a eÅŸitlemek (resetlemek) toparlanmayÄ± (recovery) imkansÄ±z kÄ±lan bir dead-lock yaratÄ±yordu. 
       Bu yÃ¼zden 0'a zorlama kÄ±smÄ± kaldÄ±rÄ±ldÄ±. GÃ¼ven adÄ±m adÄ±m artarak eÅŸiÄŸi geÃ§ebilecek. */
}

/* ========================================================================== */
/*  CE_Init                                                                   */
/* ========================================================================== */
void CE_Init(Confidence_Evaluator_t *eval) {
    if (eval == NULL) return;

    eval->last_acc_x = 0.0f;
    eval->last_acc_y = 0.0f;
    eval->last_acc_z = GRAVITY_MPS2;   /* Statik durumda Z â‰ˆ 9.81 m/sÂ² */
    eval->last_gyro_x = 0.0f;
    eval->last_gyro_y = 0.0f;
    eval->last_gyro_z = 0.0f;
    eval->last_mag_x = 0.0f;
    eval->last_mag_y = 0.0f;
    eval->last_mag_z = 0.0f;
    eval->last_baro_press = FILTER_SEA_LEVEL_PA_DEFAULT;
    eval->initialized = 0;
}

/* ========================================================================== */
/*  CE_Update                                                                 */
/*  calibratedValue birimleri: ivme=m/sÂ², gyro=rad/s, baro=Pa                 */
/* ========================================================================== */
void CE_Update(Confidence_Evaluator_t *eval, DataCenter *dataC, uint32_t curr_time) {
    if (eval == NULL || dataC == NULL) return;

    if (eval->initialized == 0) {
        eval->last_acc_x = dataC->acc.x.calibratedValue;
        eval->last_acc_y = dataC->acc.y.calibratedValue;
        eval->last_acc_z = dataC->acc.z.calibratedValue;
        eval->last_gyro_x = dataC->gyro.x.calibratedValue;
        eval->last_gyro_y = dataC->gyro.y.calibratedValue;
        eval->last_gyro_z = dataC->gyro.z.calibratedValue;
        eval->last_mag_x = dataC->mag.x.calibratedValue;
        eval->last_mag_y = dataC->mag.y.calibratedValue;
        eval->last_mag_z = dataC->mag.z.calibratedValue;
        if (dataC->baro.press.calibratedValue > 0.0f) eval->last_baro_press = dataC->baro.press.calibratedValue;
        eval->initialized = 1;
    }

    /* --- 1. Ä°VMEÃ–LÃ‡ER KANALLARI (m/sÂ²) --- */
    EvaluateChannel(dataC->acc.x.calibratedValue, &eval->last_acc_x,
                    &dataC->acc.x.confidence, dataC->acc.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);
    EvaluateChannel(dataC->acc.y.calibratedValue, &eval->last_acc_y,
                    &dataC->acc.y.confidence, dataC->acc.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);
    EvaluateChannel(dataC->acc.z.calibratedValue, &eval->last_acc_z,
                    &dataC->acc.z.confidence, dataC->acc.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);

    /* --- 2. JÄ°ROSKOP KANALLARI (rad/s) --- */
    EvaluateChannel(dataC->gyro.x.calibratedValue, &eval->last_gyro_x,
                    &dataC->gyro.x.confidence, dataC->gyro.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
    EvaluateChannel(dataC->gyro.y.calibratedValue, &eval->last_gyro_y,
                    &dataC->gyro.y.confidence, dataC->gyro.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
    EvaluateChannel(dataC->gyro.z.calibratedValue, &eval->last_gyro_z,
                    &dataC->gyro.z.confidence, dataC->gyro.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);

    /* --- 3. BAROMETRE KANALI (Pa) --- */
    EvaluateChannel(dataC->baro.press.calibratedValue, &eval->last_baro_press,
                    &dataC->baro.press.confidence, dataC->baro.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_BARO_PA);

    /* --- 4. MANYETOMETRE (Spike + Timeout kontrolÃ¼) --- */
    /* Manyetometre motor giriÅŸimi yÃ¼zÃ¼nden anlÄ±k sÄ±Ã§ramalar (spike) yapabilir. */
    EvaluateChannel(dataC->mag.x.calibratedValue, &eval->last_mag_x,
                    &dataC->mag.x.confidence, dataC->mag.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_MAG_UT);
    EvaluateChannel(dataC->mag.y.calibratedValue, &eval->last_mag_y,
                    &dataC->mag.y.confidence, dataC->mag.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_MAG_UT);
    EvaluateChannel(dataC->mag.z.calibratedValue, &eval->last_mag_z,
                    &dataC->mag.z.confidence, dataC->mag.UpdateTime,
                    curr_time, CONFIDENCE_MAX_SPIKE_MAG_UT);

    /* --- 5. GPS (Timeout + Fix kontrolÃ¼) --- */
    if ((curr_time - dataC->gps.UpdateTime) > CONFIDENCE_TIMEOUT_US || dataC->gps.fixQuality == 0) {
        dataC->gps.x.confidence = 0.0f;
        dataC->gps.y.confidence = 0.0f;
        dataC->gps.z.confidence = 0.0f;
        dataC->gps.speed.confidence = 0.0f;
        dataC->gps.course.confidence = 0.0f;
    } else {
        /* GPS fix var â†’ gÃ¼veni toparla */
        dataC->gps.x.confidence += CONFIDENCE_RECOVERY_RATE;
        dataC->gps.y.confidence += CONFIDENCE_RECOVERY_RATE;
        dataC->gps.z.confidence += CONFIDENCE_RECOVERY_RATE;
        dataC->gps.speed.confidence += CONFIDENCE_RECOVERY_RATE;
        dataC->gps.course.confidence += CONFIDENCE_RECOVERY_RATE;
        if (dataC->gps.x.confidence > 1.0f) {
            dataC->gps.x.confidence = 1.0f;
            dataC->gps.y.confidence = 1.0f;
            dataC->gps.z.confidence = 1.0f;
            dataC->gps.speed.confidence = 1.0f;
            dataC->gps.course.confidence = 1.0f;
        }
    }
}

