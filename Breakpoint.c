/**************************************************************************************\
*                                                                                      *
*  This file is part of UOLog,                                                         *
*  An advanced packet monitoring and logging tool for Ultima Online (UO).              *
*                                                                                      *
*  Copyright (C) 2003 by Folke Will (Folko)                                            *
*                                                                                      *
*  This program is free software; you can redistribute it and/or modify                *
*  it under the terms of the GNU General Public License as published by                *
*  the Free Software Foundation; either version 2 of the License, or                   *
*  (at your option) any later version.                                                 *
*                                                                                      *
*  This program is distributed in the hope that it will be useful,                     *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of                      *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
*  GNU General Public License for more details.                                        *
*                                                                                      *
*  You can contact the author by sending E-Mail to Folko <Folko@elitecoder.net>        *
*                                                                                      *
\**************************************************************************************/

/*
    <CVS SERVER GENERATED>
    This project was stored on Folko's CVS server as $Source: /home/cvs/UOLogC/Breakpoint.c,v $.
   
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Breakpoint.c,v 1.3 2003/08/04 02:09:58 folko23 Exp $
   
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#include "UOLog.h"

#define BPX_OPCODE 0xCC

/* SetBreakpoint - Set a breakpoint
 * SetBreakpoint sets a breakpoint in the target process by overwriting the original
 * instruction with "int3", the CPU instruction for a software breakpoint.
 * It saves the original byte, it has to be re-written before the execution resumes.
 */
void SetBreakpoint(Breakpoint *BPX)
{
   BYTE bpx_opcode=BPX_OPCODE;
   BYTE new_val=0;

   ReadProcessMemory(BPX->TargetProcess, BPX->Address, &new_val, sizeof(new_val), 0);
   if(new_val != BPX_OPCODE)
      BPX->OldValue = new_val;
   WriteProcessMemory(BPX->TargetProcess, BPX->Address, &bpx_opcode, sizeof(bpx_opcode), 0);
   FlushInstructionCache(BPX->TargetProcess, BPX->Address, sizeof(bpx_opcode));
   return;
}

/* BreakpointAction - Do work!
 * BreakpointAction sets the process into SINGLE_STEP mode so you can easily know when
 * the CPU finishes the instruction (Thanks to DarkStorm for this inspiration)
 * It then removes the Breakpoint and calls the callback, if present.
 */
void BreakpointAction(Breakpoint *BPX, HANDLE threadhandle)
{
   static CONTEXT context;
   memset(&context, 0, sizeof(context));

   context.ContextFlags=CONTEXT_FULL;
   GetThreadContext(threadhandle, &context);
   context.Eip=(DWORD)(LRESULT)BPX->Address;
   context.EFlags|= 0x0100; /* Single step */
   SetThreadContext(threadhandle, &context);

   RemoveBreakpoint(BPX);

   if(BPX->CallbackFunc)
      BPX->CallbackFunc(BPX->TargetProcess, BPX->dbgev, &context, BPX->type);
   return;
}

/* RemoveBreakpoint - Remove a breakpoint
 * RemoveBreakpoint removes a breakpoint from the target process.
 * It overwrites the instruction with the original byte.
 */
void RemoveBreakpoint(Breakpoint *BPX)
{
   WriteProcessMemory(BPX->TargetProcess, BPX->Address, &BPX->OldValue, sizeof(BPX->OldValue), 0);
   FlushInstructionCache(BPX->TargetProcess, BPX->Address, sizeof(BPX->OldValue));
   return;
}
