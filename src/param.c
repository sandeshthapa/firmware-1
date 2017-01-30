#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#include <stdbool.h>
#include <stdint.h>

#include "flash.h"
#include "mavlink.h"
#include "mavlink_param.h"
#include "mavlink_stream.h"

#include "param.h"
#include "mixer.h"
//#include "rc.h" <-- I want to include this file so I can manually specify the RC type.  But I get errors if I do

// global variable definitions
params_t _params;

// local function definitions
static void init_param_int(param_id_t id, char name[PARAMS_NAME_LENGTH], int32_t value)
{
  memcpy(_params.names[id], name, PARAMS_NAME_LENGTH);
  _params.values[id] = value;
  _params.types[id] = PARAM_TYPE_INT32;
}

static void init_param_float(param_id_t id, char name[PARAMS_NAME_LENGTH], float value)
{
  memcpy(_params.names[id], name, PARAMS_NAME_LENGTH);
  _params.values[id] = *((int32_t *) &value);
  _params.types[id] = PARAM_TYPE_FLOAT;
}

// function definitions
void init_params(void)
{
  for(uint8_t i = 0; i < PARAMS_COUNT; i++)
  {
    init_param_int(i, "DEFAULT", 0);
  }
  initEEPROM();
  if (!read_params())
  {
    set_param_defaults();
    write_params();
  }

  for (uint16_t id = 0; id < PARAMS_COUNT; id++)
    param_change_callback((param_id_t) id);
}

