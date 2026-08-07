#include "M3.3_EKFAttitude.h"
#include "M0.1_FilterConfig.h"
#include "M3.1_MatrixOps.h"
#include <math.h>
#include <string.h>

// Donanımsal FPU (1.0f / sqrtf(x)) kullanılacak. Quake III hack'ine gerek yok!

/* ========================================================================== */
/*  M3_Attitude_Init                                                          */
/* ========================================================================== */
void M3_Attitude_Init(M3_EKF_Attitude_t *ekf) {
    if (ekf == NULL) return;

    memset(ekf->x, 0, sizeof(ekf->x));
    ekf->x[0] = 1.0f; // q0
    // q1=0, q2=0, q3=0
    // bias_x=0, bias_y=0, bias_z=0

    memset(ekf->P, 0, sizeof(ekf->P));
    // Başlangıç belirsizlikleri
    for (int i = 0; i < 4; i++) ekf->P[i * 7 + i] = 1.0f;     /* Kuaterniyonlar */
    for (int i = 4; i < 7; i++) ekf->P[i * 7 + i] = 0.01f;    /* Biaslar */
}

/* ========================================================================== */
/*  M3_Attitude_Update                                                        */
/* ========================================================================== */
void M3_Attitude_Update(M3_EKF_Attitude_t *ekf, DataCenter *dataC, float dt_seconds) {
    if (ekf == NULL || dataC == NULL || dt_seconds <= 0.0f) return;

    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;

    float gx = dataC->gyro.x.calibratedValue;
    float gy = dataC->gyro.y.calibratedValue;
    float gz = dataC->gyro.z.calibratedValue;

    float q0 = ekf->x[0];
    float q1 = ekf->x[1];
    float q2 = ekf->x[2];
    float q3 = ekf->x[3];
    float bx = ekf->x[4];
    float by = ekf->x[5];
    float bz = ekf->x[6];
    
    // Bias'ı çıkar (Integral kazancı eklersek buralar güncellenecek)
    gx -= bx;
    gy -= by;
    gz -= bz;

    /* ================================================================== */
    /*  MAHONY COMPLEMENTARY FILTER (TAMAMLAYICI FİLTRE) ADIMI            */
    /*  Kullanıcının isteği üzerine ağır EKF matrisleri yerine endüstri   */
    /*  standardı olan Mahony filtresi entegre edilmiştir.                */
    /* ================================================================== */
    
    // Sadece ivmeölçer mantıklı veriler üretiyorsa düzeltme yap
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        
        // İvmeölçer vektörünü donanımsal FPU ile normalize et
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

        // Oransal (Proportional) düzeltmeyi jiroskoba uygula
        gx += (ORIENTATION_SYSTEM_GAIN * 2.0f) * halfex;
        gy += (ORIENTATION_SYSTEM_GAIN * 2.0f) * halfey;
        gz += (ORIENTATION_SYSTEM_GAIN * 2.0f) * halfez;
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
    ekf->x[0] = q0 * recipNorm;
    ekf->x[1] = q1 * recipNorm;
    ekf->x[2] = q2 * recipNorm;
    ekf->x[3] = q3 * recipNorm;

    /* ================================================================== */
    /*  3. ÇIKTILAR (DataCenter'a Kayıt)                                  */
    /* ================================================================== */
    q0 = ekf->x[0]; q1 = ekf->x[1]; q2 = ekf->x[2]; q3 = ekf->x[3];

    // Euler Açıları (Yaw dahil - fakat M4 bunu ezecek/düzeltecektir)
    dataC->estimated.roll.value  = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * RAD2DEG;
    float sinP = 2.0f * (q0 * q2 - q3 * q1);
    if (sinP >  1.0f) sinP =  1.0f;
    if (sinP < -1.0f) sinP = -1.0f;
    dataC->estimated.pitch.value = -asinf(sinP) * RAD2DEG; // Eksi işareti ile Havacılık Standardı (Burun Yukarı = Pozitif)
    dataC->estimated.yaw.value = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3)) * RAD2DEG;

    dataC->estimated.q0.value = q0;
    dataC->estimated.q1.value = q1;
    dataC->estimated.q2.value = q2;
    dataC->estimated.q3.value = q3;

    // Jiroskop Bias Kayıtları
    dataC->estimated.gyro_bias_x.value = ekf->x[4];
    dataC->estimated.gyro_bias_y.value = ekf->x[5];
    dataC->estimated.gyro_bias_z.value = ekf->x[6];

    // Tahmin güveni (Kovaryansın köşegenine ters orantılı)
    float conf = 1.0f / (1.0f + ekf->P[0]); // q0 belirsizliği
    dataC->estimated.pitch.confidence = conf;
    dataC->estimated.roll.confidence  = conf;
    dataC->estimated.q0.confidence    = conf;
    dataC->estimated.q1.confidence    = conf;
    dataC->estimated.q2.confidence    = conf;
    dataC->estimated.q3.confidence    = conf;
    
    dataC->estimated.gyro_bias_x.confidence = 1.0f / (1.0f + ekf->P[4*7+4]);
    dataC->estimated.gyro_bias_y.confidence = 1.0f / (1.0f + ekf->P[5*7+5]);
    dataC->estimated.gyro_bias_z.confidence = 1.0f / (1.0f + ekf->P[6*7+6]);
}
