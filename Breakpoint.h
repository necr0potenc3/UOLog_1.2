#ifndef _BREAKPOINT_H_
#define _BREAKPOINT_H_

typedef void DEBUG_FUNCTION(HANDLE, DEBUG_EVENT *, CONTEXT *, int);

typedef struct tagBreakpoint
{
   void *Address;
   unsigned char OldValue;
   HANDLE TargetProcess;
   DEBUG_FUNCTION *CallbackFunc;
   DEBUG_EVENT *dbgev;
   int type;
}Breakpoint;


void SetBreakpoint(Breakpoint *BPX);
void BreakpointAction(Breakpoint *BPX, HANDLE threadhandle);
void RemoveBreakpoint(Breakpoint *BPX);

#endif /* _BREAKPOINT_H_ */