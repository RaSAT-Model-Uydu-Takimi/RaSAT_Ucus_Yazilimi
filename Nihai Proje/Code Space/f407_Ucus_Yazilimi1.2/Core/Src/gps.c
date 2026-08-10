/*
 * gps.c
 *
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

/*
 * gps.c
 *
 * GPS NMEA parser driver
 *
 * Görev:
 * - UART üzerinden gelen NMEA byte'larını toplamak
 * - GGA ve RMC mesajlarını parse etmek
 * - latitude, longitude, altitude, speed, course üretmek
 */

#include "gps.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>


/*==================================================
 * AYARLAR
 *==================================================*/

#define GPS_LINE_BUFFER_SIZE       128U

#define GPS_KNOT_TO_MPS            0.514444f


/*==================================================
 * STATIC DEĞİŞKENLER
 *==================================================*/

static UART_HandleTypeDef *gps_uart = NULL;

static uint8_t gps_ready = 0U;

static uint8_t gps_rx_byte = 0U;

static char gps_line[GPS_LINE_BUFFER_SIZE];

static uint16_t gps_line_index = 0U;

static GPS_Data gps_data;


/*==================================================
 * YARDIMCI FONKSİYONLAR
 *==================================================*/

static uint8_t GPS_HexToNibble(char c)
{
    if((c >= '0') && (c <= '9'))
    {
        return (uint8_t)(c - '0');
    }

    if((c >= 'A') && (c <= 'F'))
    {
        return (uint8_t)(c - 'A' + 10);
    }

    if((c >= 'a') && (c <= 'f'))
    {
        return (uint8_t)(c - 'a' + 10);
    }

    return 0xFFU;
}


static uint8_t GPS_ChecksumIsValid(const char *sentence)
{
    const char *star;
    const char *p;

    uint8_t calculated = 0U;
    uint8_t high;
    uint8_t low;
    uint8_t received;

    if(sentence == NULL)
    {
        return 0U;
    }

    if(sentence[0] != '$')
    {
        return 0U;
    }

    star = strchr(sentence, '*');

    if(star == NULL)
    {
        return 0U;
    }

    if((star[1] == '\0') || (star[2] == '\0'))
    {
        return 0U;
    }

    p = &sentence[1];

    while((p < star) && (*p != '\0'))
    {
        calculated ^= (uint8_t)(*p);
        p++;
    }

    high = GPS_HexToNibble(star[1]);
    low = GPS_HexToNibble(star[2]);

    if((high == 0xFFU) || (low == 0xFFU))
    {
        return 0U;
    }

    received = (uint8_t)((high << 4) | low);

    if(calculated == received)
    {
        return 1U;
    }

    return 0U;
}


static uint8_t GPS_GetField(const char *sentence,
                            uint8_t field_index,
                            char *output,
                            uint16_t output_size)
{
    const char *p;
    const char *start;

    uint8_t current_field = 0U;
    uint16_t length;

    if((sentence == NULL) || (output == NULL) || (output_size == 0U))
    {
        return 0U;
    }

    output[0] = '\0';

    p = sentence;

    if(*p == '$')
    {
        p++;
    }

    while((*p != '\0') && (*p != '\r') && (*p != '\n'))
    {
        start = p;

        while((*p != ',') &&
              (*p != '*') &&
              (*p != '\0') &&
              (*p != '\r') &&
              (*p != '\n'))
        {
            p++;
        }

        if(current_field == field_index)
        {
            length = (uint16_t)(p - start);

            if(length >= output_size)
            {
                length = output_size - 1U;
            }

            memcpy(output, start, length);
            output[length] = '\0';

            return 1U;
        }

        if(*p == ',')
        {
            p++;
            current_field++;
        }
        else
        {
            break;
        }
    }

    return 0U;
}


static uint8_t GPS_ParseCoordinate(const char *coord_field,
                                   const char *direction_field,
                                   double *decimal_deg)
{
    double raw;
    int degrees;
    double minutes;
    double result;

    if((coord_field == NULL) ||
       (direction_field == NULL) ||
       (decimal_deg == NULL))
    {
        return 0U;
    }

    if((coord_field[0] == '\0') ||
       (direction_field[0] == '\0'))
    {
        return 0U;
    }

    /*
     * NMEA formatı:
     *
     * Latitude  -> ddmm.mmmmm
     * Longitude -> dddmm.mmmmm
     *
     * Decimal degree:
     * degree + minute / 60
     */
    raw = atof(coord_field);

    degrees = (int)(raw / 100.0);
    minutes = raw - ((double)degrees * 100.0);

    result = (double)degrees + (minutes / 60.0);

    if((direction_field[0] == 'S') ||
       (direction_field[0] == 'W'))
    {
        result = -result;
    }

    *decimal_deg = result;

    return 1U;
}


