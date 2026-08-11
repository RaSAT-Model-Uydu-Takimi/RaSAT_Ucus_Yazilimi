/*
 * dshot.h
 *
 *  Created on: Aug 9, 2026
 *      Author: abduh
 */

#ifndef INC_DSHOT_H_
#define INC_DSHOT_H_


#define SIFIR_BITI_TICK 53
#define BIR_BITI_TICK 105

#define ADCRAW_to_AMPER_FACTOR 0.0208f

#include "main.h" // BU SATIRI EKLE

void Dshot_Init_1(TIM_HandleTypeDef *htim_signalGenerator, TIM_HandleTypeDef *htim_communicator);
void Dshot_Init_2(TIM_HandleTypeDef *htim_signalGenerator, TIM_HandleTypeDef *htim_communicator, ADC_HandleTypeDef *adc_current);
void Motors_Arm(uint32_t firstDelay_ms, uint32_t secondDelay_ms);
void Motors_Disarm(uint32_t firstDelay_ms, uint32_t secondDelay_ms);
void Dshot_Frame_End_Callback(TIM_HandleTypeDef *htim);
void DSHOT_Prepare_Packet(uint16_t throttle, uint16_t* buffer);
void Dshot_Communicator_Timer_Callback(TIM_HandleTypeDef *htim, uint16_t signals[4]);
uint32_t Read_RawADC_Current_Value();
float Get_ESC_Current(uint32_t adcrawvalue);









#endif /* INC_DSHOT_H_ */
