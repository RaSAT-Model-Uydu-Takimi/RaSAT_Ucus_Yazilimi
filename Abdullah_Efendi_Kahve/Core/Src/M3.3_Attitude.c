#include "M3.3_Attitude.h"
#include "M0.1_FilterConfig.h"
#include <math.h>
#include <string.h>

// Donanmsal FPU (1.0f / sqrtf(x)) kullanlacak.

/* ========================================================================== */
/*  M3_Attitude_Init                                                          */
/* ========================================================================== */
void M3_Attitude_Init(M3_Attitude_t *attitude) {
    if (attitude == NULL) return;

    memset(attitude->q, 0, sizeof(attitude->q));
    attitude->q[0] = 1.0f; // q0
    // q1=0, q2=0, q3=0

    memset(attitude->integralFB, 0, sizeof(attitude->integralFB));
    // integralFB_x=0, integralFB_y=0, integralFB_z=0
}

/* ========================================================================== */
/*  M3_Attitude_Update                                                        */
/* ========================================================================== */
void M3_Attitude_Update(M3_Attitude_t *attitude, DataCenter *dataC, float dt_seconds) {
    if (attitude == NULL || dataC == NULL || dt_seconds <= 0.0f) return;

    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;

    float gx = dataC->gyro.x.calibratedValue;
    float gy = dataC->gyro.y.calibratedValue;
    float gz = dataC->gyro.z.calibratedValue;

    float q0 = attitude->q[0];
    float q1 = attitude->q[1];
    float q2 = attitude->q[2];
    float q3 = attitude->q[3];
    
    // Mahony Filter kazançları
    float Kp = ORIENTATION_SYSTEM_GAIN * 2.0f;
    float Ki = Kp * 0.1f; // Integral gain, Kp'nin %10'u kadar (ince ayar yapılabilir)

    /* ================================================================== */
    /*  MAHONY COMPLEMENTARY FILTER (TAMAMLAYICI FİLTRE) ADIMI            */
    /* ================================================================== */
    
    // Sadece ivmeölçer mantıklı veriler üretiyorsa düzeltme yap
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        
        // İvmeölçer vektörünü normalize et
        float recipNorm = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Kuaterniyon üzerinden hesaplanan Tahmini Yerçekimi Yönü
        float halfvx = q1 * q3 - q0 * q2;
        float halfvy = q0 * q1 + q2 * q3;
        float halfvz = q0 * q0 - 0.5f + q3 * q3;

        // Hata: Tahmin edilen yerçekimi ile ölçülen ivme arasındaki vektörel çarpım (Cross Product)
        float halfex = (ay * halfvz - az * halfvy);
        float halfey = (az * halfvx - ax * halfvz);
        float halfez = (ax * halfvy - ay * halfvx);

        // Integral hatayı hesapla ve biriktir (Ki > 0 ise)
        if (Ki > 0.0f) {
            attitude->integralFB[0] += Ki * halfex * dt_seconds;
            attitude->integralFB[1] += Ki * halfey * dt_seconds;
            attitude->integralFB[2] += Ki * halfez * dt_seconds;
            
            // Integral hatayı jiroskoba uygula
            gx += attitude->integralFB[0];
            gy += attitude->integralFB[1];
            gz += attitude->integralFB[2];
        } else {
            attitude->integralFB[0] = 0.0f;
            attitude->integralFB[1] = 0.0f;
            attitude->integralFB[2] = 0.0f;
        }

        // Oransal (Proportional) düzeltmeyi jiroskoba uygula
        gx += Kp * halfex;
        gy += Kp * halfey;
        gz += Kp * halfez;
    }

    // Jiroskop verisini kullanarak kuaterniyon türevini entegre et
    gx *= (0.5f * dt_seconds);
    gy *= (0.5f * dt_seconds);
    gz *= (0.5f * dt_seconds);
    
    float qa = q0;
    float qb = q1;
    float qc = q2;
    
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // Yeni kuaterniyonu normalize et
    float recipNorm = 1.0f / sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    attitude->q[0] = q0 * recipNorm;
    attitude->q[1] = q1 * recipNorm;
    attitude->q[2] = q2 * recipNorm;
    attitude->q[3] = q3 * recipNorm;

    /* ================================================================== */
    /*  ÇIKTILAR (DataCenter'a Kayıt)                                     */
    /* ================================================================== */
    q0 = attitude->q[0]; q1 = attitude->q[1]; q2 = attitude->q[2]; q3 = attitude->q[3];

    // Euler Açıları (Yaw dahil - fakat M4 bunu ezecek/düzeltecektir)
    dataC->estimated.roll.value  = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * RAD2DEG;
    float sinP = 2.0f * (q0 * q2 - q3 * q1);
    if (sinP >  1.0f) sinP =  1.0f;
    if (sinP < -1.0f) sinP = -1.0f;
    dataC->estimated.pitch.value = -asinf(sinP) * RAD2DEG; // Eksi işareti ile Havacılık Standardı
    dataC->estimated.yaw.value = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3)) * RAD2DEG;

    dataC->estimated.q0.value = q0;
    dataC->estimated.q1.value = q1;
    dataC->estimated.q2.value = q2;
    dataC->estimated.q3.value = q3;

    // Jiroskop Bias Kayıtları (Ters işaretli olarak tutulabilir, integral hatası bias'ı temsil eder)
    dataC->estimated.gyro_bias_x.value = -attitude->integralFB[0];
    dataC->estimated.gyro_bias_y.value = -attitude->integralFB[1];
    dataC->estimated.gyro_bias_z.value = -attitude->integralFB[2];

    // Mahony'de EKF gibi bir kovaryans matrisi (P) yoktur. 
    // Ancak confidence olarak 1.0f gönderebiliriz.
    float conf = 1.0f; 
    dataC->estimated.pitch.confidence = conf;
    dataC->estimated.roll.confidence  = conf;
    dataC->estimated.q0.confidence    = conf;
    dataC->estimated.q1.confidence    = conf;
    dataC->estimated.q2.confidence    = conf;
    dataC->estimated.q3.confidence    = conf;
    
    dataC->estimated.gyro_bias_x.confidence = conf;
    dataC->estimated.gyro_bias_y.confidence = conf;
    dataC->estimated.gyro_bias_z.confidence = conf;
}
