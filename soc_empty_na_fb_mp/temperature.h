/*
 * temperature.h
 *
 *  Created on: 12 mai 2025
 *      Author: matth
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

//int16_t convert_to_BLE_unit(int32_t raw_temp);
sl_status_t read_temperature(int32_t *temp_addr);

#endif /* TEMPERATURE_H_ */
