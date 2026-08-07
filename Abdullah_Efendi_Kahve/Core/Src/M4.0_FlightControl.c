/*
 * flight_control.c
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */


#include "M4.0_FlightControl.h"
#include "M0.2_SystemConfig.h"
#include "M4.2_Motors.h"
#include "M4.1_PID.h"
#include "main.h"

#include <stddef.h>


#define SOFT_LANDING_VEL_MPS    (-0.8f)


static FlightControlState current_state = Falling_start_act;
static FlightControlStatus status;

static PID_Controller altitude_pid;
static PID_Controller velocity_pid;

static uint32_t state_entry_time_ms = 0U;
static uint8_t mission_started = 0U;

static uint8_t apam_timer_active = 0U;
static uint32_t apam_start_time_ms = 0U;


/*--------------------------------------------------
 * Yardımcı fonksiyonlar
 *--------------------------------------------------*/

static float clamp_float(float value, float min_value, float max_value)
{
    if(value > max_value)
    {
        return max_value;
    }

    if(value < min_value)
    {
        return min_value;
    }

    return value;
}


static float get_hover_throttle(void)
{
    float throttle;

    throttle = (VEHICLE_MASS_KG * G) / MAX_TOTAL_THRUST_N;

    return clamp_float(throttle, PWM_MIN, PWM_MAX);
}


static void reset_pid(PID_Controller *pid)
{
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->filtered_val = 0.0f;
}


static void reset_all_pids(void)
{
    reset_pid(&altitude_pid);
    reset_pid(&velocity_pid);
}


static void configure_pids(void)
{
    /*
     * Dış PID:
     * İrtifa hatasını hedef dikey hıza çevirir.
     */
    altitude_pid.Kp = 0.08f;
    altitude_pid.Ki = 0.00f;
    altitude_pid.Kd = 0.02f;

    altitude_pid.out_min = OUTER_VEL_MIN;
    altitude_pid.out_max = OUTER_VEL_MAX;
    altitude_pid.alpha = 0.25f;


    /*
     * İç PID:
     * Hedef dikey hız ile gerçek dikey hız arasındaki hatadan
     * throttle düzeltmesi üretir.
     */
    velocity_pid.Kp = 0.06f;
    velocity_pid.Ki = 0.02f;
    velocity_pid.Kd = 0.01f;

    velocity_pid.out_min = INNER_TRIM_MIN;
    velocity_pid.out_max = INNER_TRIM_MAX;
    velocity_pid.alpha = 0.25f;

    reset_all_pids();
}


static void clear_commands(void)
{
    status.separation_cmd = 0U;
    status.apam_cmd = 0U;
}


static void refresh_status(const FlightControlInput *input, uint32_t now_ms)
{
    status.state = current_state;
    status.act_code = (uint8_t)current_state;
    status.state_time_ms = now_ms - state_entry_time_ms;

    if(input != NULL)
    {
        status.altitude_m = input->altitude_m;
        status.vertical_velocity_mps = input->vertical_velocity_mps;
    }

    status.throttle_cmd = Motors_GetThrottle();
    status.pwm_us = Motors_GetPWM();
}


static void enter_state(FlightControlState new_state)
{
    current_state = new_state;
    state_entry_time_ms = HAL_GetTick();

    clear_commands();
    reset_all_pids();

    status.state = current_state;
    status.act_code = (uint8_t)current_state;
    status.state_time_ms = 0U;

    switch(current_state)
    {
        /*
         * Bu statelerde motor kapalı.
         */
        case Falling_start_act:
        case Separation_start_act:
        case Falling_end_act:
        case APAM_act:
            Motors_Disarm();
            break;

        /*
         * Bu statelerde motor kontrolü aktif.
         */
        case Separation_end_act:
        case Hover_start_act:
        case Hover_end_act:
        case Landing_prep_act:
            Motors_Arm();
            break;

        default:
            Motors_Disarm();
            break;
    }
}


