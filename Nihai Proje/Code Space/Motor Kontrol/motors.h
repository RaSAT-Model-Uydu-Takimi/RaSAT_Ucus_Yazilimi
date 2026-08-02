/*
 * motors.h
 *
 * Dört ESC/motorun ayrı ayrı PWM ile kontrol edilmesi.
 */

#ifndef INC_MOTORS_H_
#define INC_MOTORS_H_

#include "main.h"
#include <stdint.h>

typedef enum
{
    MOTOR_1 = 0,
    MOTOR_2,
    MOTOR_3,
    MOTOR_4,
    MOTOR_COUNT
} MotorId_t;

typedef struct
{
    float m1;
    float m2;
    float m3;
    float m4;
} MotorOutputs_t;

/* PWM kanallarını başlatır. Başarılıysa 1 döndürür. */
uint8_t Motors_Init(TIM_HandleTypeDef *timer);

/* ESC durum yönetimi */
void Motors_Arm(void);
void Motors_Disarm(void);
uint8_t Motors_IsArmed(void);

/* Bütün motorları güvenli çıkış değerine alır. */
void Motors_StopAll(void);

/* Eski kullanımla uyumlu: bütün motorlara aynı throttle değerini verir. */
void Motors_SetThrottle(float throttle);

/* Tek bir motoru kontrol eder. Throttle aralığı: 0.0f - 1.0f */
void Motors_SetMotorThrottle(MotorId_t motor, float throttle);

/* Dört motora birbirinden farklı throttle değerleri verir. */
void Motors_SetOutputs(const MotorOutputs_t *outputs);

/* Son gönderilen değerleri okur. */
float Motors_GetMotorThrottle(MotorId_t motor);
uint16_t Motors_GetMotorPWM(MotorId_t motor);

#endif /* INC_MOTORS_H_ */