void set_param_defaults(void)
{
  /******************************/
  /*** HARDWARE CONFIGURATION ***/
  /******************************/
  init_param_int(PARAM_BOARD_REVISION, "BOARD_REV", 2);
  init_param_int(PARAM_BAUD_RATE, "BAUD_RATE", 921600);

  /*****************************/
  /*** MAVLINK CONFIGURATION ***/
  /*****************************/
  init_param_int(PARAM_SYSTEM_ID, "SYS_ID", 1);
  init_param_int(PARAM_STREAM_HEARTBEAT_RATE, "STRM_HRTBT", 1);

  init_param_int(PARAM_STREAM_ATTITUDE_RATE, "STRM_ATTITUDE", 100);
  init_param_int(PARAM_STREAM_IMU_RATE, "STRM_IMU", 500);
  init_param_int(PARAM_STREAM_MAG_RATE, "STRM_MAG", 75);
  init_param_int(PARAM_STREAM_BARO_RATE, "STRM_BARO", 100);
  init_param_int(PARAM_STREAM_AIRSPEED_RATE, "STRM_AIRSPEED", 20);
  init_param_int(PARAM_STREAM_GPS_RATE, "STRM_GPS", 0);
  init_param_int(PARAM_STREAM_SONAR_RATE, "STRM_SONAR", 40);

  init_param_int(PARAM_STREAM_SERVO_OUTPUT_RAW_RATE, "STRM_SERVO", 50);
  init_param_int(PARAM_STREAM_RC_RAW_RATE, "STRM_RC", 50);

  /********************************/
  /*** CONTROLLER CONFIGURATION ***/
  /********************************/
  init_param_int(PARAM_MAX_COMMAND, "PARAM_MAX_CMD", 1000);

  init_param_float(PARAM_PID_ROLL_RATE_P, "PID_ROLL_RATE_P", 0.070f);
  init_param_float(PARAM_PID_ROLL_RATE_I, "PID_ROLL_RATE_I", 0.000f);
  init_param_float(PARAM_PID_ROLL_RATE_D, "PID_ROLL_RATE_D", 0.000f);
  init_param_float(PARAM_ROLL_RATE_TRIM, "ROLLRATE_TRIM", 0.0f);
  init_param_float(PARAM_MAX_ROLL_RATE, "MAX_ROLL_RATE", 3.14159f);

  init_param_float(PARAM_PID_PITCH_RATE_P, "PID_PITCH_RATE_P", 0.070f);
  init_param_float(PARAM_PID_PITCH_RATE_I, "PID_PITCH_RATE_I", 0.0000f);
  init_param_float(PARAM_PID_PITCH_RATE_D, "PID_PITCH_RATE_D", 0.0000f);
  init_param_float(PARAM_PITCH_RATE_TRIM, "PITCHRATE_TRIM", 0.0f);
  init_param_float(PARAM_MAX_PITCH_RATE, "MAX_PITCH_RATE", 3.14159f);

  init_param_float(PARAM_PID_YAW_RATE_P, "PID_YAW_RATE_P", 0.25f);
  init_param_float(PARAM_PID_YAW_RATE_I, "PID_YAW_RATE_I", 0.0f);
  init_param_float(PARAM_PID_YAW_RATE_D, "PID_YAW_RATE_I", 0.0f);
  init_param_float(PARAM_YAW_RATE_TRIM, "YAWRATE_TRIM", 0.0f);
  init_param_float(PARAM_MAX_YAW_RATE, "MAX_YAW_RATE", 6.283f);

  init_param_float(PARAM_PID_ROLL_ANGLE_P, "PID_ROLL_ANG_P", 0.15f);
  init_param_float(PARAM_PID_ROLL_ANGLE_I, "PID_ROLL_ANG_I", 0.0f);
  init_param_float(PARAM_PID_ROLL_ANGLE_D, "PID_ROLL_ANG_D", 0.07f);
  init_param_float(PARAM_ROLL_ANGLE_TRIM, "ROLL_TRIM", 0.0f);
  init_param_float(PARAM_MAX_ROLL_ANGLE, "MAX_ROLL_ANG", 0.786f);

  init_param_float(PARAM_PID_PITCH_ANGLE_P, "PID_PITCH_ANG_P", 0.15f);
  init_param_float(PARAM_PID_PITCH_ANGLE_I, "PID_PITCH_ANG_I", 0.0f);
  init_param_float(PARAM_PID_PITCH_ANGLE_D, "PID_PITCH_ANG_D", 0.07f);
  init_param_float(PARAM_PITCH_ANGLE_TRIM, "PITCH_TRIM", 0.0f);
  init_param_float(PARAM_MAX_PITCH_ANGLE, "MAX_PITCH_ANG", 0.786);

  init_param_float(PARAM_PID_ALT_P, "PID_ALT_P", 0.0f);
  init_param_float(PARAM_PID_ALT_I, "PID_ALT_I", 0.0f);
  init_param_float(PARAM_PID_ALT_D, "PID_ALT_D", 0.0f);
  init_param_float(PARAM_HOVER_THROTTLE, "FAILSAFE_THR", 0.5);

  init_param_float(PARAM_PID_TAU, "PID_TAU", 0.05f);


  /*************************/
  /*** PWM CONFIGURATION ***/
  /*************************/
  init_param_int(PARAM_MOTOR_PWM_SEND_RATE, "MOTOR_PWM_UPDATE", 490);
  init_param_int(PARAM_MOTOR_IDLE_PWM, "MOTOR_IDLE_PWM", 1100);
  init_param_int(PARAM_SPIN_MOTORS_WHEN_ARMED, "ARM_SPIN_MOTORS", true);

  /*******************************/
  /*** ESTIMATOR CONFIGURATION ***/
  /*******************************/
  init_param_int(PARAM_INIT_TIME, "FILTER_INIT_T", 3000); // ms
  init_param_float(PARAM_FILTER_KP, "FILTER_KP", 1.0f);
  init_param_float(PARAM_FILTER_KI, "FILTER_KI", 0.1f);

  init_param_float(PARAM_GYRO_ALPHA, "GYRO_LPF_ALPHA", 0.888f);
  init_param_float(PARAM_ACC_ALPHA, "ACC_LPF_ALPHA", 0.888f);

  init_param_float(PARAM_ACCEL_SCALE, "ACCEL_SCALE", 1.0f);

  init_param_float(PARAM_GYRO_X_BIAS, "GYRO_X_BIAS", 0.0f);
  init_param_float(PARAM_GYRO_Y_BIAS, "GYRO_Y_BIAS", 0.0f);
  init_param_float(PARAM_GYRO_Z_BIAS, "GYRO_Z_BIAS", 0.0f);
  init_param_float(PARAM_ACC_X_BIAS,  "ACC_X_BIAS", 0.0f);
  init_param_float(PARAM_ACC_Y_BIAS,  "ACC_Y_BIAS", 0.0f);
  init_param_float(PARAM_ACC_Z_BIAS,  "ACC_Z_BIAS", 0.0f);
  init_param_float(PARAM_ACC_X_TEMP_COMP,  "ACC_X_TEMP_COMP", 0.0f);
  init_param_float(PARAM_ACC_Y_TEMP_COMP,  "ACC_Y_TEMP_COMP", 0.0f);
  init_param_float(PARAM_ACC_Z_TEMP_COMP,  "ACC_Z_TEMP_COMP", 0.0f);

  /************************/
  /*** RC CONFIGURATION ***/
  /************************/
  init_param_int(PARAM_RC_TYPE, "RC_TYPE",1);
  init_param_int(PARAM_RC_X_CHANNEL, "RC_X_CHN", 0);
  init_param_int(PARAM_RC_Y_CHANNEL, "RC_Y_CHN", 1);
  init_param_int(PARAM_RC_Z_CHANNEL, "RC_Z_CHN", 3);
  init_param_int(PARAM_RC_F_CHANNEL, "RC_F_CHN", 2);
  init_param_int(PARAM_RC_ATTITUDE_OVERRIDE_CHANNEL, "RC_ATT_OVRD_CHN", 4);
  init_param_int(PARAM_RC_THROTTLE_OVERRIDE_CHANNEL, "RC_THR_OVRD_CHN", 4);
  init_param_int(PARAM_RC_ATT_CONTROL_TYPE_CHANNEL,  "RC_ATT_CTRL_CHN", 5);
  init_param_int(PARAM_RC_F_CONTROL_TYPE_CHANNEL,    "RC_F_CTRL_CHN", 7);
  init_param_int(PARAM_RC_NUM_CHANNELS, "RC_NUM_CHN", 6);

  init_param_int(PARAM_RC_X_CENTER, "RC_X_CENTER", 1500);
  init_param_int(PARAM_RC_Y_CENTER, "RC_Y_CENTER", 1500);
  init_param_int(PARAM_RC_Z_CENTER, "RC_Z_CENTER", 1500);
  init_param_int(PARAM_RC_F_BOTTOM, "RC_F_BOTTOM", 1000);
  init_param_int(PARAM_RC_X_RANGE,  "RC_X_RANGE", 1000);
  init_param_int(PARAM_RC_Y_RANGE,  "RC_Y_RANGE", 1000);
  init_param_int(PARAM_RC_Z_RANGE,  "RC_Z_RANGE", 1000);
  init_param_int(PARAM_RC_F_RANGE,  "RC_F_RANGE", 1000);
  init_param_int(PARAM_RC_SWITCH_5_DIRECTION, "SWITCH_5_DIR", 1);
  init_param_int(PARAM_RC_SWITCH_6_DIRECTION, "SWITCH_6_DIR", 1);
  init_param_int(PARAM_RC_SWITCH_7_DIRECTION, "SWITCH_7_DIR", 1);
  init_param_int(PARAM_RC_SWITCH_8_DIRECTION, "SWITCH_8_DIR", 1);

  init_param_int(PARAM_RC_OVERRIDE_DEVIATION, "RC_OVRD_DEV", 100);
  init_param_int(PARAM_OVERRIDE_LAG_TIME, "OVRD_LAG_TIME", 1000);
  init_param_int(PARAM_RC_OVERRIDE_TAKE_MIN_THROTTLE, "MIN_THROTTLE", false);

  init_param_float(PARAM_RC_MAX_ROLL, "RC_MAX_ROLL", 0.786f);
  init_param_float(PARAM_RC_MAX_PITCH, "RC_MAX_PITCH", 0.786f);
  init_param_float(PARAM_RC_MAX_ROLLRATE, "RC_MAX_ROLLRATE", 3.14159f);
  init_param_float(PARAM_RC_MAX_PITCHRATE, "RC_MAX_PITCHRATE", 3.14159f);
  init_param_float(PARAM_RC_MAX_YAWRATE, "RC_MAX_YAWRATE", 0.786f);

  /***************************/
  /*** FRAME CONFIGURATION ***/
  /***************************/
  init_param_int(PARAM_MIXER, "MIXER", QUADCOPTER_X);

  init_param_int(PARAM_FIXED_WING, "FIXED_WING", false);
  init_param_int(PARAM_ELEVATOR_REVERSE, "ELEVATOR_REV", 0);
  init_param_int(PARAM_AILERON_REVERSE, "AIL_REV", 0);
  init_param_int(PARAM_RUDDER_REVERSE, "RUDDER_REV", 0);

  /********************/
  /*** ARMING SETUP ***/
  /********************/
  init_param_int(PARAM_ARM_STICKS, "ARM_STICKS", true);
  init_param_int(PARAM_ARM_CHANNEL, "ARM_CHANNEL", 5);
  init_param_int(PARAM_ARM_THRESHOLD, "ARM_THRESHOLD", 150);
}

