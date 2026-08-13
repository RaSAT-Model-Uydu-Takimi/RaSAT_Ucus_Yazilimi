/*
 * M3.4_Altitude.c
 *
 * İrtifa ve Dikey Hız Hesaplama Modülü (2-Durumlu Kalman Filtresi)
 */

#include "M3.4_Altitude.h"
#include "M0.1_FilterConfig.h"
#include <math.h>

// Kalman State Variables
static float X_alt = 0.0f; // Tahmini İrtifa (m)
static float X_vel = 0.0f; // Tahmini Dikey Hız (m/s)

// Kalman Covariance Matrix (P)
static float P00 = 1.0f;
static float P01 = 0.0f;
static float P10 = 0.0f;
static float P11 = 1.0f;

void M3_4_Altitude_Init(DataCenter *dc) {
    X_alt = 0.0f;
    X_vel = 0.0f;
    
    P00 = 1.0f;
    P01 = 0.0f;
    P10 = 0.0f;
    P11 = 1.0f;
    
    dc->estimated.altitude = 0.0f;
    dc->estimated.vertical_velocity = 0.0f;
}

void M3_4_Altitude_Update(DataCenter *dc, float dt) {
    // 1. İvmeölçer Verisini Earth Frame'e (Dünya Eksenine) Döndür
    // (İvmeölçer 1G biriminde okunduğu varsayılmıştır)
    float ax = dc->acc.calibrated_x;
    float ay = dc->acc.calibrated_y;
    float az = dc->acc.calibrated_z;
    
    float q0 = dc->estimated.q0;
    float q1 = dc->estimated.q1;
    float q2 = dc->estimated.q2;
    float q3 = dc->estimated.q3;
    
    // Z ekseni dönüşüm vektörü (Body'den Earth'e Z yansıması)
    float v_z_x = 2.0f * (q1*q3 - q0*q2);
    float v_z_y = 2.0f * (q0*q1 + q2*q3);
    float v_z_z = q0*q0 - q1*q1 - q2*q2 + q3*q3;
    
    // Dünya eksenindeki Z ivmesi (G cinsinden)
    float a_z_earth = (ax * v_z_x) + (ay * v_z_y) + (az * v_z_z);
    
    // Net dikey ivme: Yerçekimini (1G) çıkar ve m/s^2'ye çevir
    float net_acc_z_ms2 = (a_z_earth - 1.0f) * GRAVITY_MSS;
    
    // 2. TAHMİN (PREDICT) ADIMI
    // X = F * X + B * u
    float predicted_alt = X_alt + (X_vel * dt) + (0.5f * net_acc_z_ms2 * dt * dt);
    float predicted_vel = X_vel + (net_acc_z_ms2 * dt);
    
    // Süreç Gürültüsü (Q) Hesaplaması
    // İvmeölçer gürültüsü (G^2 cinsinden), (m/s^2)^2'ye çeviriyoruz
    float acc_var_ms4 = dc->calibProfile.acc_noise_z * GRAVITY_MSS * GRAVITY_MSS;
    if(acc_var_ms4 < 0.001f) acc_var_ms4 = 0.001f;
    
    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    float dt4 = dt3 * dt;
    
    float Q00 = 0.25f * dt4 * acc_var_ms4;
    float Q01 = 0.5f  * dt3 * acc_var_ms4;
    float Q10 = 0.5f  * dt3 * acc_var_ms4;
    float Q11 = dt2 * acc_var_ms4;
    
    // P = F * P * F^T + Q
    float P00_pred = P00 + dt * (P10 + P01 + P11 * dt) + Q00;
    float P01_pred = P01 + P11 * dt + Q01;
    float P10_pred = P10 + P11 * dt + Q10;
    float P11_pred = P11 + Q11;
    
    // 3. BAROMETRE ÖLÇÜMÜ (Hipsometrik Formül)
    float press = dc->baro.calibrated_press;
    float p0 = dc->calibProfile.baro_press_bias; // Yer seviyesi basıncı
    if(p0 < 800.0f) p0 = 1013.25f; // Mantıksız bir değerse standarta dön
    if(press < 100.0f) press = p0; // Veri gelmiyorsa 0 metrede tut
    
    float alt_baro = 44330.0f * (1.0f - powf(press / p0, 0.190263f));
    
    // 4. GÜNCELLEME (UPDATE) ADIMI
    // Ölçüm Gürültüsü (R) - Basınç varyansını kabaca irtifa varyansına (m^2) çeviriyoruz
    // 1 hPa değişim deniz seviyesinde yaklaşık 8.4 metreye denk gelir (Karesi ~70).
    float R_baro = dc->calibProfile.baro_press_noise * 70.0f; 
    if(R_baro < 1.0f) R_baro = 1.0f; // Minimum 1m varyans
    
    // İnovasyon
    float Y = alt_baro - predicted_alt;
    
    // İnovasyon Kovaryansı (S) = P00 + R
    float S = P00_pred + R_baro;
    
    // Kalman Kazancı (K)
    float K0 = P00_pred / S;
    float K1 = P10_pred / S;
    
    // State Güncellemesi
    X_alt = predicted_alt + (K0 * Y);
    X_vel = predicted_vel + (K1 * Y);
    
    // P Matrisi Güncellemesi: P = (I - K*H) * P
    P00 = P00_pred - (K0 * P00_pred);
    P01 = P01_pred - (K0 * P01_pred);
    P10 = P10_pred - (K1 * P00_pred);
    P11 = P11_pred - (K1 * P01_pred);
    
    // 5. Sonuçları DataCenter'a Aktar
    dc->estimated.altitude = X_alt;
    dc->estimated.vertical_velocity = X_vel;
    dc->estimated.linear_acceleration_z = net_acc_z_ms2;
}
