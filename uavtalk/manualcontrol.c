/* Helper functions based off UAVTalk from OpenPilot to convert DS4 data to
 * information our quadcopter can understand */
#include <string.h>

#include "manualcontrol.h"

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

// Returns num bytes in buffer
int controller_data_to_control_command(const ds4_controls_t* ds4, uint8_t* buf) {
  int headerSize = UAVTalkHeader(buf, UAVTALK_MESSAGE_TYPE_OBJ, MANUALCONTROLCOMMAND_OBJID, sizeof(ManualControlCommandData));
  
  ManualControlCommandData* controls = (ManualControlCommandData *) (buf + headerSize);
  
  controls->Throttle = (ds4->right_analog_y > 127) ? (ds4->right_analog_y - 127.0f) / 128.0f : 0.0f;
  controls->Roll     = (ds4->left_analog_x - 128.0f) / 128.0f;
  controls->Pitch    = (ds4->left_analog_y - 128.0f) / 128.0f;
  controls->Yaw      = (ds4->right_analog_x - 128.0f) / 128.0f;

  controls->Collective = 0.0f;
  controls->Thrust     = (ds4->right_analog_y > 127) ? (ds4->right_analog_y - 127.0f) / 128.0f : 0.0f;
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
