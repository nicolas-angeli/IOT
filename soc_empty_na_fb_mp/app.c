/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#define TEMPERATURE_TIMER_SIGNAL (1<<0)

#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"
#include "gatt_db.h"

#include "temperature.h"
#include <stdint.h>
#include "sl_status.h"
#include "sl_sensor_rht.h"
#include "math.h"
#include "stdbool.h"
// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

bool notify_mode = false;
uint8_t step = 0;
sl_sleeptimer_timer_handle_t handle;

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  app_log_info("%s \n", __FUNCTION__);

}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
static uint8_t connection_handle = SL_BT_INVALID_CONNECTION_HANDLE;

void timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data){
  uint8_t* ptr = data;
  *ptr+=1;
  app_log_info("%s: Timer step %d\n", __FUNCTION__, *ptr);
  sl_bt_external_signal(TEMPERATURE_TIMER_SIGNAL);
}


void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      connection_handle = evt->data.evt_connection_opened.connection;
      app_log_info("%s: Connection opened\n", __FUNCTION__);
      app_log_info("Handler : 0x%x\n", connection_handle);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("%s: Connection closed\n\n\n", __FUNCTION__);
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);
      connection_handle = SL_BT_INVALID_CONNECTION_HANDLE;

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////
    case sl_bt_evt_gatt_server_user_read_request_id:
      int32_t BLE_raw_temperature;
      size_t value_len = sizeof(BLE_raw_temperature);
      uint16_t sent_len;

      uint8_t chan = evt->data.evt_gatt_server_user_read_request.connection;

      if(evt->data.evt_gatt_server_user_read_request.characteristic == gattdb_temperature) {
              app_log_info("%s: Temperature requested\n", __FUNCTION__);
              sl_status_t status = read_temperature(&BLE_raw_temperature);
              app_log_info("%s: Read temperature: %d with status %lu\n", __FUNCTION__, BLE_raw_temperature, status);
              app_log_info("%s: Handler = 0x%x\n", __FUNCTION__, chan);



              sc = sl_bt_gatt_server_send_user_read_response(chan, gattdb_temperature, 0, value_len, (uint8_t*) &BLE_raw_temperature, &sent_len);
      }


      break;
    case sl_bt_evt_gatt_server_characteristic_status_id :
      notify_mode = !notify_mode;
      app_log_info("NOTIFY triggered\n");

      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature && evt->data.evt_gatt_server_characteristic_status.status_flags == 0x1) {
          app_log_info("NOTIFY on temperature\n");
          app_log_info("\t status flags : %x\n", evt->data.evt_gatt_server_characteristic_status.status_flags);

          app_log_info("NOTIFY status: %d\n", notify_mode);
          if(notify_mode) sl_sleeptimer_start_periodic_timer_ms(&handle, 1000, timer_callback, &step, 0, 0);
          else  sl_sleeptimer_stop_timer(&handle);

      } else {
          app_log_info("Wrong characteristic : %d\n"
              "or wrong status flag : %x\n",
              evt->data.evt_gatt_server_characteristic_status.characteristic, evt->data.evt_gatt_server_characteristic_status.status_flags);
      }
      break;
    case sl_bt_evt_system_external_signal_id :
      sl_status_t notification_status;
      int32_t notif_temp;
      int value_size = sizeof(notif_temp);

      //Lecture de la température
      sl_status_t status = read_temperature(&notif_temp);

      //Envoi de la notification
      if (evt->data.evt_system_external_signal.extsignals == TEMPERATURE_TIMER_SIGNAL) {
        notification_status = sl_bt_gatt_server_send_notification(connection_handle, gattdb_temperature, value_size,(uint8_t*) &notif_temp);
        if (notification_status == SL_STATUS_OK) app_log_info("Temperature notification send : %d\n", notif_temp);
      }
      break;
      // -------------------------------
    // Default event handler.
    default:
      break;
  }
}
