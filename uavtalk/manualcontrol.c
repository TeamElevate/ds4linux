/* Helper functions based off UAVTalk from OpenPilot to convert DS4 data to
 * information our quadcopter can understand */
#include <string.h>

#include "manualcontrol.h"
#include "uavtalkheader.h"

//STOLEN FROM OPENPILOT.
// /build/uavobject-synthetics/flight/manualcontrolcommand.h

#define MANUALCONTROLCOMMAND_OBJID 0x161A2C98

// Enumeration options for field Connected
typedef enum {
    MANUALCONTROLCOMMAND_CONNECTED_FALSE=0,
    MANUALCONTROLCOMMAND_CONNECTED_TRUE=1
} ManualControlCommandConnectedOptions;

/*
 * Packed Object data (unaligned).
 * Should only be used where 4 byte alignment can be guaranteed
 * (eg a single instance on the heap)
 */
typedef struct {
    float Throttle;
    float Roll;
    float Pitch;
    float Yaw;
    float Collective;
    float Thrust;
    uint16_t Channel[9];
    uint8_t Connected;
    uint8_t FlightModeSwitchPosition;
} __attribute__((packed)) ManualControlCommandDataPacked;

/*
 * Packed Object data.
 * Alignment is forced to 4 bytes so as to avoid the potential for CPU usage faults
 * on Cortex M4F during load/store of float UAVO fields
 */
typedef ManualControlCommandDataPacked __attribute__((aligned(4))) ManualControlCommandData;

// /build/uavobject-synthetics/flight/manualcontrolcommand.h
// END

#define DS4_CENTER_THRESHOLD 5
#define DS4_HOLD 3
#define DS4_MAX (128 - DS4_CENTER_THRESHOLD)

float calcThrottle(int stick, uint8_t dpad, uint8_t activated, uint8_t cruise_on) {
  static float throttle = 0.0f;

  if (cruise_on) {
    if ((dpad == 0 || dpad == 1 || dpad == 7) && activated == 0) {
      throttle += 0.01f;
      if (throttle > 1.0f) throttle = 1.0f;
    } else if ((dpad == 3 || dpad == 4 || dpad == 5) && activated == 0) {
      throttle -= 0.01f;
      if (throttle < 0.0f) throttle = 0.0f;
    }
  } else {
    if (stick > 131.0f) {
      throttle = (stick - 128.0f) / 128.0f;
    } else {
      throttle = 0.0f;
    }
  }
  return throttle;

  /*
  static int last_stick[DS4_HOLD] = {0, 0, 0};
  static int throttle_offset = 0;
  static int proposed_offset_stick = 0;

  int throttle = 0;
  // Normalize the stick around the center.
  if (stick >= 128 - DS4_CENTER_THRESHOLD && stick <= 128 + DS4_CENTER_THRESHOLD)
    stick = 0;
  else if (stick > 128 + DS4_CENTER_THRESHOLD)
    stick = stick - 128 - DS4_CENTER_THRESHOLD;
  else
    stick = stick - 128 + DS4_CENTER_THRESHOLD;

  int stick_deriv = stick - last_stick[0];

  // Correct for center overshoot.
  if ((stick > 0 && stick < 20 && stick_deriv > 80) || (stick < 0 && stick > -20 && stick_deriv < -80))
    stick = 0;

  // Ignore fast changes heading toward the center.
  if ((stick > 0 && stick_deriv < -40) || (stick < 0 && stick_deriv > 40)) {
    throttle = throttle_offset + last_stick[0];
  } else if (stick == 0) {
    if (last_stick[DS4_HOLD - 1] - proposed_offset_stick <= 1 && last_stick[DS4_HOLD - 1] - proposed_offset_stick >= -1) {
      throttle_offset += proposed_offset_stick;
      if (throttle_offset > DS4_MAX)
        throttle_offset = DS4_MAX;
      if (throttle_offset < 0)
        throttle_offset = 0;
    }
    proposed_offset_stick = 0;
    throttle = throttle_offset;
  } else {
    throttle = throttle_offset + stick;
    int val = stick;
    int i;
    for (i = 0; i < DS4_HOLD; i++) {
      if (val - last_stick[i] > 1 || val - last_stick[i] < -1)
        break;
    }
    if (i == DS4_HOLD)
      proposed_offset_stick = val;
  }

  int i;
  for (i = DS4_HOLD - 1; i >= 1; i--)
    last_stick[i] = last_stick[i - 1];
  last_stick[0] = stick;

  if (throttle > DS4_MAX)
    throttle = DS4_MAX;
  if (throttle < 0)
    throttle = 0;
  return throttle / (float) DS4_MAX;*/
}

// Returns num bytes in buffer
uint16_t controller_data_to_control_command(const ds4_controls_t* ds4, uint8_t* buf) {
  static uint8_t last_l1 = 0;
  static uint8_t cruise_on = 0;
  int headerSize = makeUAVTalkHeader(buf, UAVTALK_MESSAGE_TYPE_OBJ, MANUALCONTROLCOMMAND_OBJID, sizeof(ManualControlCommandData));

  ManualControlCommandData* controls = (ManualControlCommandData *) (buf + headerSize);

  cruise_on ^= ds4->l1 == 1 && last_l1 == 0;
  if (ds4->left_analog_y < 3) {
    cruise_on = 0;
  }
  if (ds4->share && ds4->options) {
    cruise_on = 0;
  }

  float throttle = calcThrottle(ds4->left_analog_y, ds4->dpad, ds4->_unknown, cruise_on);
  last_l1 = ds4->l1;
  controls->Throttle = throttle;
  controls->Thrust   = throttle;
  controls->Roll     = (ds4->right_analog_x - 128.0f) / 128.0f;
  controls->Pitch    = (ds4->right_analog_y - 128.0f) / 128.0f;
  controls->Yaw      = (ds4->r2_analog - ds4->l2_analog) / 256.0f;

  controls->Collective = 0.0f;
  int i;
  for (i = 0; i < 9; i++) {
    controls->Channel[i] = 0;
  }
  // dpad           : 3;
  controls->Channel[0] |= (ds4->dpad << 0);
  // _unknown       : 1;
  controls->Channel[0] |= (ds4->_unknown << 3);
  // square         : 1;
  controls->Channel[0] |= (ds4->square << 4);
  // cross          : 1;
  controls->Channel[0] |= (ds4->cross << 5);
  // circle         : 1;
  controls->Channel[0] |= (ds4->circle << 6);
  // triangle       : 1;
  controls->Channel[0] |= (ds4->triangle << 7);

  // l1             : 1;
  controls->Channel[1] |= (ds4->l1 << 0);
  // r1             : 1;
  controls->Channel[1] |= (ds4->r1 << 1);
  // l2             : 1;
  controls->Channel[1] |= (ds4->l2 << 2);
  // r2             : 1;
  controls->Channel[1] |= (ds4->r2 << 3);
  // share          : 1;
  controls->Channel[1] |= (ds4->share << 4);
  // options        : 1;
  controls->Channel[1] |= (ds4->options << 5);
  // l3             : 1;
  controls->Channel[1] |= (ds4->l3 << 6);
  // r3             : 1;
  controls->Channel[1] |= (ds4->r3 << 7);

  // Not really sure what this does yet
  controls->Connected = 1;
  controls->FlightModeSwitchPosition = 0;

  return headerSize + sizeof(ManualControlCommandData);
}
