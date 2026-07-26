# ifndef INC_SENSOR_CALIBRATOR_H_
# define INC_SENSOR_CALIBRATOR_H_

#include "Data.h"
#include "Filter_Config.h"

#ifdef __cplusplus
extern "C" {


/*  
Tanım: Kalibrasyon sabitlerinin kurulması
İşlev: Her bir sensör kanalı için BİAS , SCALE değerlerini  belleğe alır
*/
typedef struct {
    float acc_x_bias , acc_y_bias , acc_z_bias;
    float acc_x_scale , acc_y_scale , acc_z_scale;
    float acc_thermal_drift;

    float gyro_x_bias , gyro_y_bias , gyro_z_bias;
    float gyro_x_scale , gyro_y_scale , gyro_z_scale;

    float mag_x_bias , mag_y_bias , mag_z_bias;
    float mag_x_scale , mag_y_scale , mag_z_scale;

    float baro_press_bias , baro_press_scale;
    float baro_temp_bias , baro_temp_scale; 
    
    float sea_level_pa ;
    float temp_ref_c ;

    float ref_ground_altitude_m ; // Referans zemin yüksekliği (FILTER_CALIB_SAMPLES_COUNT) kadar örnek alındıktan sonra hesaplanacak
}SensorCalib_t;

/*  
*Tanım: 
*İşlev: 
*/

/*  
*Tanım: Kurulum yapan fonksiyon 
*İşlev: Struct içini Filter_Config.h sabitleriyle doldurur.
*/
void SensorCalib_Init(SensorCalib_t *calib);



/*  
*Tanım: Kalibrasyon Uygulayıcı fonksiyon 
*İşlev:
1 DataCenter : rawValue değerlerini Sensor_Calib_t : bias ve scale değerlerini okur
2 Data Center: calibValue değerlerini doldurur
*/
void SensorCalibApply (const SensorCalib_t *calib , DataCenter *dataC );


#ifdef __cplusplus
}
#endif

#endif /* INC_SENSOR_CALIBRATOR_H_ */