/*
 * flight_control.h
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#ifndef INC_FLIGHT_CONTROL_H_
#define INC_FLIGHT_CONTROL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Yarışma görev durumları.
 * Bu değerler telemetriye gönderilecek act kodlarıyla aynıdır.
 */
typedef enum
{
    Falling_start_act     = 1,  /* Roketten ayrılma / pasif iniş başlangıcı */
    Separation_start_act  = 2,  /* Görev yükü ayrılma başlangıcı */
    Separation_end_act    = 3,  /* Ayrılma tamamlandı / aktif iniş */
    Hover_start_act       = 4,  /* 200 m hover başlangıcı */
    Hover_end_act         = 5,  /* Hover bitti / tekrar aktif iniş */
    Landing_prep_act      = 6,  /* Son yaklaşma / yumuşak iniş */
    Falling_end_act       = 7,  /* Yere temas / görev sonu */
    APAM_act              = 8   /* Acil durum paraşüt mekanizması */
} FlightControlState;


/*
 * Bu yapı daha sonra sensor_processing.c tarafından doldurulacak.
 * Flight control ham sensör okumaz; hazır işlenmiş veriyi kullanır.
 */
typedef struct
{
    float altitude_m;              /* Anlık irtifa [m] */
    float vertical_velocity_mps;   /* Dikey hız [m/s], inişte negatif */

    uint8_t data_valid;            /* Sensör verisi güvenilir mi? */
    uint8_t payload_separated;     /* Görev yükü ayrılması tamamlandı mı? */
    uint8_t landed_detected;       /* Yere temas algılandı mı? */
    uint8_t apam_request;          /* Dışarıdan acil durum/APAM isteği */

} FlightControlInput;


/*
 * Telemetri, debug ve diğer modüller için uçuş kontrol çıktıları.
 */
typedef struct
{
    FlightControlState state;
    uint8_t act_code;

    float altitude_m;
    float vertical_velocity_mps;

    float target_altitude_m;
    float target_velocity_mps;

    float throttle_cmd;
    uint16_t pwm_us;

    uint32_t state_time_ms;

    uint8_t separation_cmd;        /* Ayrılma mekanizmasını tetikle */
    uint8_t apam_cmd;              /* APAM mekanizmasını tetikle */

} FlightControlStatus;


void FlightControl_Init(void);
void FlightControl_Start(void);
void FlightControl_Stop(void);

void FlightControl_Update(const FlightControlInput *input);

FlightControlState FlightControl_GetState(void);
FlightControlStatus FlightControl_GetStatus(void);

const char *FlightControl_GetStateName(FlightControlState state);


#endif /* INC_FLIGHT_CONTROL_H_ */
