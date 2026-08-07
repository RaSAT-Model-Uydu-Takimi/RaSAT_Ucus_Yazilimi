################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/M1.0_SensorReaderCore.c \
../Core/Src/M1.1_MPU9250.c \
../Core/Src/M1.2_BMP280.c \
../Core/Src/M2.0_FactoryCalibrator.c \
../Core/Src/M2.1_CalibrationUI.c \
../Core/Src/M3.0_FilterCore.c \
../Core/Src/M3.1_MatrixOps.c \
../Core/Src/M3.2_ConfidenceEvaluator.c \
../Core/Src/M3.3_EKFAttitude.c \
../Core/Src/M3.4_EKFYaw.c \
../Core/Src/M3.5_EKFAltitude.c \
../Core/Src/M3.6_EKFNavXY.c \
../Core/Src/M3.7_Kinematics.c \
../Core/Src/M3.8_StationReference.c \
../Core/Src/M4.0_FlightControl.c \
../Core/Src/M4.1_PID.c \
../Core/Src/M4.3_Attitude.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/M1.0_SensorReaderCore.o \
./Core/Src/M1.1_MPU9250.o \
./Core/Src/M1.2_BMP280.o \
./Core/Src/M2.0_FactoryCalibrator.o \
./Core/Src/M2.1_CalibrationUI.o \
./Core/Src/M3.0_FilterCore.o \
./Core/Src/M3.1_MatrixOps.o \
./Core/Src/M3.2_ConfidenceEvaluator.o \
./Core/Src/M3.3_EKFAttitude.o \
./Core/Src/M3.4_EKFYaw.o \
./Core/Src/M3.5_EKFAltitude.o \
./Core/Src/M3.6_EKFNavXY.o \
./Core/Src/M3.7_Kinematics.o \
./Core/Src/M3.8_StationReference.o \
./Core/Src/M4.0_FlightControl.o \
./Core/Src/M4.1_PID.o \
./Core/Src/M4.3_Attitude.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/M1.0_SensorReaderCore.d \
./Core/Src/M1.1_MPU9250.d \
./Core/Src/M1.2_BMP280.d \
./Core/Src/M2.0_FactoryCalibrator.d \
./Core/Src/M2.1_CalibrationUI.d \
./Core/Src/M3.0_FilterCore.d \
./Core/Src/M3.1_MatrixOps.d \
./Core/Src/M3.2_ConfidenceEvaluator.d \
./Core/Src/M3.3_EKFAttitude.d \
./Core/Src/M3.4_EKFYaw.d \
./Core/Src/M3.5_EKFAltitude.d \
./Core/Src/M3.6_EKFNavXY.d \
./Core/Src/M3.7_Kinematics.d \
./Core/Src/M3.8_StationReference.d \
./Core/Src/M4.0_FlightControl.d \
./Core/Src/M4.1_PID.d \
./Core/Src/M4.3_Attitude.d \
./Core/Src/main.d \
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
	-$(RM) ./Core/Src/M1.0_SensorReaderCore.cyclo ./Core/Src/M1.0_SensorReaderCore.d ./Core/Src/M1.0_SensorReaderCore.o ./Core/Src/M1.0_SensorReaderCore.su ./Core/Src/M1.1_MPU9250.cyclo ./Core/Src/M1.1_MPU9250.d ./Core/Src/M1.1_MPU9250.o ./Core/Src/M1.1_MPU9250.su ./Core/Src/M1.2_BMP280.cyclo ./Core/Src/M1.2_BMP280.d ./Core/Src/M1.2_BMP280.o ./Core/Src/M1.2_BMP280.su ./Core/Src/M2.0_FactoryCalibrator.cyclo ./Core/Src/M2.0_FactoryCalibrator.d ./Core/Src/M2.0_FactoryCalibrator.o ./Core/Src/M2.0_FactoryCalibrator.su ./Core/Src/M2.1_CalibrationUI.cyclo ./Core/Src/M2.1_CalibrationUI.d ./Core/Src/M2.1_CalibrationUI.o ./Core/Src/M2.1_CalibrationUI.su ./Core/Src/M3.0_FilterCore.cyclo ./Core/Src/M3.0_FilterCore.d ./Core/Src/M3.0_FilterCore.o ./Core/Src/M3.0_FilterCore.su ./Core/Src/M3.1_MatrixOps.cyclo ./Core/Src/M3.1_MatrixOps.d ./Core/Src/M3.1_MatrixOps.o ./Core/Src/M3.1_MatrixOps.su ./Core/Src/M3.2_ConfidenceEvaluator.cyclo ./Core/Src/M3.2_ConfidenceEvaluator.d ./Core/Src/M3.2_ConfidenceEvaluator.o ./Core/Src/M3.2_ConfidenceEvaluator.su ./Core/Src/M3.3_EKFAttitude.cyclo ./Core/Src/M3.3_EKFAttitude.d ./Core/Src/M3.3_EKFAttitude.o ./Core/Src/M3.3_EKFAttitude.su ./Core/Src/M3.4_EKFYaw.cyclo ./Core/Src/M3.4_EKFYaw.d ./Core/Src/M3.4_EKFYaw.o ./Core/Src/M3.4_EKFYaw.su ./Core/Src/M3.5_EKFAltitude.cyclo ./Core/Src/M3.5_EKFAltitude.d ./Core/Src/M3.5_EKFAltitude.o ./Core/Src/M3.5_EKFAltitude.su ./Core/Src/M3.6_EKFNavXY.cyclo ./Core/Src/M3.6_EKFNavXY.d ./Core/Src/M3.6_EKFNavXY.o ./Core/Src/M3.6_EKFNavXY.su ./Core/Src/M3.7_Kinematics.cyclo ./Core/Src/M3.7_Kinematics.d ./Core/Src/M3.7_Kinematics.o ./Core/Src/M3.7_Kinematics.su ./Core/Src/M3.8_StationReference.cyclo ./Core/Src/M3.8_StationReference.d ./Core/Src/M3.8_StationReference.o ./Core/Src/M3.8_StationReference.su ./Core/Src/M4.0_FlightControl.cyclo ./Core/Src/M4.0_FlightControl.d ./Core/Src/M4.0_FlightControl.o ./Core/Src/M4.0_FlightControl.su ./Core/Src/M4.1_PID.cyclo ./Core/Src/M4.1_PID.d ./Core/Src/M4.1_PID.o ./Core/Src/M4.1_PID.su ./Core/Src/M4.3_Attitude.cyclo ./Core/Src/M4.3_Attitude.d ./Core/Src/M4.3_Attitude.o ./Core/Src/M4.3_Attitude.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