bool read_params(void)
{
  return readEEPROM();
}

bool write_params(void)
{
  return writeEEPROM(true);
}

void param_change_callback(param_id_t id)
{
  switch (id)
  {
  case PARAM_SYSTEM_ID:
    mavlink_system.sysid = get_param_int(PARAM_SYSTEM_ID);
    break;
  case PARAM_STREAM_HEARTBEAT_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_HEARTBEAT, get_param_int(PARAM_STREAM_HEARTBEAT_RATE));
    break;

  case PARAM_STREAM_ATTITUDE_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_ATTITUDE, get_param_int(PARAM_STREAM_ATTITUDE_RATE));
    break;

  case PARAM_STREAM_IMU_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_IMU, get_param_int(PARAM_STREAM_IMU_RATE));
    break;
  case PARAM_STREAM_AIRSPEED_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_DIFF_PRESSURE, get_param_int(PARAM_STREAM_AIRSPEED_RATE));
    break;
  case PARAM_STREAM_SONAR_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_SONAR, get_param_int(PARAM_STREAM_SONAR_RATE));
    break;
  case  PARAM_STREAM_BARO_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_BARO, get_param_int(PARAM_STREAM_BARO_RATE));
    break;
  case  PARAM_STREAM_MAG_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_MAG, get_param_int(PARAM_STREAM_MAG_RATE));
    break;

  case PARAM_STREAM_SERVO_OUTPUT_RAW_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_SERVO_OUTPUT_RAW, get_param_int(PARAM_STREAM_SERVO_OUTPUT_RAW_RATE));
    break;
  case PARAM_STREAM_RC_RAW_RATE:
    mavlink_stream_set_rate(MAVLINK_STREAM_ID_RC_RAW, get_param_int(PARAM_STREAM_RC_RAW_RATE));
    break;

  case PARAM_RC_TYPE:
    init_PWM();
    break;
  case PARAM_MOTOR_PWM_SEND_RATE:
    init_PWM();
    break;
  case PARAM_MIXER:
    init_mixing();
    break;

  default:
    // no action needed for this parameter
    break;
  }
}

