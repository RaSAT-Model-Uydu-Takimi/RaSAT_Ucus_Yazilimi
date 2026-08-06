/*
 * config.h
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_


/*==================================================
 * CONTROL LOOP
 *==================================================*/

#define CONTROL_DT                 0.005f
#define CONTROL_FREQ_HZ            200.0f

/*==================================================
 * PHYSICS
 *==================================================*/

#define G               9.81f

#define VEHICLE_MASS_KG            1.69908f
#define HOVER_REFERENCE_MASS_KG    1.250f
#define MAX_TOTAL_THRUST_N    42.0f

/*==================================================
 * ESC
 *==================================================*/

#define ESC_MIN_US                 1000U
#define ESC_MAX_US                 2000U
#define ESC_ARM_US                 1050U
#define ESC_CUTOFF_US              1000U

/*==================================================
 * MISSION
 *==================================================*/

#define APOGEE_ALT_M               1800.0f
#define SEPARATION_ALT_M           1000.0f
#define HOVER_ALT_M                 200.0f
#define LANDING_SLOW_ALT_M           50.0f
#define GROUND_ALT_M                  0.5f

/*==================================================
 * CONTROL LIMITS
 *==================================================*/

#define ACTIVE_DESCENT_VEL_MPS      (-8.5f)

#define HOVER_TIME_MS             10000U
#define EMERGENCY_CONFIRM_MS      10000U

#define EMERGENCY_SPEED_MPS         16.0f
#define EMERGENCY_ALT_MIN_M        100.0f

#define PWM_MIN                     0.0f
#define PWM_MAX                     1.0f

#define OUTER_VEL_MIN             -10.0f
#define OUTER_VEL_MAX              10.0f

#define INNER_TRIM_MIN            -0.35f
#define INNER_TRIM_MAX             0.35f



#endif /* INC_CONFIG_H_ */
