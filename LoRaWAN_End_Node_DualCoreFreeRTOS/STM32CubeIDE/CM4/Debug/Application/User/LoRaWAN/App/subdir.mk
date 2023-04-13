################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/CayenneLpp.c \
C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/app_lorawan.c \
C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/lora_app.c 

OBJS += \
./Application/User/LoRaWAN/App/CayenneLpp.o \
./Application/User/LoRaWAN/App/app_lorawan.o \
./Application/User/LoRaWAN/App/lora_app.o 

C_DEPS += \
./Application/User/LoRaWAN/App/CayenneLpp.d \
./Application/User/LoRaWAN/App/app_lorawan.d \
./Application/User/LoRaWAN/App/lora_app.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/LoRaWAN/App/CayenneLpp.o: C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/CayenneLpp.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../../CM4/Core/Inc -I../../../CM4/LoRaWAN/App -I../../../CM4/LoRaWAN/Target -I../../../CM4/MbMux -I../../../Common/Board -I../../../Common/System -I../../../Common/MbMux -I../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../Utilities/trace/adv_trace -I../../../Utilities/misc -I../../../Utilities/timer -I../../../Utilities/lpm/tiny_lpm -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../Middlewares/Third_Party/SubGHz_Phy -I../../../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/app_lorawan.o: C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/app_lorawan.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../../CM4/Core/Inc -I../../../CM4/LoRaWAN/App -I../../../CM4/LoRaWAN/Target -I../../../CM4/MbMux -I../../../Common/Board -I../../../Common/System -I../../../Common/MbMux -I../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../Utilities/trace/adv_trace -I../../../Utilities/misc -I../../../Utilities/timer -I../../../Utilities/lpm/tiny_lpm -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../Middlewares/Third_Party/SubGHz_Phy -I../../../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/User/LoRaWAN/App/lora_app.o: C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/CM4/LoRaWAN/App/lora_app.c Application/User/LoRaWAN/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WL55xx -c -I../../../CM4/Core/Inc -I../../../CM4/LoRaWAN/App -I../../../CM4/LoRaWAN/Target -I../../../CM4/MbMux -I../../../Common/Board -I../../../Common/System -I../../../Common/MbMux -I../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../Utilities/trace/adv_trace -I../../../Utilities/misc -I../../../Utilities/timer -I../../../Utilities/lpm/tiny_lpm -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../Middlewares/Third_Party/SubGHz_Phy -I../../../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Application-2f-User-2f-LoRaWAN-2f-App

clean-Application-2f-User-2f-LoRaWAN-2f-App:
	-$(RM) ./Application/User/LoRaWAN/App/CayenneLpp.cyclo ./Application/User/LoRaWAN/App/CayenneLpp.d ./Application/User/LoRaWAN/App/CayenneLpp.o ./Application/User/LoRaWAN/App/CayenneLpp.su ./Application/User/LoRaWAN/App/app_lorawan.cyclo ./Application/User/LoRaWAN/App/app_lorawan.d ./Application/User/LoRaWAN/App/app_lorawan.o ./Application/User/LoRaWAN/App/app_lorawan.su ./Application/User/LoRaWAN/App/lora_app.cyclo ./Application/User/LoRaWAN/App/lora_app.d ./Application/User/LoRaWAN/App/lora_app.o ./Application/User/LoRaWAN/App/lora_app.su

.PHONY: clean-Application-2f-User-2f-LoRaWAN-2f-App

