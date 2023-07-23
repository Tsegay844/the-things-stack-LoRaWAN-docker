################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/iis2mdc/iis2mdc_reg.c 

OBJS += \
./Core/Src/iis2mdc/iis2mdc_reg.o 

C_DEPS += \
./Core/Src/iis2mdc/iis2mdc_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/iis2mdc/%.o Core/Src/iis2mdc/%.su Core/Src/iis2mdc/%.cyclo: ../Core/Src/iis2mdc/%.c Core/Src/iis2mdc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Utilities/trace/adv_trace -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I../LoRaWAN/App -I../LoRaWAN/Target -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-iis2mdc

clean-Core-2f-Src-2f-iis2mdc:
	-$(RM) ./Core/Src/iis2mdc/iis2mdc_reg.cyclo ./Core/Src/iis2mdc/iis2mdc_reg.d ./Core/Src/iis2mdc/iis2mdc_reg.o ./Core/Src/iis2mdc/iis2mdc_reg.su

.PHONY: clean-Core-2f-Src-2f-iis2mdc

