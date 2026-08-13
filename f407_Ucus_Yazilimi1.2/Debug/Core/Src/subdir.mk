################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Eylem_Bloklari.c \
../Core/Src/M3.0_FilterSystem.c \
../Core/Src/M3.1_Calibration.c \
../Core/Src/M3.2_Attitude.c \
../Core/Src/M3.3_Yaw.c \
../Core/Src/Sensor_Kodlari.c \
../Core/Src/TM.c \
../Core/Src/bmp280.c \
../Core/Src/dshot.c \
../Core/Src/gps.c \
../Core/Src/main.c \
../Core/Src/mpu9250.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/Eylem_Bloklari.o \
./Core/Src/M3.0_FilterSystem.o \
./Core/Src/M3.1_Calibration.o \
./Core/Src/M3.2_Attitude.o \
./Core/Src/M3.3_Yaw.o \
./Core/Src/Sensor_Kodlari.o \
./Core/Src/TM.o \
./Core/Src/bmp280.o \
./Core/Src/dshot.o \
./Core/Src/gps.o \
./Core/Src/main.o \
./Core/Src/mpu9250.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/Eylem_Bloklari.d \
./Core/Src/M3.0_FilterSystem.d \
./Core/Src/M3.1_Calibration.d \
./Core/Src/M3.2_Attitude.d \
./Core/Src/M3.3_Yaw.d \
./Core/Src/Sensor_Kodlari.d \
./Core/Src/TM.d \
./Core/Src/bmp280.d \
./Core/Src/dshot.d \
./Core/Src/gps.d \
./Core/Src/main.d \
./Core/Src/mpu9250.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Eylem_Bloklari.cyclo ./Core/Src/Eylem_Bloklari.d ./Core/Src/Eylem_Bloklari.o ./Core/Src/Eylem_Bloklari.su ./Core/Src/M3.0_FilterSystem.cyclo ./Core/Src/M3.0_FilterSystem.d ./Core/Src/M3.0_FilterSystem.o ./Core/Src/M3.0_FilterSystem.su ./Core/Src/M3.1_Calibration.cyclo ./Core/Src/M3.1_Calibration.d ./Core/Src/M3.1_Calibration.o ./Core/Src/M3.1_Calibration.su ./Core/Src/M3.2_Attitude.cyclo ./Core/Src/M3.2_Attitude.d ./Core/Src/M3.2_Attitude.o ./Core/Src/M3.2_Attitude.su ./Core/Src/M3.3_Yaw.cyclo ./Core/Src/M3.3_Yaw.d ./Core/Src/M3.3_Yaw.o ./Core/Src/M3.3_Yaw.su ./Core/Src/Sensor_Kodlari.cyclo ./Core/Src/Sensor_Kodlari.d ./Core/Src/Sensor_Kodlari.o ./Core/Src/Sensor_Kodlari.su ./Core/Src/TM.cyclo ./Core/Src/TM.d ./Core/Src/TM.o ./Core/Src/TM.su ./Core/Src/bmp280.cyclo ./Core/Src/bmp280.d ./Core/Src/bmp280.o ./Core/Src/bmp280.su ./Core/Src/dshot.cyclo ./Core/Src/dshot.d ./Core/Src/dshot.o ./Core/Src/dshot.su ./Core/Src/gps.cyclo ./Core/Src/gps.d ./Core/Src/gps.o ./Core/Src/gps.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu9250.cyclo ./Core/Src/mpu9250.d ./Core/Src/mpu9250.o ./Core/Src/mpu9250.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

