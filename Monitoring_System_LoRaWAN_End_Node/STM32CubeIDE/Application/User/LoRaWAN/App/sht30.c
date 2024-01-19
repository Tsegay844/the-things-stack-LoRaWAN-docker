/*
 * sht30.c
 *
 *  Created on: Jan 16, 2024
 *      Author: Bruno Casu
 */

#include "lora_app.h"
#include "platform.h"
#include "i2c.h"
#include "sys_app.h"


#define SHT3X_I2C_TIMEOUT 300
#define SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH 0x45

/**
 * Registers addresses.
 */
typedef enum
{
	SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH = 0x2c06,
	SHT3X_COMMAND_CLEAR_STATUS = 0x3041,
	SHT3X_COMMAND_SOFT_RESET = 0x30A2,
	SHT3X_COMMAND_HEATER_ENABLE = 0x306d,
	SHT3X_COMMAND_HEATER_DISABLE = 0x3066,
	SHT3X_COMMAND_READ_STATUS = 0xf32d,
	SHT3X_COMMAND_FETCH_DATA = 0xe000,
	SHT3X_COMMAND_MEASURE_HIGHREP_10HZ = 0x2737,
	SHT3X_COMMAND_MEASURE_LOWREP_10HZ = 0x272a
} sht3x_command_t;

static uint8_t sht3x_calculate_crc(const uint8_t *data, size_t length);
int sht3x_init();
static int sht3x_send_command(sht3x_command_t command);
static uint16_t sht3x_uint8_to_uint16(uint8_t msb, uint8_t lsb);
int sht3x_read_temperature_and_humidity(struct sensor_data_t* data_buff);

// HAL sensor function
void hal_read_sensor_data(struct sensor_data_t* data_buff){
	static int sht3x_init_flag = 0;
	int ret_val = 0;
	if (data_buff != NULL){
		if (sht3x_init_flag == 0){
			ret_val = sht3x_init();
			if (ret_val == 1){
				// SHT30 INIT OK
				sht3x_init_flag = 1;
				data_buff->temp = 33.345;
			}
			else {data_buff->temp = 0.1;}
		}
		else{
			sht3x_read_temperature_and_humidity(data_buff);
			//data_buff->temp = 11.111;
			//data_buff->r_hum = 99.9;
		}

	}
	else{
		Error_Handler();
	}
}

// SHT3x Functions

static uint8_t sht3x_calculate_crc(const uint8_t *data, size_t length)
{
	uint8_t crc = 0xff;
	for (size_t i = 0; i < length; i++) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if ((crc & 0x80u) != 0) {
				crc = (uint8_t)((uint8_t)(crc << 1u) ^ 0x31u);
			} else {
				crc <<= 1u;
			}
		}
	}
	return crc;
}


static int sht3x_send_command(sht3x_command_t command)
{
	uint8_t command_buffer[2] = {(command & 0xff00u) >> 8u, command & 0xffu};

	if (HAL_I2C_Master_Transmit(&hi2c1, SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH << 1u, command_buffer, sizeof(command_buffer),
	                            SHT3X_I2C_TIMEOUT) != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 SEND COMMAND FAILED\r\n");
		return 0;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "SHT30 SEND COMMAND OK\r\n");
	return 1;
}

static uint16_t sht3x_uint8_to_uint16(uint8_t msb, uint8_t lsb)
{
	return (uint16_t)((uint16_t)msb << 8u) | lsb;
}

int sht3x_init()
{
	// assert(handle->i2c_handle->Init.NoStretchMode == I2C_NOSTRETCH_DISABLE);
	// TODO: Assert i2c frequency is not too high

	uint8_t status_reg_and_checksum[3];
	if (HAL_I2C_Mem_Read(&hi2c1, SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH << 1u, SHT3X_COMMAND_READ_STATUS, 2, (uint8_t*)&status_reg_and_checksum,
					  sizeof(status_reg_and_checksum), SHT3X_I2C_TIMEOUT) != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "INIT SHT30 HAL_I2C_Mem_Read FAILED\r\n");
		return 0;
	}

	uint8_t calculated_crc = sht3x_calculate_crc(status_reg_and_checksum, 2);

	if (calculated_crc != status_reg_and_checksum[2]) {
		APP_LOG(TS_OFF, VLEVEL_M, "INIT SHT30 CRC CHECK FAILED\r\n");
		return 0;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "INIT SHT30 OK - STATUS REG VAL: %02X %02X %02X\r\n",
			status_reg_and_checksum[0], status_reg_and_checksum[1], status_reg_and_checksum[2]);
	return 1;
}

int sht3x_read_temperature_and_humidity(struct sensor_data_t* data_buff)
{
	sht3x_send_command(SHT3X_COMMAND_MEASURE_HIGHREP_STRETCH);

	HAL_Delay(1);

	uint8_t buffer[6];
	if (HAL_I2C_Master_Receive(&hi2c1, SHT3X_I2C_DEVICE_ADDRESS_ADDR_PIN_HIGH << 1u, buffer, sizeof(buffer), SHT3X_I2C_TIMEOUT) != HAL_OK) {
		return 0;
	}

	uint8_t temperature_crc = sht3x_calculate_crc(buffer, 2);
	uint8_t humidity_crc = sht3x_calculate_crc(buffer + 3, 2);
	if (temperature_crc != buffer[2] || humidity_crc != buffer[5]) {
		return false;
	}

	uint16_t temperature_raw = sht3x_uint8_to_uint16(buffer[0], buffer[1]);
	uint16_t humidity_raw = sht3x_uint8_to_uint16(buffer[3], buffer[4]);

	data_buff->temp = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
	data_buff->r_hum = 100.0f * (float)humidity_raw / 65535.0f;

	return 1;
}


