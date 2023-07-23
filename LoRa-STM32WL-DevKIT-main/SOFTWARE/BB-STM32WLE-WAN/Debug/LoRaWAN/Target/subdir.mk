################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LoRaWAN/Target/lora_info.c \
../LoRaWAN/Target/radio_board_if.c \
../LoRaWAN/Target/stm32wlxx_olimex.c \
../LoRaWAN/Target/stm32wlxx_olimex_radio.c 

OBJS += \
./LoRaWAN/Target/lora_info.o \
./LoRaWAN/Target/radio_board_if.o \
./LoRaWAN/Target/stm32wlxx_olimex.o \
./LoRaWAN/Target/stm32wlxx_olimex_radio.o 

C_DEPS += \
./LoRaWAN/Target/lora_info.d \
./LoRaWAN/Target/radio_board_if.d \
./LoRaWAN/Target/stm32wlxx_olimex.d \
./LoRaWAN/Target/stm32wlxx_olimex_radio.d 


# Each subdirectory must supply rules for building sources it contributes
LoRaWAN/Target/%.o LoRaWAN/Target/%.su LoRaWAN/Target/%.cyclo: ../LoRaWAN/Target/%.c LoRaWAN/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Utilities/trace/adv_trace -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I../LoRaWAN/App -I../LoRaWAN/Target -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

clean: clean-LoRaWAN-2f-Target

clean-LoRaWAN-2f-Target:
	-$(RM) ./LoRaWAN/Target/lora_info.cyclo ./LoRaWAN/Target/lora_info.d ./LoRaWAN/Target/lora_info.o ./LoRaWAN/Target/lora_info.su ./LoRaWAN/Target/radio_board_if.cyclo ./LoRaWAN/Target/radio_board_if.d ./LoRaWAN/Target/radio_board_if.o ./LoRaWAN/Target/radio_board_if.su ./LoRaWAN/Target/stm32wlxx_olimex.cyclo ./LoRaWAN/Target/stm32wlxx_olimex.d ./LoRaWAN/Target/stm32wlxx_olimex.o ./LoRaWAN/Target/stm32wlxx_olimex.su ./LoRaWAN/Target/stm32wlxx_olimex_radio.cyclo ./LoRaWAN/Target/stm32wlxx_olimex_radio.d ./LoRaWAN/Target/stm32wlxx_olimex_radio.o ./LoRaWAN/Target/stm32wlxx_olimex_radio.su

.PHONY: clean-LoRaWAN-2f-Target

