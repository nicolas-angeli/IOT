/*
 * temperature.h
 *
 *  Created on: 12 mai 2025
 *      Author: matth
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

//int16_t convert_to_BLE_unit(int32_t raw_temp);
sl_status_t read_temperature(int16_t* raw_BLE_value);

#endif /* TEMPERATURE_H_ */