static uint8_t apam_required(const FlightControlInput *input, uint32_t now_ms)
{
    uint8_t speed_emergency;

    /*
     * Dışarıdan doğrudan APAM isteği geldiyse beklemeden APAM.
     */
    if(input->apam_request != 0U)
    {
        return 1U;
    }

    /*
     * Çok yüksek düşüş hızı acil durum sayılır.
     * Ancak tek ölçümle karar vermemek için süreyle doğrulanır.
     */
    speed_emergency =
        (input->altitude_m > EMERGENCY_ALT_MIN_M) &&
        (input->vertical_velocity_mps <= -EMERGENCY_SPEED_MPS);

    if(speed_emergency != 0U)
    {
        if(apam_timer_active == 0U)
        {
            apam_timer_active = 1U;
            apam_start_time_ms = now_ms;
            return 0U;
        }

        if((now_ms - apam_start_time_ms) >= EMERGENCY_CONFIRM_MS)
        {
            return 1U;
        }
    }
    else
    {
        apam_timer_active = 0U;
        apam_start_time_ms = 0U;
    }

    return 0U;
}


static void run_velocity_control(const FlightControlInput *input,
                                 float target_velocity_mps)
{
    float throttle_trim;
    float throttle_cmd;

    /*
     * İç PID:
     * hedef dikey hız - ölçülen dikey hız -> throttle düzeltmesi
     */
    throttle_trim = compute_pid(&velocity_pid,
                                target_velocity_mps,
                                input->vertical_velocity_mps,
                                CONTROL_DT);

    /*
     * Temel hover throttle üzerine PID düzeltmesi eklenir.
     */
    throttle_cmd = get_hover_throttle() + throttle_trim;
    throttle_cmd = clamp_float(throttle_cmd, PWM_MIN, PWM_MAX);

    Motors_SetThrottle(throttle_cmd);

    status.target_velocity_mps = target_velocity_mps;
    status.throttle_cmd = Motors_GetThrottle();
    status.pwm_us = Motors_GetPWM();
}


static void run_hover_control(const FlightControlInput *input)
{
    float target_velocity_mps;

    /*
     * Dış PID:
     * 200 m hedef irtifa - ölçülen irtifa -> hedef dikey hız
     */
    target_velocity_mps = compute_pid(&altitude_pid,
                                      HOVER_ALT_M,
                                      input->altitude_m,
                                      CONTROL_DT);

    status.target_altitude_m = HOVER_ALT_M;

    /*
     * İç PID:
     * hedef dikey hız -> throttle
     */
    run_velocity_control(input, target_velocity_mps);
}


/*--------------------------------------------------
 * Public fonksiyonlar
 *--------------------------------------------------*/

void FlightControl_Init(void)
{
    status.state = Falling_start_act;
    status.act_code = (uint8_t)Falling_start_act;

    status.altitude_m = 0.0f;
    status.vertical_velocity_mps = 0.0f;

    status.target_altitude_m = 0.0f;
    status.target_velocity_mps = 0.0f;

    status.throttle_cmd = 0.0f;
    status.pwm_us = ESC_CUTOFF_US;

    status.state_time_ms = 0U;

    status.separation_cmd = 0U;
    status.apam_cmd = 0U;

    mission_started = 0U;

    apam_timer_active = 0U;
    apam_start_time_ms = 0U;

    configure_pids();
    Motors_Init();

    enter_state(Falling_start_act);
}


void FlightControl_Start(void)
{
    /*
     * Bu fonksiyon roketten ayrılma / pasif iniş başlangıcı
     * algılandığında çağrılmalıdır.
     */
    mission_started = 1U;

    apam_timer_active = 0U;
    apam_start_time_ms = 0U;

    enter_state(Falling_start_act);
}


void FlightControl_Stop(void)
{
    mission_started = 0U;

    enter_state(Falling_end_act);
}


