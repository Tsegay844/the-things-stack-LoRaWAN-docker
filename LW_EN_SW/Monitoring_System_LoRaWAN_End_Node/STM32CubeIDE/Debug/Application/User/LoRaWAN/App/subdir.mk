################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/CayenneLpp.c \
/home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/app_lorawan.c \
../Application/User/LoRaWAN/App/bmp280.c \
/home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/lora_app.c \
/home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/lora_info.c \
../Application/User/LoRaWAN/App/sht30.c 

OBJS += \
./Application/User/LoRaWAN/App/CayenneLpp.o \
./Application/User/LoRaWAN/App/app_lorawan.o \
./Application/User/LoRaWAN/App/bmp280.o \
./Application/User/LoRaWAN/App/lora_app.o \
./Application/User/LoRaWAN/App/lora_info.o \
./Application/User/LoRaWAN/App/sht30.o 

C_DEPS += \
./Application/User/LoRaWAN/App/CayenneLpp.d \
./Application/User/LoRaWAN/App/app_lorawan.d \
./Application/User/LoRaWAN/App/bmp280.d \
./Application/User/LoRaWAN/App/lora_app.d \
./Application/User/LoRaWAN/App/lora_info.d \
./Application/User/LoRaWAN/App/sht30.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/LoRaWAN/App/CayenneLpp.o: /home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/CayenneLpp.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Utilities/trace/adv_trace -I../../Utilities/misc -I../../Utilities/timer -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../Middlewares/Third_Party/LoRaWAN/Mac -I../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../Middlewares/Third_Party/SubGHz_Phy -I../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/STM32WLxx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/app_lorawan.o: /home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/app_lorawan.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Utilities/trace/adv_trace -I../../Utilities/misc -I../../Utilities/timer -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../Middlewares/Third_Party/LoRaWAN/Mac -I../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../Middlewares/Third_Party/SubGHz_Phy -I../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/STM32WLxx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/%.o Application/User/LoRaWAN/App/%.su Application/User/LoRaWAN/App/%.cyclo: ../Application/User/LoRaWAN/App/%.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Utilities/trace/adv_trace -I../../Utilities/misc -I../../Utilities/timer -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../Middlewares/Third_Party/LoRaWAN/Mac -I../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../Middlewares/Third_Party/SubGHz_Phy -I../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/STM32WLxx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/lora_app.o: /home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/lora_app.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Utilities/trace/adv_trace -I../../Utilities/misc -I../../Utilities/timer -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../Middlewares/Third_Party/LoRaWAN/Mac -I../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../Middlewares/Third_Party/SubGHz_Phy -I../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/STM32WLxx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/lora_info.o: /home/brunocasu/Clones/nucleo-wl55jc-sensor-node/Monitoring_System_LoRaWAN_End_Node/LoRaWAN/App/lora_info.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../Utilities/trace/adv_trace -I../../Utilities/misc -I../../Utilities/timer -I../../Utilities/lpm/tiny_lpm -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../Middlewares/Third_Party/LoRaWAN/Mac -I../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../Middlewares/Third_Party/SubGHz_Phy -I../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/STM32WLxx_Nucleo -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Application-2f-User-2f-LoRaWAN-2f-App

clean-Application-2f-User-2f-LoRaWAN-2f-App:
	-$(RM) ./Application/User/LoRaWAN/App/CayenneLpp.cyclo ./Application/User/LoRaWAN/App/CayenneLpp.d ./Application/User/LoRaWAN/App/CayenneLpp.o ./Application/User/LoRaWAN/App/CayenneLpp.su ./Application/User/LoRaWAN/App/app_lorawan.cyclo ./Application/User/LoRaWAN/App/app_lorawan.d ./Application/User/LoRaWAN/App/app_lorawan.o ./Application/User/LoRaWAN/App/app_lorawan.su ./Application/User/LoRaWAN/App/bmp280.cyclo ./Application/User/LoRaWAN/App/bmp280.d ./Application/User/LoRaWAN/App/bmp280.o ./Application/User/LoRaWAN/App/bmp280.su ./Application/User/LoRaWAN/App/lora_app.cyclo ./Application/User/LoRaWAN/App/lora_app.d ./Application/User/LoRaWAN/App/lora_app.o ./Application/User/LoRaWAN/App/lora_app.su ./Application/User/LoRaWAN/App/lora_info.cyclo ./Application/User/LoRaWAN/App/lora_info.d ./Application/User/LoRaWAN/App/lora_info.o ./Application/User/LoRaWAN/App/lora_info.su ./Application/User/LoRaWAN/App/sht30.cyclo ./Application/User/LoRaWAN/App/sht30.d ./Application/User/LoRaWAN/App/sht30.o ./Application/User/LoRaWAN/App/sht30.su

.PHONY: clean-Application-2f-User-2f-LoRaWAN-2f-App

