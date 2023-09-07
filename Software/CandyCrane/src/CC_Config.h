#pragma once
#include <esp_task.h>

#define __DEVICE_NAME__ "CandyCrane"
#define __DEVICE_VERSION__ "0.1"

#define CC_BUCKET_MAC "84:CC:A8:83:D0:92"

#define NETWORK_WAIT_TIME_PER_TRY 30 //seconds
#define NETWORK_TRY_TIMES 3

#define WIFI_PREFERENCES_FILENAME "wifi"
#define WIFI_PREFERENCES_SSID_KEY "ssid"
#define WIFI_PREFERENCES_PASWORD_KEY "password"

/* CORE and PRIORITY Assignments */
#define STACK_SIZE (ESP_TASK_MAIN_STACK) // Stack size for each new thread

#define CRANE_CONTROL_PRIORITY tskIDLE_PRIORITY+10
#define CRANE_CONTROL_QUEUE_HANDLER_PRIORITY tskIDLE_PRIORITY+11
#define CANDY_CRANE_COMMS_HANDLER_PRIORITY tskIDLE_PRIORITY+12
#define OTA_PRIORITY tskIDLE_PRIORITY+10
#define WEB_SERVER_PRIORITY tskIDLE_PRIORITY+12

#define CRANE_CONTROL_CORE 0
#define CRANE_CONTROL_QUEUE_HANDLER_CORE 0
#define CANDY_CRANE_COMMS_CORE 1
#define WEBSERVER_CORE	1
#define OTA_CORE	1

/* Crane Controller */
#define BUCKET_HEARTBEAT_INTERVAL 500
#define BUCKET_LOST_PONG_COUNT 30
#define BUCKET_HEARBEATS_MAX_MISSED 1

#define BUCKET_NO_DISTANCE -1
#define BUCKET_COMMAND_WAIT_TIME 5000
#define BUCKET_MOVE_WAIT_TIME 10000
#define BUCKET_HOME_DISTANCE 50 //mm

#define MM_FROM_SWITCH_FOR_HOME 1

#define MULTITINYSTEPPER_RESET_PIN 26
#define DOLLY_LIMIT_SWITCH_PIN 33
#define DOLLY_STEPS_PER_MM 26 //12.2  //6.1
#define DOLLY_MAXIMUM_DISTANCE 240.0
#define DOLLY_SPEED_STEPS_SECOND 700
#define DOLLY_ACCEL_STEPS_SECOND 700
#define DOLLY_CALIBRATION_SPEED 400


#define ROPE_BARREL_STEPPER_R MTS_STEPPER_1
#define ROPE_BARREL_STEPPER_R_DIR true
#define ROPE_BARREL_STEPPER_L MTS_STEPPER_2
#define ROPE_BARREL_STEPPER_L_DIR false

#define ROPE_BARREL_STEPS_PER_MM 25.6 //12.8  //24.3 //48.6
#define ROPE_BARREL_MAXIMUM_DISTANCE 1000
#define ROPE_BARREL_SPEED_STEPS_SECOND 700
#define ROPE_BARREL_ACCEL_STEPS_SECOND 700
#define ROPE_BARREL_CALIBRATION_SPEED 300
#define BUCKET_DEFAULT_DISTANCE 40.0

#define TOWER_LIMIT_SWITCH_PIN 25
#define TOWER_STEPS_PER_MM 6.5 //13.0
#define TOWER_MAXIMUM_DISTANCE 215.0
#define TOWER_SPEED_STEPS_SECOND 250
#define TOWER_ACCEL_STEPS_SECOND 250
#define TOWER_CALIBRATION_SPEED 125

#define DEFAULT_BUCKET_CLOSED_ANGLE  105
#define DEFAULT_BUCKET_OPEN_ANGLE 40
#define DEFAULT_BUCKET_OPEN_SPEED 60
#define DEFAULT_BUCKET_CLOSE_SPEED 60

#define DEFAULT_BUCKET_PING_RATE 10000 //10 sec
#define DEFUALT_MAX_MISSED_PINGS 3

#define CANDY_RANGER_SERVO_PIN 5
#define CANDY_RANGER_DEFAULT_SPEED 40
#define CANDY_RANGER_FAST_SPEED 55
#define CANDY_RANGER_SCAN_SPEED  30

#define CANDY_RANGER_HOME_POS 20
#define CANDY_RANGER_ZONE_A_START 55
#define CANDY_RANGER_ZONE_A_END 80
#define CANDY_RANGER_ZONE_B_START 95
#define CANDY_RANGER_ZONE_B_END 115

#define CANDY_RANGER_NUM_SAMPLES 5

#define WEBPAGE_NETWORK "/web/net.htm"
#define WEBPAGE_ROOT "/web/root.htm"
#define WEBPAGE_CC_CONTROL "/web/contr.htm"