param_id_t lookup_param_id(const char name[PARAMS_NAME_LENGTH])
{
  for (uint16_t id = 0; id < PARAMS_COUNT; id++)
  {
    bool match = true;
    for (uint8_t i = 0; i < PARAMS_NAME_LENGTH; i++)
    {
      // compare each character
      if (name[i] != _params.names[id][i])
      {
        match = false;
        break;
      }

      // stop comparing if end of string is reached
      if (_params.names[id][i] == '\0')
        break;
    }

    if (match)
      return (param_id_t) id;
  }

  return PARAMS_COUNT;
}

int get_param_int(param_id_t id)
{
  return _params.values[id];
}

float get_param_float(param_id_t id)
{
  return *(float *) &_params.values[id];
}

char *get_param_name(param_id_t id)
{
  return _params.names[id];
}

param_type_t get_param_type(param_id_t id)
{
  return _params.types[id];
}

bool set_param_int(param_id_t id, int32_t value)
{
  if (id < PARAMS_COUNT && value != _params.values[id])
  {
    _params.values[id] = value;
    param_change_callback(id);
    mavlink_send_param(id);
    return true;
  }
  return false;
}

bool set_param_float(param_id_t id, float value)
{
  return set_param_int(id, *(int32_t *) &value);
}

bool set_param_by_name_int(const char name[PARAMS_NAME_LENGTH], int32_t value)
{
  param_id_t id = lookup_param_id(name);
  return set_param_int(id, value);
}

bool set_param_by_name_float(const char name[PARAMS_NAME_LENGTH], float value)
{
  return set_param_by_name_int(name, *(int32_t *) &value);
}