static void GPS_ParseGGA(const char *sentence)
{
    char lat_field[20];
    char ns_field[4];
    char lon_field[20];
    char ew_field[4];

    char quality_field[8];
    char satellites_field[8];
    char hdop_field[16];
    char altitude_field[16];

    double latitude;
    double longitude;

    uint8_t quality;

    if(GPS_GetField(sentence, 2U, lat_field, sizeof(lat_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 3U, ns_field, sizeof(ns_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 4U, lon_field, sizeof(lon_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 5U, ew_field, sizeof(ew_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 6U, quality_field, sizeof(quality_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 7U, satellites_field, sizeof(satellites_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 8U, hdop_field, sizeof(hdop_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 9U, altitude_field, sizeof(altitude_field)) == 0U)
    {
        return;
    }

    quality = (uint8_t)atoi(quality_field);

    gps_data.fix_quality = quality;
    gps_data.satellites = (uint8_t)atoi(satellites_field);
    gps_data.hdop = (float)atof(hdop_field);

    if(quality == 0U)
    {
        gps_data.fix_valid = 0U;
        gps_data.new_data = 1U;
        return;
    }

    if(GPS_ParseCoordinate(lat_field,
                           ns_field,
                           &latitude) == 0U)
    {
        return;
    }

    if(GPS_ParseCoordinate(lon_field,
                           ew_field,
                           &longitude) == 0U)
    {
        return;
    }

    gps_data.latitude_deg = latitude;
    gps_data.longitude_deg = longitude;
    gps_data.altitude_m = (float)atof(altitude_field);

    gps_data.fix_valid = 1U;
    gps_data.new_data = 1U;
}


static void GPS_ParseRMC(const char *sentence)
{
    char status_field[4];

    char lat_field[20];
    char ns_field[4];
    char lon_field[20];
    char ew_field[4];

    char speed_field[16];
    char course_field[16];

    double latitude;
    double longitude;

    if(GPS_GetField(sentence, 2U, status_field, sizeof(status_field)) == 0U)
    {
        return;
    }

    if(status_field[0] != 'A')
    {
        gps_data.fix_valid = 0U;
        gps_data.new_data = 1U;
        return;
    }

    if(GPS_GetField(sentence, 3U, lat_field, sizeof(lat_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 4U, ns_field, sizeof(ns_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 5U, lon_field, sizeof(lon_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 6U, ew_field, sizeof(ew_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 7U, speed_field, sizeof(speed_field)) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 8U, course_field, sizeof(course_field)) == 0U)
    {
        return;
    }

    if(GPS_ParseCoordinate(lat_field,
                           ns_field,
                           &latitude) == 0U)
    {
        return;
    }

    if(GPS_ParseCoordinate(lon_field,
                           ew_field,
                           &longitude) == 0U)
    {
        return;
    }

    gps_data.latitude_deg = latitude;
    gps_data.longitude_deg = longitude;

    /*
     * RMC speed field knot cinsindedir.
     */
    gps_data.speed_mps =
        (float)atof(speed_field) * GPS_KNOT_TO_MPS;

    gps_data.course_deg = (float)atof(course_field);

    gps_data.fix_valid = 1U;
    gps_data.new_data = 1U;
}


static void GPS_ParseSentence(const char *sentence)
{
    char message_id[8];
    uint16_t length;

    if(GPS_ChecksumIsValid(sentence) == 0U)
    {
        return;
    }

    if(GPS_GetField(sentence, 0U, message_id, sizeof(message_id)) == 0U)
    {
        return;
    }

    length = (uint16_t)strlen(message_id);

    if(length < 3U)
    {
        return;
    }

    /*
     * Talker ID değişebilir:
     *
     * GPGGA, GNGGA, GLGGA...
     * GPRMC, GNRMC...
     *
     * Bu yüzden son 3 karaktere bakıyoruz.
     */
    if(strcmp(&message_id[length - 3U], "GGA") == 0)
    {
        GPS_ParseGGA(sentence);
    }
    else if(strcmp(&message_id[length - 3U], "RMC") == 0)
    {
        GPS_ParseRMC(sentence);
    }
}


/*==================================================
 * PUBLIC FONKSİYONLAR
 *==================================================*/

uint8_t GPS_Init(UART_HandleTypeDef *uart_handle)
{
    if(uart_handle == NULL)
    {
        gps_ready = 0U;
        return 0U;
    }

    gps_uart = uart_handle;
    gps_ready = 1U;

    gps_line_index = 0U;

    memset(gps_line, 0, sizeof(gps_line));
    memset(&gps_data, 0, sizeof(gps_data));

    return 1U;
}


uint8_t GPS_StartReceiveIT(void)
{
    if((gps_ready == 0U) || (gps_uart == NULL))
    {
        return 0U;
    }

    if(HAL_UART_Receive_IT(gps_uart,
                           &gps_rx_byte,
                           1U) != HAL_OK)
    {
        return 0U;
    }

    return 1U;
}


void GPS_UART_RxCpltCallback(UART_HandleTypeDef *uart_handle)
{
    if((gps_ready == 0U) || (gps_uart == NULL))
    {
        return;
    }

    if(uart_handle != gps_uart)
    {
        return;
    }

    GPS_ProcessByte(gps_rx_byte);

    /*
     * Bir sonraki byte için UART interrupt tekrar başlatılır.
     */
    (void)HAL_UART_Receive_IT(gps_uart,
                              &gps_rx_byte,
                              1U);
}


void GPS_ProcessByte(uint8_t byte)
{
    if(byte == '$')
    {
        gps_line_index = 0U;
        gps_line[gps_line_index] = (char)byte;
        gps_line_index++;
        return;
    }

    if(byte == '\r')
    {
        return;
    }

    if(byte == '\n')
    {
        if(gps_line_index > 6U)
        {
            gps_line[gps_line_index] = '\0';
            GPS_ParseSentence(gps_line);
        }

        gps_line_index = 0U;
        return;
    }

    if(gps_line_index < (GPS_LINE_BUFFER_SIZE - 1U))
    {
        gps_line[gps_line_index] = (char)byte;
        gps_line_index++;
    }
    else
    {
        /*
         * Satır taşarsa parser'ı resetle.
         */
        gps_line_index = 0U;
    }
}


uint8_t GPS_Read(GPS_Data *data)
{
    if((data == NULL) || (gps_ready == 0U))
    {
        return 0U;
    }

    *data = gps_data;

    gps_data.new_data = 0U;

    return 1U;
}


uint8_t GPS_IsReady(void)
{
    return gps_ready;
}


