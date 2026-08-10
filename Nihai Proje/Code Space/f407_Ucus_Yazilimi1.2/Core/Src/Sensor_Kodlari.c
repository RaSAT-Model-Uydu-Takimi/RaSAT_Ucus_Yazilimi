/*
 * Sensor_Kodlari.c
 *
 *  Created on: Aug 10, 2026
 *      Author: abduh
 */

#include "Sensor_Kodlari.h"

MPU9250_Data imu_data; // Sensör verilerini tutacağımız yapı
BMP280_Data myBmpData;
GPS_Data gps;
volatile uint8_t MPU9250_veri_hazir_bayragi = 0;

I2C_HandleTypeDef *I2C_HATTI;
UART_HandleTypeDef *GPS_UART_HATTI;
uint16_t MPU9250_INT_PIN;

uint8_t Sensors_Init(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *gps_uart_handle, uint16_t mpu9250_int_pin){

	I2C_HATTI = i2c_handle;
	GPS_UART_HATTI = gps_uart_handle;
	MPU9250_INT_PIN = mpu9250_int_pin;

	if (MPU9250_Init(I2C_HATTI) != 1) {
	  		return 0;
	  	}

	  if (BMP280_Init(I2C_HATTI) != 1) {
		  return 0;
	    }
	  if(GPS_Init(GPS_UART_HATTI) != 0U)
	  {
		  if(GPS_StartReceiveIT() !=0U){

		  }
		  else{return 0;}
	  }
	  else{return 0;}

	  return 1;
}




void SensorleriOku(DataCenter *DC, uint32_t SystemMicros){

	if(MPU9250_veri_hazir_bayragi==1){
		  if (MPU9250_Read(&imu_data) == 1) {

			  DC->acc.raw_x = imu_data.accel_x_g;
			  DC->acc.raw_y = imu_data.accel_y_g;
			  DC->acc.raw_z = imu_data.accel_z_g;
			  DC->acc.UpdateTime = SystemMicros;

			  DC->gyro.raw_x = imu_data.gyro_x_dps;
			  DC->gyro.raw_y = imu_data.gyro_y_dps;
			  DC->gyro.raw_z = imu_data.gyro_z_dps;
			  DC->gyro.UpdateTime = SystemMicros;

			  if (imu_data.mag_valid == 1) {
				  DC->mag.raw_x = imu_data.mag_x_uT;
				  DC->mag.raw_y = imu_data.mag_y_uT;
				  DC->mag.raw_z = imu_data.mag_z_uT;
				  DC->mag.UpdateTime = SystemMicros;
			  }

		  }
		  MPU9250_veri_hazir_bayragi=0;
	  }

	static uint32_t sonBmpOkumaZamani = 0;
	if (SystemMicros - sonBmpOkumaZamani >= 2000) { // 2ms'de bir oku
	    sonBmpOkumaZamani = SystemMicros;
	    if (BMP280_Read(&myBmpData) == 1) {
	        DC->baro.raw_press = myBmpData.pressure_pa;
	        DC->baro.raw_temp = myBmpData.temperature_c;
	        DC->baro.UpdateTime = SystemMicros;
	    }
	}


	  if(GPS_Read(&gps) != 0U)
	  {
		  //asilgps.new_data = gps.new_data;
		  //DC->gps. = gps.fix_valid;
		  DC->gps.fixQuality = gps.fix_quality;
		  DC->gps.satelliteCount = gps.satellites;

		  DC->gps.raw_lat = gps.latitude_deg;
		  DC->gps.raw_lon = gps.longitude_deg;

		  DC->gps.raw_alt = gps.altitude_m;
		  DC->gps.speed = gps.speed_mps;
		  //DC->gps. = gps.course_deg;
		  DC->gps.HDOP = gps.hdop;
		  DC->gps.UpdateTime = SystemMicros;
	  }
	  else
	  {
	  }
}



void MPU9250_Int_Fonksiyonu(uint16_t GPIO_Pin){
	if (GPIO_Pin == MPU9250_INT_PIN)
	{
		MPU9250_veri_hazir_bayragi = 1;
	}
}

void GPS_Uart_Int_Fonksiyonu(UART_HandleTypeDef *huart){
	if (huart == GPS_UART_HATTI)
	{
		GPS_UART_RxCpltCallback(huart);
	}
}


