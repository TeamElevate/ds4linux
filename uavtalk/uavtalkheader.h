#ifndef __UAVTALKHEADER_H__
#define __UAVTALKHEADER_H__

#include <inttypes.h>

#define UAVTALK_SYNC_VAL 0x3C
#define UAVTALK_MESSAGE_TYPE_OBJ 0x0
#define UAVTALK_MESSAGE_TYPE_OBJ_REQ 0x1
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

uint16_t makeUAVTalkHeader(uint8_t *buf, uint8_t type, uint32_t objId, uint16_t length);

#endif
