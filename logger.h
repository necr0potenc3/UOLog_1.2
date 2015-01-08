#ifndef _LOGGER_H_
#define _LOGGER_H_

char *TimeStamp(char *format, ...);
void NumOut(char *format, ...);
char *FormatLogMessage(unsigned char *rawpacket, unsigned int len, int type);
void DumpPacket(unsigned char *rawpacket, int len, int type);
void FinalizePacketLog(void);
char *InitLogger(void);

#define MAX_TIME 250

#endif /* _PACKETS_H_ */