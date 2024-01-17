/*
 * sht30.c
 *
 *  Created on: Jan 16, 2024
 *      Author: brunocasu
 */

#include "lora_app.h"
#include "platform.h"


void hal_read_sensor_data(struct sensor_data_t* data_buff){

	if (data_buff != NULL){
		data_buff->temp = 22.2;
		data_buff->r_hum = 99.9;
	}
	else{
		Error_Handler();
	}

}
