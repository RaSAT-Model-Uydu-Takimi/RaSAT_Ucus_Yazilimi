/*
 * motors.h
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#ifndef INC_MOTORS_H_
#define INC_MOTORS_H_

#include <stdint.h>



/* ESC'leri başlat */
void Motors_Init(void);

/* ESC'leri arm et */
void Motors_Arm(void);

/* ESC'leri güvenli moda al */
void Motors_Disarm(void);

/* Bütün motorları durdur */
void Motors_StopAll(void);

/* Throttle gönder (0.0 - 1.0) */
void Motors_SetThrottle(float throttle);

/* Son throttle değerini oku */
float Motors_GetThrottle(void);

/* Son PWM değerini oku */
uint16_t Motors_GetPWM(void);




#endif /* INC_MOTORS_H_ */
