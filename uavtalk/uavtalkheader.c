#include "uavtalkheader.h"

uint16_t makeUAVTalkHeader(uint8_t *buf, uint8_t type, uint32_t objId, uint16_t length) {
  UAVTalkHeader* header = (UAVTalkHeader*)(buf);

  header->SyncVal      = UAVTALK_SYNC_VAL;
  header->UseTimestamp = 0;
  header->Version      = UAVTALK_VERSION;
  header->MessageType  = type;
  header->Length       = sizeof(UAVTalkHeader) + length;
  header->ObjectID     = objId;
  header->InstanceID   = 0;

  return sizeof(UAVTalkHeader);
}
