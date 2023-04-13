################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/Common/System/system_stm32wlxx.c 

OBJS += \
./Drivers/CMSIS/system_stm32wlxx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32wlxx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32wlxx.o: C:/Users/bruno.USER-PC/Desktop/Clones/nucleo-wl55jc-sensor-node/LoRaWAN_End_Node_DualCoreFreeRTOS/Common/System/system_stm32wlxx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DCORE_CM0PLUS -DUSE_HAL_DRIVER -DSTM32WL55xx '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -DKMS_ENABLED -c -I../../../CM0PLUS/Core/Inc -I../../../CM0PLUS/KMS/App -I../../../CM0PLUS/LoRaWAN/App -I../../../CM0PLUS/LoRaWAN/Target -I../../../CM0PLUS/MbMux -I../../../Common/Board -I../../../Common/System -I../../../Common/MbMux -I../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../Utilities/trace/adv_trace -I../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../Utilities/misc -I../../../Utilities/sequencer -I../../../Utilities/timer -I../../../Utilities/lpm/tiny_lpm -I../../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../Middlewares/ST/STM32_Key_Management_Services/Modules -I../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../Middlewares/ST/STM32_Key_Management_Services/Interface/CryptoApi -I../../../Middlewares/ST/STM32_Key_Management_Services/Interface/CryptoApi/HAL -I../../../Middlewares/ST/STM32_Key_Management_Services/Interface/CryptoApi/MBED -I../../../Middlewares/ST/STM32_Key_Management_Services/Interface/CryptoApi/ST -I../../../Middlewares/ST/STM32_Key_Management_Services/niKMS -I../../../Middlewares/Third_Party/mbed-crypto/include -I../../../Middlewares/Third_Party/mbed-crypto/include/mbedtls -I../../../Middlewares/Third_Party/mbed-crypto/include/psa -I../../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../Middlewares/Third_Party/SubGHz_Phy -I../../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32wlxx.cyclo ./Drivers/CMSIS/system_stm32wlxx.d ./Drivers/CMSIS/system_stm32wlxx.o ./Drivers/CMSIS/system_stm32wlxx.su

.PHONY: clean-Drivers-2f-CMSIS

