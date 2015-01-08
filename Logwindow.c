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
    This project was stored on Folko's CVS server as $Source: /home/cvs/UOLogC/Logwindow.c,v $.
   
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: Logwindow.c,v 1.2 2003/06/20 10:25:21 folko23 Exp $
   
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#include "UOLog.h"

void CreateLogWindow(LogWindow *wnd, int x, int y, int width, int height, char *Font)
{
   wnd->hWnd=CreateWindowEx(0, "RichEdit", "Welcome to Folko's UOLog!\n", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_NOHIDESEL, x, y, width, height, wnd->hParentWnd, NULL, GetModuleHandle(NULL), NULL);

   wnd->font=CreateFont(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Font);
   if(wnd->font)
      SendMessage(wnd->hWnd, WM_SETFONT, (WPARAM)wnd->font, 1);
}

void ResizeLogWindow(LogWindow *wnd, int x, int y, int width, int height)
{
   MoveWindow(wnd->hWnd, x, y, width, height, 1);
}

void ClearLogWindow(LogWindow *wnd)
{
   SetWindowText(wnd->hWnd, "");
}

void AppendLogWindow(LogWindow *wnd, char *Text)
{
   unsigned long index=(unsigned long)SendMessage(wnd->hWnd, WM_GETTEXTLENGTH, 0, 0);
   unsigned long oldstart, oldend;

   SendMessage(wnd->hWnd, EM_GETSEL, (WPARAM)&oldstart, (LPARAM)&oldend); /* Get old selection */
   SendMessage(wnd->hWnd, EM_SETSEL, (WPARAM)index, (LPARAM)index); /* Set selection to end */
   SendMessage(wnd->hWnd, EM_REPLACESEL, 0, (LPARAM) Text); /* This will paste the text at the end */
   SendMessage(wnd->hWnd, EM_SETSEL, oldstart, oldend); /* Restore old selection */

   if(Options.AutoScroll)
      SendMessage(wnd->hWnd, WM_VSCROLL, SB_BOTTOM, 0); /* Arog: Added autoscrolling */
}

void RemoveLogWindow(LogWindow *wnd)
{
   if(wnd->font)
      DeleteObject(wnd->font);
   if(wnd->hWnd)
      DestroyWindow(wnd->hWnd);
}
