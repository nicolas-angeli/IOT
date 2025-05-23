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

sl_status_t read_temperature(int32_t *temp_addr) {
  uint32_t rh;
  int32_t t;

  sl_status_t read_status =  sl_sensor_rht_get(&rh, &t);
  if (!read_status) {
    t = t/10;
    //app_log_info("%s: Temperature read (BLE unit : raw/10) : %ld\n", __FUNCTION__, t);
    *temp_addr = t;
  }
  return read_status;
}
