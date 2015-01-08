#ifndef _GLOBALS_H_
#define _GLOBALS_H_


typedef struct tagFrequency
{
   unsigned long bytecount;
   unsigned long count;
}Frequency;

typedef struct tagDebugLoopParam
{
   DWORD TargetPID;
   int attached;
   char *path;
}DebugLoopParam;

typedef struct tagClientEntry
{
   char Version[50];
   void *Send, *Recv;
   short RegBufSend, RegLenSend, RegBufRecv, RegLenRecv;
}ClientEntry;

typedef struct tagGlobalOptions
{
   HWND hWnd;
   LogWindow *PacketLog;
   HMENU MainMenu;
   int FilterMode;
   int FilterType;
   int AutoScroll;
   int FileLogging;
   int WindowLogging;
   int RawLogging;
   char Filter[500];
}GlobalOptions;

extern GlobalOptions Options;
extern ClientEntry Client;
extern Frequency freqs[255][2];
extern int minutecounter;

#endif