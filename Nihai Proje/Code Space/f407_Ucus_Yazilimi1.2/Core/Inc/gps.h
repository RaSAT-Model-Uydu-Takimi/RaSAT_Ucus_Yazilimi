/*
 * gps.h
 *
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

/*
 * GPS NMEA parser driver
 * BN-880 / BN-220 gibi UART NMEA GPS modülleri için.
 */
#include <stdint.h>
#include "main.h"


typedef struct
{
    double latitude_deg;
    double longitude_deg;

    float altitude_m;
    float speed_mps;
    float course_deg;
    float hdop; // maks 100, min:0    ne kadar küçük o kadar iyi. umut kalibre olma süresinde 99 almış. kalibre olduktan sonra 5'i görmüş. cam kenarına çıkınca 0.9 almış bu değeri.

    uint8_t satellites;
    uint8_t fix_quality; // maks 5 min 0    0 -> konum geçersiz 1 normal 2 daha iyi 4 en hassas 5 4 kadar hassas değil.
    uint8_t fix_valid;

    uint8_t new_data;

} GPS_Data;


uint8_t GPS_Init(UART_HandleTypeDef *uart_handle);

uint8_t GPS_StartReceiveIT(void);

void GPS_UART_RxCpltCallback(UART_HandleTypeDef *uart_handle);

void GPS_ProcessByte(uint8_t byte);

uint8_t GPS_Read(GPS_Data *data);

uint8_t GPS_IsReady(void);


#endif /* INC_GPS_H_ */
