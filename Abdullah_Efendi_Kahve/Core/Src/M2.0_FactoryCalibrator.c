#include "main.h"
#include "M2.1_CalibrationUI.h"
#include "M2.0_FactoryCalibrator.h"
#include <math.h>
#include <string.h>

void SensorCalib_Init(SensorCalib_t *calib) {
    if (!calib) return;
    
    // Config dosyasÃ„Â±ndan mevcut deÃ„Å¸erleri yÃƒÂ¼kle
    calib->acc_x_bias = CALIB_ACC_X_BIAS;
    calib->acc_y_bias = CALIB_ACC_Y_BIAS;
    calib->acc_z_bias = CALIB_ACC_Z_BIAS;
    
    calib->acc_x_scale = CALIB_ACC_X_SCALE;
    calib->acc_y_scale = CALIB_ACC_Y_SCALE;
    calib->acc_z_scale = CALIB_ACC_Z_SCALE;
    
    calib->gyro_x_bias = CALIB_GYRO_X_BIAS;
    calib->gyro_y_bias = CALIB_GYRO_Y_BIAS;
    calib->gyro_z_bias = CALIB_GYRO_Z_BIAS;

    calib->gyro_x_scale = CALIB_GYRO_X_SCALE;
    calib->gyro_y_scale = CALIB_GYRO_Y_SCALE;
    calib->gyro_z_scale = CALIB_GYRO_Z_SCALE;

    calib->board_pitch_offset = 0.0f;
    calib->board_roll_offset = 0.0f;
    
    calib->state = CALIB_STATE_IDLE;
    
    for(int i=0; i<3; i++) {
        calib->acc_variance[i] = 0.0f;
        calib->gyro_variance[i] = 0.0f;
    }
}

void SensorCalibApply(const SensorCalib_t *calib, DataCenter *dataC) {
    if (!calib || !dataC) return;
    
    // --- Ã„Â°vmeÃƒÂ¶lÃƒÂ§er Kalibrasyonu (Birim: g -> m/sÃ‚Â²) ---
    // GerÃƒÂ§ek deÃ„Å¸er = (Ãƒâ€“lÃƒÂ§ÃƒÂ¼len - Bias) * Scale
    float ax_g = (dataC->acc.x.rawValue - calib->acc_x_bias) * calib->acc_x_scale;
    float ay_g = (dataC->acc.y.rawValue - calib->acc_y_bias) * calib->acc_y_scale;
    float az_g = (dataC->acc.z.rawValue - calib->acc_z_bias) * calib->acc_z_scale;
    
    // Ãƒâ€¡oÃ„Å¸u uÃƒÂ§uÃ…Å¸ filtresi m/sÃ‚Â² kullanÃ„Â±r, 1g = 9.80665 m/sÃ‚Â²
    dataC->acc.x.calibratedValue = ax_g * 9.80665f;
    dataC->acc.y.calibratedValue = ay_g * 9.80665f;
    dataC->acc.z.calibratedValue = az_g * 9.80665f;
    
    // --- Jiroskop Kalibrasyonu (Birim: derece/sn -> radyan/sn) ---
    float gx_dps = (dataC->gyro.x.rawValue - calib->gyro_x_bias) * calib->gyro_x_scale;
    float gy_dps = (dataC->gyro.y.rawValue - calib->gyro_y_bias) * calib->gyro_y_scale;
    float gz_dps = (dataC->gyro.z.rawValue - calib->gyro_z_bias) * calib->gyro_z_scale;
    
    // Ãƒâ€¡oÃ„Å¸u uÃƒÂ§uÃ…Å¸ filtresi rad/s kullanÃ„Â±r (pi / 180 = 0.0174533f)
    dataC->gyro.x.calibratedValue = gx_dps * 0.01745329251f;
    dataC->gyro.y.calibratedValue = gy_dps * 0.01745329251f;
    dataC->gyro.z.calibratedValue = gz_dps * 0.01745329251f;
}

void SensorCalib_ApplyBoardAlignment(const SensorCalib_t *calib, float *pitch, float *roll) {
    if (!calib || !pitch || !roll) return;
    *pitch -= calib->board_pitch_offset;
    *roll  -= calib->board_roll_offset;
}

void SensorCalib_SetLevel(SensorCalib_t *calib, float current_pitch, float current_roll) {
    if (!calib) return;
    calib->board_pitch_offset = current_pitch;
    calib->board_roll_offset = current_roll;
}

