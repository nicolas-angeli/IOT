/*
 * temperature.c
 *
 *  Created on: 12 mai 2025
 *      Author: matth
 */
#include "math.h"
#include "app_log.h"
#include "app.h"
#include "temperature.h"
#include "sl_status.h"
#include "sl_sensor_rht.h"

// int16_t convert_to_BLE_unit(int32_t raw_temp) {
//   int8_t M = 1;
//   int8_t d = -2;
//   //int8_t b = 0;
//   return (int16_t)(raw_temp * M * pow((double)10, (double) d));
// }

sl_status_t read_temperature(int16_t* raw_BLE_value) {
  uint32_t rh;
  int32_t t;

  sl_status_t read_status =  sl_sensor_rht_get(&rh, &t);
  if (!read_status) {
    *raw_BLE_value = t/10;
    app_log_info("%s: Temperature read (BLE unit) : %d\n", __FUNCTION__, *raw_BLE_value);
  }

  return read_status;
}
