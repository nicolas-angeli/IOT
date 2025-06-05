/*
 * irradiance.h
 *
 *  Created on: 5 juin 2025
 *      Author: matth
 */

#ifndef IRRADIANCE_H_
#define IRRADIANCE_H_

//int16_t convert_to_BLE_unit(int32_t raw_temp);
sl_status_t read_irradiance(int32_t *lux_addr, int32_t *uvi_addr);

#endif /* IRRADIANCE_H_ */