void SensorCalib_StartAdvanced(SensorCalib_t *calib) {
    if (!calib) return;
    
    calib->state = CALIB_STATE_TRANSITION;
    calib->current_step = 0;
    calib->timer_start_ms = 0; // Ã„Â°lk dÃƒÂ¶ngÃƒÂ¼de gÃƒÂ¼ncellenecek
    calib->sample_count = 0;
    
    for(int i=0; i<3; i++) {
        for(int s=0; s<6; s++) {
            calib->step_averages[s][i] = 0.0f;
        }
        calib->acc_M2[i] = 0.0f;
        
        calib->gyro_avg_accum[i] = 0.0f;
        calib->gyro_M2[i] = 0.0f;
    }
    
    if (calib->on_transition_start) {
        calib->on_transition_start(calib->current_step);
    }
}

void SensorCalib_ProcessAdvanced(SensorCalib_t *calib, DataCenter *dataC, uint32_t current_time_ms) {
    if (!calib || !dataC) return;
    if (calib->state == CALIB_STATE_IDLE || calib->state == CALIB_STATE_FINISHED) return;

    if (calib->timer_start_ms == 0) {
        calib->timer_start_ms = current_time_ms;
    }

    uint32_t elapsed = current_time_ms - calib->timer_start_ms;

    if (calib->state == CALIB_STATE_TRANSITION) {
        if (elapsed >= CALIB_TRANSITION_TIME_MS) {
            // SÃƒÂ¼re doldu, ÃƒÂ¶lÃƒÂ§ÃƒÂ¼me geÃƒÂ§
            calib->state = CALIB_STATE_MEASURING;
            calib->timer_start_ms = current_time_ms;
            calib->sample_count = 0;
            
            if (calib->on_measuring_start) {
                calib->on_measuring_start(calib->current_step);
            }
        }
    } 
    else if (calib->state == CALIB_STATE_MEASURING) {
        if (elapsed < CALIB_MEASURING_TIME_MS) {
            // Ãƒâ€“LÃƒâ€¡ÃƒÅ“M YAPIYORUZ - Recursive Mean ve Welford's Variance
            calib->sample_count++;
            uint32_t n = calib->sample_count;
            
            // X, Y, Z Eksenleri iÃƒÂ§in Welford
            for(int i=0; i<3; i++) {
                float acc_val = (i == 0) ? dataC->acc.x.rawValue : ((i == 1) ? dataC->acc.y.rawValue : dataC->acc.z.rawValue);
                float gyro_val = (i == 0) ? dataC->gyro.x.rawValue : ((i == 1) ? dataC->gyro.y.rawValue : dataC->gyro.z.rawValue);
                
                // Ã„Â°vmeÃƒÂ¶lÃƒÂ§er OrtalamasÃ„Â± (Her adÃ„Â±m iÃƒÂ§in o anki 3 eksen vektÃƒÂ¶rÃƒÂ¼nÃƒÂ¼ ayrÃ„Â± kaydet)
                float *acc_target = &calib->step_averages[calib->current_step][i];
                
                // Recursive Mean: new_mean = old_mean + (value - old_mean) / n
                float acc_delta = acc_val - *acc_target;
                *acc_target += acc_delta / (float)n;
                float acc_delta2 = acc_val - *acc_target;
                calib->acc_M2[i] += acc_delta * acc_delta2; // Varyans iÃƒÂ§in M2 toplayÃ„Â±cÃ„Â±
                
                // Jiroskop OrtalamasÃ„Â±
                // Jiroskop her adÃ„Â±mda 0 olmalÃ„Â±dÃ„Â±r, bu yÃƒÂ¼zden tÃƒÂ¼m adÃ„Â±mlarda kÃƒÂ¼mÃƒÂ¼latif toplayabiliriz.
                // GerÃƒÂ§ek n = (current_step * toplam_ÃƒÂ¶rnek) + n olur, ama basitleÃ…Å¸tirmek iÃƒÂ§in her adÃ„Â±mÃ„Â± ayrÃ„Â± toplayÃ„Â±p ortalayacaÃ„Å¸Ã„Â±z.
                // Burada sadece o anki adÃ„Â±mÃ„Â±n ortalamasÃ„Â±nÃ„Â± alÃ„Â±p sonra genel toplama ekleriz (State deÃ„Å¸iÃ…Å¸iminde)
                float gyro_delta = gyro_val - calib->gyro_avg_accum[i];
                calib->gyro_avg_accum[i] += gyro_delta / (float)n;
                float gyro_delta2 = gyro_val - calib->gyro_avg_accum[i];
                calib->gyro_M2[i] += gyro_delta * gyro_delta2;
            }
        } 
        else {
            // Ãƒâ€“lÃƒÂ§ÃƒÂ¼m bitti, sonraki adÃ„Â±ma geÃƒÂ§
            calib->current_step++;
            
            if (calib->current_step < 6) {
                calib->state = CALIB_STATE_TRANSITION;
                calib->timer_start_ms = current_time_ms;
                
                if (calib->on_transition_start) {
                    calib->on_transition_start(calib->current_step);
                }
            } else {
                // TÃƒÂ¼mÃƒÂ¼ Bitti! Nihai hesaplamalarÃ„Â± yap
                calib->state = CALIB_STATE_FINISHED;
                
            // Toplam ÃƒÂ¶rnek sayÃ„Â±sÃ„Â±nÃ„Â± hesapla (6 adÃ„Â±m x AdÃ„Â±m baÃ…Å¸Ã„Â±na ÃƒÂ¶rnek sayÃ„Â±sÃ„Â±)
            // (Ãƒâ€“rnek: Her adÃ„Â±mda n=5000 ÃƒÂ¶lÃƒÂ§ÃƒÂ¼m yapÃ„Â±ldÃ„Â±ysa total_n = 30000)
            uint32_t total_n = calib->sample_count * 6;
            if (total_n < 2) total_n = 2; // BÃƒÂ¶lme hatasÃ„Â±nÃ„Â± ÃƒÂ¶nle
            
            for(int i=0; i<3; i++) {
                // Her eksen iÃƒÂ§in 6 adÃ„Â±mdaki en bÃƒÂ¼yÃƒÂ¼k ve en kÃƒÂ¼ÃƒÂ§ÃƒÂ¼k ortalamayÃ„Â± bul (Min/Max Method)
                float max_val = -999999.0f;
                float min_val =  999999.0f;
                
                for(int s=0; s<6; s++) {
                    if(calib->step_averages[s][i] > max_val) max_val = calib->step_averages[s][i];
                    if(calib->step_averages[s][i] < min_val) min_val = calib->step_averages[s][i];
                }
                
                // 1. Ã„Â°vmeÃƒÂ¶lÃƒÂ§er Bias ve Scale
                float bias = (max_val + min_val) / 2.0f;
                // 1g ve -1g arasÃ„Â± fark 2g'dir. Scale faktÃƒÂ¶rÃƒÂ¼ bunu normalize eder:
                float scale = 2.0f / (max_val - min_val);
                
                if (i == 0) { calib->acc_x_bias = bias; calib->acc_x_scale = scale; }
                if (i == 1) { calib->acc_y_bias = bias; calib->acc_y_scale = scale; }
                if (i == 2) { calib->acc_z_bias = bias; calib->acc_z_scale = scale; }
                
                // 2. Jiroskop Bias ve Scale
                // Jiroskop her adÃ„Â±mda 0 okumasÃ„Â± gerektiÃ„Å¸inden, ortalamasÃ„Â± direkt Bias'tÃ„Â±r.
                // Scale faktÃƒÂ¶rÃƒÂ¼ jiroskop iÃƒÂ§in kalibratÃƒÂ¶rde 1.0 bÃ„Â±rakÃ„Â±lÃ„Â±r, eksenler baÃ„Å¸Ã„Â±msÃ„Â±zdÃ„Â±r.
                float g_bias = calib->gyro_avg_accum[i];
                if (i == 0) { calib->gyro_x_bias = g_bias; calib->gyro_x_scale = 1.0f; calib->gyro_avg[0] = g_bias; }
                if (i == 1) { calib->gyro_y_bias = g_bias; calib->gyro_y_scale = 1.0f; calib->gyro_avg[1] = g_bias; }
                if (i == 2) { calib->gyro_z_bias = g_bias; calib->gyro_z_scale = 1.0f; calib->gyro_avg[2] = g_bias; }
                
                // 3. Welford's Variance KayÃ„Â±t (Standart SapmanÃ„Â±n Karesi)
                calib->acc_variance[i] = calib->acc_M2[i] / (float)(total_n - 1);
                calib->gyro_variance[i] = calib->gyro_M2[i] / (float)(total_n - 1);
            }
            
            if (calib->on_calibration_finished != NULL) {
                    calib->on_calibration_finished();
                }
            }
        }
    }
}
void SensorCalib_QuickRun(SensorCalib_t *calib, DataCenter *data, uint32_t current_time_ms)
{
    if(calib->state == CALIB_STATE_FINISHED)
        return;

    if(calib->state == CALIB_STATE_IDLE)
    {
        // 5 saniye bekle
        calib->timer_start_ms = current_time_ms;
        calib->state = CALIB_STATE_TRANSITION;
        HAL_GPIO_WritePin(CALIB_LED_ORANGE_GPIO_Port, CALIB_LED_ORANGE_Pin, GPIO_PIN_SET);
        return;
    }

    if(calib->state == CALIB_STATE_TRANSITION)
    {
        if(current_time_ms - calib->timer_start_ms > CALIB_TRANSITION_TIME_MS)
        {
            calib->timer_start_ms = current_time_ms;
            calib->sample_count = 0;
            
            // X, Y, Z icin temizle
            for(int i=0; i<3; i++) {
                calib->acc_sum[i] = 0.0f;
                calib->acc_sum_sq[i] = 0.0f;
                calib->gyro_sum[i] = 0.0f;
                calib->gyro_sum_sq[i] = 0.0f;
            }

            calib->state = CALIB_STATE_MEASURING;
            HAL_GPIO_WritePin(CALIB_LED_ORANGE_GPIO_Port, CALIB_LED_ORANGE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CALIB_LED_RED_GPIO_Port, CALIB_LED_RED_Pin, GPIO_PIN_SET);
        }
        return;
    }

    if(calib->state == CALIB_STATE_MEASURING)
    {
        // Sadece X(0), Y(1), Z(2) icin veri topla
        calib->acc_sum[0] += data->acc.x.rawValue;
        calib->acc_sum_sq[0] += (data->acc.x.rawValue * data->acc.x.rawValue);
        calib->acc_sum[1] += data->acc.y.rawValue;
        calib->acc_sum_sq[1] += (data->acc.y.rawValue * data->acc.y.rawValue);
        calib->acc_sum[2] += data->acc.z.rawValue;
        calib->acc_sum_sq[2] += (data->acc.z.rawValue * data->acc.z.rawValue);
        
        calib->gyro_sum[0] += data->gyro.x.rawValue;
        calib->gyro_sum_sq[0] += (data->gyro.x.rawValue * data->gyro.x.rawValue);
        calib->gyro_sum[1] += data->gyro.y.rawValue;
        calib->gyro_sum_sq[1] += (data->gyro.y.rawValue * data->gyro.y.rawValue);
        calib->gyro_sum[2] += data->gyro.z.rawValue;
        calib->gyro_sum_sq[2] += (data->gyro.z.rawValue * data->gyro.z.rawValue);
        
        calib->sample_count++;

        if(current_time_ms - calib->timer_start_ms > CALIB_MEASURING_TIME_MS)
        {
            float n = (float)calib->sample_count;
            
            // 1. Bias (Ortalama) Hesaplama
            calib->acc_x_bias = calib->acc_sum[0] / n;
            calib->acc_y_bias = calib->acc_sum[1] / n;
            // Z ekseninde yercekimi var, onu cikariyoruz
            calib->acc_z_bias = (calib->acc_sum[2] / n) - 1.0f;
            
            calib->gyro_x_bias = calib->gyro_sum[0] / n;
            calib->gyro_y_bias = calib->gyro_sum[1] / n;
            calib->gyro_z_bias = calib->gyro_sum[2] / n;
            
            // 2. Scale 1.0 kabul ediliyor
            calib->acc_x_scale = 1.0f;
            calib->acc_y_scale = 1.0f;
            calib->acc_z_scale = 1.0f;
            
            calib->gyro_x_scale = 1.0f;
            calib->gyro_y_scale = 1.0f;
            calib->gyro_z_scale = 1.0f;
            
            // 3. Noise (Varyans) Hesaplama
            calib->acc_variance[0] = (calib->acc_sum_sq[0] / n) - (calib->acc_x_bias * calib->acc_x_bias);
            calib->acc_variance[1] = (calib->acc_sum_sq[1] / n) - (calib->acc_y_bias * calib->acc_y_bias);
            calib->acc_variance[2] = (calib->acc_sum_sq[2] / n) - ((calib->acc_sum[2]/n) * (calib->acc_sum[2]/n));
            
            calib->gyro_variance[0] = (calib->gyro_sum_sq[0] / n) - (calib->gyro_x_bias * calib->gyro_x_bias);
            calib->gyro_variance[1] = (calib->gyro_sum_sq[1] / n) - (calib->gyro_y_bias * calib->gyro_y_bias);
            calib->gyro_variance[2] = (calib->gyro_sum_sq[2] / n) - (calib->gyro_z_bias * calib->gyro_z_bias);
            
            // Bitir
            calib->state = CALIB_STATE_FINISHED;
            HAL_GPIO_WritePin(CALIB_LED_RED_GPIO_Port, CALIB_LED_RED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CALIB_LED_GREEN_GPIO_Port, CALIB_LED_GREEN_Pin, GPIO_PIN_SET);
            
            CalibrationUI_PrintResults(calib);
        }
    }
}



