/* Helper functions based off UAVTalk from OpenPilot to convert DS4 data to
 * information our quadcopter can understand */
#include "manualcontrol.h"
#include "crc.h"

//STOLEN FROM OPENPILOT.
// /build/uavobject-synthetics/flight/manualcontrolcommand.h

#define MANUALCONTROLCOMMAND_OBJID 0x161A2C98
#define MANUALCONTROLCOMMAND_ISSINGLEINST 1
#define MANUALCONTROLCOMMAND_ISSETTINGS 0
#define MANUALCONTROLCOMMAND_ISPRIORITY 0
#define MANUALCONTROLCOMMAND_NUMBYTES sizeof(ManualControlCommandData)

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

#define UAVTALK_SYNC_VAL 0x3C
#define UAVTALK_MESSAGE_TYPE_OBJ 0x0
#define UAVTALK_VERSION 0x2

typedef struct {
  uint8_t  SyncVal     : 8;
  uint8_t  MessageType : 4;
  uint8_t  Version     : 3;
  uint8_t  UseTimestamp: 1; /* Should be 0 for now */
  uint16_t Length      : 16;
  uint32_t ObjectID    : 32;
  uint32_t InstanceID  : 16;
} __attribute__((packed)) UAVTalkHeader;

// Returns num bytes in buffer
int controller_data_to_control_command(const ds4_controls_t* ds4, uint8_t* buf) {
  UAVTalkHeader* header = (UAVTalkHeader*)(buf);

  header->SyncVal      = UAVTALK_SYNC_VAL;
  header->UseTimestamp = 0;
  header->Version      = UAVTALK_VERSION;
  header->MessageType  = UAVTALK_MESSAGE_TYPE_OBJ;
  header->Length       = sizeof(UAVTalkHeader) + sizeof(ManualControlCommandData);
  header->ObjectID     = MANUALCONTROLCOMMAND_OBJID;
  header->InstanceID   = 0;

  ManualControlCommandData* controls = (ManualControlCommandData*)(buf + sizeof(UAVTalkHeader));
  controls->Throttle = (ds4->right_analog_y > 127) ? (ds4->right_analog_y - 127.0f) / 128.0f : 0.0f;
  controls->Roll     = (ds4->left_analog_x - 128.0f) / 128.0f;
  controls->Pitch    = (ds4->left_analog_y - 128.0f) / 128.0f;
  controls->Yaw      = (ds4->right_analog_x - 128.0f) / 128.0f;

  controls->Collective = 0.0f;
  controls->Thrust     = 0.0f;
  int i;
  for (i = 0; i < 9; i++) {
    controls->Channel[i] = 0;
  }

  // Not really sure what this does yet
  controls->Connected = 1;
  controls->FlightModeSwitchPosition = 0;

  uint8_t *crc = buf + header->Length;
  *crc = updateCRC(0x00, buf, header->Length);

  return header->Length + 1;
}
