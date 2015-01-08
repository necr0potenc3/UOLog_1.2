#ifndef _PACKETS_H_
#define _PACKETS_H_

char *GetPacketName(unsigned char *packet, int len);
int CheckFilter(unsigned char *Packet, int len);
void OnPacket(HANDLE Process, DEBUG_EVENT *dbgev, CONTEXT *context, int type);
int HandlePacket(unsigned char *packet, int len, int type);

#endif /* _PACKETS_H_ */