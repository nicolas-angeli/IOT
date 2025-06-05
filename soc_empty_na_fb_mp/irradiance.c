/*
 * irradiance.c
 *
 *  Created on: 5 juin 2025
 *      Author: matth
 */
#include "math.h"
#include "app_log.h"
#include "app.h"
#include "irradiance.h"
#include "sl_status.h"
#include "sl_sensor_light.h"

sl_status_t read_irradiance(int32_t *lux_addr, int32_t *uvi_addr) {
  float uvi;
  float l;

  sl_status_t read_status =  sl_sensor_light_get(&l, &uvi);
  if (!read_status || 1) {
    //l = l/10;
    *lux_addr = 10*l;//en lux
    *uvi_addr = (int32_t) uvi;
  }
  return read_status;
}