void FlightControl_Update(const FlightControlInput *input)
{
    uint32_t now_ms;

    now_ms = HAL_GetTick();

    /*
     * Görev başlamadıysa motorları kapalı tut.
     */
    if(mission_started == 0U)
    {
        Motors_StopAll();
        refresh_status(input, now_ms);
        return;
    }

    /*
     * Sensör verisi yoksa veya güvenilir değilse APAM.
     */
    if((input == NULL) || (input->data_valid == 0U))
    {
        enter_state(APAM_act);
        status.apam_cmd = 1U;
        refresh_status(input, HAL_GetTick());
        return;
    }

    clear_commands();

    /*
     * Acil durum kontrolü.
     */
    if((current_state != APAM_act) &&
       (current_state != Falling_end_act) &&
       (apam_required(input, now_ms) != 0U))
    {
        enter_state(APAM_act);
    }

    switch(current_state)
    {
        case Falling_start_act:
            /*
             * Roketten ayrıldıktan sonra 1800 m'den 1000 m'ye kadar
             * pasif iniş. Motorlar kapalı.
             */
            status.target_altitude_m = SEPARATION_ALT_M;
            status.target_velocity_mps = 0.0f;

            Motors_StopAll();

            if(input->altitude_m <= SEPARATION_ALT_M)
            {
                enter_state(Separation_start_act);
            }
            break;


        case Separation_start_act:
            /*
             * 1000 m'de görev yükü ayrılma mekanizması tetiklenir.
             * payload_separated gelene kadar bu state'te kalır.
             */
            status.target_altitude_m = SEPARATION_ALT_M;
            status.target_velocity_mps = 0.0f;

            status.separation_cmd = 1U;

            Motors_StopAll();

            if(input->payload_separated != 0U)
            {
                enter_state(Separation_end_act);
            }
            break;


        case Separation_end_act:
            /*
             * Ayrılma tamamlandı.
             * Motorlar aktif edilir ve 200 m'ye kadar sabit hızlı iniş yapılır.
             */
            status.target_altitude_m = HOVER_ALT_M;

            if(input->altitude_m <= HOVER_ALT_M)
            {
                enter_state(Hover_start_act);
            }
            else
            {
                run_velocity_control(input, ACTIVE_DESCENT_VEL_MPS);
            }
            break;


        case Hover_start_act:
            /*
             * 200 m'de 10 saniye hover.
             */
            if((now_ms - state_entry_time_ms) >= HOVER_TIME_MS)
            {
                enter_state(Hover_end_act);
            }
            else
            {
                run_hover_control(input);
            }
            break;


        case Hover_end_act:
            /*
             * Hover bitti.
             * 50 m'ye kadar tekrar belirlenen aktif iniş hızına dönülür.
             */
            status.target_altitude_m = LANDING_SLOW_ALT_M;

            if(input->altitude_m <= LANDING_SLOW_ALT_M)
            {
                enter_state(Landing_prep_act);
            }
            else
            {
                run_velocity_control(input, ACTIVE_DESCENT_VEL_MPS);
            }
            break;


        case Landing_prep_act:
            /*
             * Son yaklaşma.
             * 50 m altında yumuşak iniş hızı kullanılır.
             */
            status.target_altitude_m = GROUND_ALT_M;

            if((input->landed_detected != 0U) ||
               (input->altitude_m <= GROUND_ALT_M))
            {
                enter_state(Falling_end_act);
            }
            else
            {
                run_velocity_control(input, SOFT_LANDING_VEL_MPS);
            }
            break;


        case Falling_end_act:
            /*
             * Yere temas / görev sonu.
             */
            status.target_altitude_m = GROUND_ALT_M;
            status.target_velocity_mps = 0.0f;

            Motors_StopAll();
            break;


        case APAM_act:
        default:
            /*
             * Acil durum paraşüt mekanizması.
             * Motorlar güvenli moda alınır, APAM komutu verilir.
             */
            status.target_altitude_m = 0.0f;
            status.target_velocity_mps = 0.0f;

            status.apam_cmd = 1U;

            Motors_StopAll();
            break;
    }

    refresh_status(input, HAL_GetTick());
}


FlightControlState FlightControl_GetState(void)
{
    return current_state;
}


FlightControlStatus FlightControl_GetStatus(void)
{
    return status;
}


const char *FlightControl_GetStateName(FlightControlState state)
{
    switch(state)
    {
        case Falling_start_act:
            return "Falling_start_act";

        case Separation_start_act:
            return "Separation_start_act";

        case Separation_end_act:
            return "Separation_end_act";

        case Hover_start_act:
            return "Hover_start_act";

        case Hover_end_act:
            return "Hover_end_act";

        case Landing_prep_act:
            return "Landing_prep_act";

        case Falling_end_act:
            return "Falling_end_act";

        case APAM_act:
            return "APAM_act";

        default:
            return "UNKNOWN";
    }
}
