################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32l031xx.s 

OBJS += \
./startup/startup_stm32l031xx.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -I"/home/mark/repositories/climate_control/climate_control/inc" -I"/home/mark/repositories/climate_control/climate_control/CMSIS/core" -I"/home/mark/repositories/climate_control/climate_control/CMSIS/device" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

