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


#define SHT30_I2C_TIMEOUT 300
#define SHT30_I2C_ADDR_PIN_HIGH 0x45
// #define SHT30_I2C_ADDR_PIN_LOW 0x44

// SHT30 registers addresses and commands
typedef enum{
	// Single shot data acquisition mode
	// Clock stretching enabled and High repetition mode (12.5 ms for acquiring data)
	SHT30_COMMAND_MEASURE_SINGLE = 0x2c06,

	// Status register read (default value should return 0x8010)
	SHT30_COMMAND_READ_STATUS = 0xf32d,

	// Soft reset / re-initialization
	SHT30_COMMAND_SOFT_RESET = 0x30A2,

} sht30_command_t;

sensor_status_t sht30_calculate_crc(const uint8_t *data, size_t length);
sensor_status_t sht30_check_status_register(struct sensor_data_t* data_buff);
sensor_status_t sht30_send_command(sht30_command_t command);
sensor_status_t sht30_read_temperature_and_humidity(struct sensor_data_t* data_buff);
static uint16_t sht30_uint8_to_uint16(uint8_t msb, uint8_t lsb);

// Sensor status control
sensor_status_t sht30_status = SENSOR_STATUS_UNINITIALIZED;

// Exported HAL sensor functions
sensor_status_t app_read_sensor_data(struct sensor_data_t* data_buff){
	if (data_buff != NULL){
		// First reading, sensor is uninitialized
		if (sht30_status == SENSOR_STATUS_UNINITIALIZED){
			sht30_status = sht30_check_status_register(data_buff);
		}
		// Read sensor values
		if (sht30_status == SENSOR_STATUS_OK){
			sht30_status = sht30_read_temperature_and_humidity(data_buff);
		}
	}
	else{
		Error_Handler();
	}
	return sht30_status;
}

sensor_status_t app_reset_sensor(struct sensor_data_t* data_buff){
	if (sht30_send_command(SHT30_COMMAND_SOFT_RESET) == SENSOR_STATUS_OK){
		HAL_Delay(1);
		// Check status register to confirm if the sensor reset was OK
		sht30_status = sht30_check_status_register(data_buff);
	}
	return sht30_status;
}

// SHT30 functions

/**
 * Check CRC value
 */
sensor_status_t sht30_calculate_crc(const uint8_t *data, size_t length){
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
	// return crc;

	if (crc != data[length]) {
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 CRC Check ERROR\r\n");
		return SENSOR_STATUS_ERROR;
	}
	else{
		return SENSOR_STATUS_OK;
	}
}

/**
 * Check status register value
 * Used to test if communication with the sensor is OK
 * The status register value is included in the LoRa payload
 */
sensor_status_t sht30_check_status_register(struct sensor_data_t* data_buff){
	uint8_t buff[3];
	if (HAL_I2C_Mem_Read(&hi2c1, SHT30_I2C_ADDR_PIN_HIGH << 1u, SHT30_COMMAND_READ_STATUS, 2, (uint8_t*)&buff,
					  sizeof(buff), SHT30_I2C_TIMEOUT) != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 HAL_I2C_Mem_Read ERROR\r\n");
		return SENSOR_STATUS_ERROR;
	}
	// STH30 returned values:
	// buff[0] = MSB of status register (default 0x80)
	// buff[1] = LSB of status register (default 0x10)
	// buff[2] = CRC value

	if(sht30_calculate_crc(buff, 2) == SENSOR_STATUS_OK){
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 Status register and CRC: %02X %02X %02X\r\n", buff[0], buff[1], buff[2]);
		data_buff->status_reg = sht30_uint8_to_uint16(buff[0], buff[1]);
		return SENSOR_STATUS_OK;
	}
	else{
		return SENSOR_STATUS_ERROR;
	}
	//if (calculated_crc != buff[2]) {
	//	APP_LOG(TS_OFF, VLEVEL_M, "INIT SHT30 CRC CHECK FAILED\r\n");
	//	return 0;
	//}
}


/**
 * Send command to the SHT30 sensor
 * I2C handle must be configured according to the Board config.
 * I2C Device address must be configured to match the SHT30 sensor address
 */
sensor_status_t sht30_send_command(sht30_command_t command){
	uint8_t command_buff[2] = {(command & 0xff00u) >> 8u, command & 0xffu};

	if (HAL_I2C_Master_Transmit(&hi2c1, SHT30_I2C_ADDR_PIN_HIGH << 1u, command_buff, sizeof(command_buff),
	                            SHT30_I2C_TIMEOUT) != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 HAL_I2C_Master_Transmit ERROR\r\n");
		return SENSOR_STATUS_ERROR;
	}
	else{
		return SENSOR_STATUS_OK;
	}
}

/**
 * Send a single shot data acquisition command to the SHT30 sensor
 */
sensor_status_t sht30_read_temperature_and_humidity(struct sensor_data_t* data_buff){
	uint8_t buff[6];
	uint8_t temp_buff[3];
	uint8_t r_hum_buff[3];
	uint16_t temperature_raw;
	uint16_t humidity_raw;

	sht30_send_command(SHT30_COMMAND_MEASURE_SINGLE);
	HAL_Delay(1);

	if (HAL_I2C_Master_Receive(&hi2c1, SHT30_I2C_ADDR_PIN_HIGH << 1u, buff, sizeof(buff), SHT30_I2C_TIMEOUT) != HAL_OK) {
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 HAL_I2C_Master_Receive ERROR\r\n");
		return SENSOR_STATUS_ERROR;
	}

	for(int i=0; i<3; i++){
		temp_buff[i] = buff[i];
		r_hum_buff[i] = buff[i+3];
	}

	if (sht30_calculate_crc(temp_buff, 2) != SENSOR_STATUS_OK){
		APP_LOG(TS_OFF, VLEVEL_M, "SHT30 Temperature data CRC ERROR\r\n");
		return SENSOR_STATUS_ERROR;
	}
	else{
		if (sht30_calculate_crc(r_hum_buff, 2) != SENSOR_STATUS_OK){
			APP_LOG(TS_OFF, VLEVEL_M, "SHT30 Humidity data CRC ERROR\r\n");
			return SENSOR_STATUS_ERROR;
		}
		else{
			// CRC ok, convert raw data to float values
			temperature_raw = sht30_uint8_to_uint16(buff[0], buff[1]);
			humidity_raw = sht30_uint8_to_uint16(buff[3], buff[4]);

			data_buff->temp = -45.0f + 175.0f * (float)temperature_raw / 65535.0f;
			data_buff->r_hum = 100.0f * (float)humidity_raw / 65535.0f;
		}
	}
	return SENSOR_STATUS_OK;
}

/**
 * Converts two uint8_t vars into one uint16_t var
 */
static uint16_t sht30_uint8_to_uint16(uint8_t msb, uint8_t lsb){
	return (uint16_t)((uint16_t)msb << 8u) | lsb;
}


