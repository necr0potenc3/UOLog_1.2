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
    This project was stored on Folko's CVS server as $Source: /home/cvs/UOLogC/UOLog.c,v $.

    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: UOLog.c,v 1.3 2003/08/27 19:53:29 folko Exp $

    If you want to report any errors, please include this tag.
    </GENERATED>
*/

/* Includes */
#include "UOLog.h"

/* Prototypes */
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
int CALLBACK OptionsDlgProc(HWND hDlg, UINT uMsg, UINT wParam, LONG lParam);
char *AttachToClient(DebugLoopParam *param);
int LoadClient(DebugLoopParam *param, HWND hWnd);
void LoadOptions(void);
void SaveOptions(void);
long ExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo);

/* Global variables */
GlobalOptions Options;
ClientEntry Client;

char *WriteMiniDump(char *name, MINIDUMP_TYPE type, EXCEPTION_POINTERS *ExceptionInfo)
{
   typedef BOOL (WINAPI *DumpFunc) (HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);
   HANDLE dmpfile;
   MINIDUMP_EXCEPTION_INFORMATION info;
   HMODULE dbghelp;
   DumpFunc WriteDump=0;

   dbghelp=LoadLibrary("dbghelp");
   if(dbghelp)
      WriteDump=(DumpFunc) GetProcAddress(dbghelp, "MiniDumpWriteDump");

   if(!dbghelp || !WriteDump)
      return "You don't have the latest DbgHelp.dll installed! Get it from microsoft.com, it's\r"
             "useful for other applications, too.";

   info.ThreadId=GetCurrentThreadId();
   info.ExceptionPointers=ExceptionInfo;
   info.ClientPointers=1;

   dmpfile=CreateFile(name, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, 0, NULL);
   if(dmpfile==INVALID_HANDLE_VALUE)
      return "Couldn't create the DumpFile.";

   if(!WriteDump(GetCurrentProcess(), GetCurrentProcessId(), dmpfile, type,
      (ExceptionInfo) ? (&info) : (NULL), NULL, NULL))
      return "Couldn't write the dump infos to the file.";

   CloseHandle(dmpfile);

   return 0;
}

long ExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
   char *res=0;

   MessageBox(NULL, "Well, UOLog crashed. UOLog will now attempt to create MiniDumps so\r"
                    "I know why it crashed. Two minidumps will be created, small.dmp and\r"
                    "big.dmp. Please mail me small.dmp or big.dmp if you can compress it\r"
                    "to <3mb.",
                    "Ugh!", MB_ICONERROR);
   if(res=WriteMiniDump("small.dmp", MiniDumpWithDataSegs, ExceptionInfo))
   {
      MessageBox(NULL, res, "Error", MB_ICONINFORMATION);
      return EXCEPTION_EXECUTE_HANDLER;
   }
   if(res=WriteMiniDump("big.dmp", MiniDumpWithFullMemory, ExceptionInfo))
   {
      MessageBox(NULL, res, "Error", MB_ICONINFORMATION);
      return EXCEPTION_EXECUTE_HANDLER;
   }
   MessageBox(NULL, "Both dumps written successfully, quitting.", "Phew", MB_ICONINFORMATION);
   return EXCEPTION_EXECUTE_HANDLER;
}

/* This function is called when an assertion occurs.
 * File: String which contains the name of the file in which the assertion occured.
 * line: Line in which the assertion occured.
 * exp: The assertion as string, e.g. "handle!=INVALID_HANDLE_VALUE"
 */
void _check(char *file, int line, char *exp)
{
   char error[ASSERT_BUF_LEN];

   sprintf(error, "Assertion failed!\rIn %s, line %d, exp: '%s' failed.\rPlease report this message.\rDepending on the severity of the error, UOLog may be unusable until you restart it.", file, line, exp);
   MessageBox(Options.hWnd?Options.hWnd:NULL, error, "UOLog", MB_ICONERROR);
   if(!WriteMiniDump("assertion.dmp", MiniDumpWithDataSegs, NULL))
      MessageBox(NULL, "An assertion dump has been written. Please send it to <folko@elitecoder.net>. Thanks!", "Info", MB_ICONINFORMATION);
}

/* Program entry point */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX wc;
   RECT deskreg;
   HWND hWnd;
   MSG msg;

   SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionFilter);
   memset(&wc, 0, sizeof(WNDCLASSEX));
   wc.cbSize = sizeof(WNDCLASSEX);
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = WndProc;
   wc.hInstance = hInstance;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
   wc.lpszClassName = "UOLogWndClass";
   wc.lpszMenuName=MAKEINTRESOURCE(ID_MAINMENU);
   wc.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
   RegisterClassEx(&wc);

   GetWindowRect(GetDesktopWindow(), &deskreg);

   hWnd=CreateWindowEx(0, "UOLogWndClass", "Folko's UOLog, version 1.2", WS_VISIBLE | WS_OVERLAPPEDWINDOW, deskreg.right/2-900/2, deskreg.bottom/2-450/2, 900, 450, NULL, NULL, hInstance, NULL);

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   while (GetMessage(&msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return 0;
}


/* Message loop for main window */
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
   static HMENU MainMenu;
   static HINSTANCE hInstance;
   static DebugLoopParam debugparms; /* Parameters for the debugger. */
   static LogWindow logwnd;
   static HMODULE riched;
   switch(uMsg)
   {
      case WM_CREATE:
         {
            if(!(riched=LoadLibrary("RICHED32")) ) /* We have to load richedit in order for the logwindow to work */
            {
               MessageBox(hWnd, "Couldn't load RichEdit control. Download richedit somewhere..", "Error", MB_ICONERROR);
               PostQuitMessage(1);
            }
            hInstance=GetModuleHandle(NULL);
            MainMenu=GetMenu(hWnd);
            logwnd.hParentWnd=hWnd;
            CreateLogWindow(&logwnd, 0, 0, 900, 400, "Courier");
            Options.PacketLog=&logwnd;
            check(Options.PacketLog!=0);

            EnableMenuItem(MainMenu, ID_CLIENT_DETACH, MF_GRAYED);

            Options.hWnd=hWnd;
            Options.MainMenu=MainMenu;
            LoadOptions();
            CheckMenuItem(MainMenu, ID_OPTIONS_AUTOSCROLL, Options.AutoScroll?MF_CHECKED:MF_UNCHECKED);
            CheckMenuItem(MainMenu, ID_LOGGING_LOGFILE, Options.FileLogging?MF_CHECKED:MF_UNCHECKED);
            CheckMenuItem(MainMenu, ID_LOGGING_PACKETWINDOW, Options.WindowLogging?MF_CHECKED:MF_UNCHECKED);
         }break;
      case WM_SIZE:
         {
            RECT newrect; GetWindowRect(hWnd, &newrect);
            ResizeLogWindow(Options.PacketLog, 0, 0, LOWORD(lParam), HIWORD(lParam));
         }break;
      case WM_COMMAND:
         {
            if(HIWORD(wParam)==BN_CLICKED)
            {
               switch(LOWORD(wParam))
               {
                  case ID_FILE_QUIT:
                     {
                        SendMessage(hWnd, WM_CLOSE, 0, 0);
                     }break;
                  case ID_CLIENT_ATTACH:
                     {
                        char *result=AttachToClient(&debugparms);
                        if(!result)
                        {
                           EnableMenuItem(MainMenu, ID_CLIENT_DETACH, MF_ENABLED);
                           EnableMenuItem(MainMenu, ID_CLIENT_ATTACH, MF_GRAYED);
                           EnableMenuItem(MainMenu, ID_CLIENT_LOAD, MF_GRAYED);
                           EnableMenuItem(MainMenu, ID_LOGGING_LOGFILE, MF_GRAYED);
                           debugparms.attached=1;
                           _beginthread(DebugLoop, 0, &debugparms);
                        }
                        else
                           MessageBox(hWnd, result, "Fatal error!", MB_ICONERROR);
                     }break;
                  case ID_CLIENT_DETACH:
                     {
                        HINSTANCE kernel=LoadLibrary("KERNEL32");
                        int found=GetProcAddress(kernel, "DebugActiveProcessStop")!=0;
                        FreeLibrary(kernel);
                        if(!found)
                        {
                           MessageBox(hWnd, "Sorry, detaching works with Windows XP only.", "Error", MB_ICONERROR);
                           break;
                        }
                        Detach();
                     }break;
                  case ID_CLIENT_LOAD:
                     {
                        if(LoadClient(&debugparms, hWnd))
                        {
                           EnableMenuItem(MainMenu, ID_CLIENT_DETACH, MF_ENABLED);
                           EnableMenuItem(MainMenu, ID_CLIENT_ATTACH, MF_GRAYED);
                           EnableMenuItem(MainMenu, ID_CLIENT_LOAD, MF_GRAYED);
                           EnableMenuItem(MainMenu, ID_LOGGING_LOGFILE, MF_GRAYED);
                           debugparms.attached=0;
                           _beginthread(DebugLoop, 0, &debugparms);
                        }
                     }break;
                  case ID_OPTIONS_FILTER:
                     {
                        DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hWnd, (DLGPROC)OptionsDlgProc, 0);
                     }break;
                  case ID_OPTIONS_SAVEOPTIONS:
                     {
                        SaveOptions();
                     }break;
                  case ID_HELP_ABOUT:
                     {
                        char Info[]=
                           "UOLog is (C) Folko <folko@elitecoder.net>, 2003 - 2004\r"
                           "Please only use this tool if you are an administator\r"
                           "or have the permission to sniff packets.\r"
                           "If you have any suggestions or find any bugs, please mail me.\r"
                           "\rCoding language: 100% Pure C!\r"
                           "\rKnown bugs:\r"
                           "AutoScrolling is a bit bugged, see changes.txt for details.";
                        MessageBox(hWnd, Info, "About...", MB_ICONINFORMATION);
                     }break;
                  case ID_OPTIONS_CLEAR:
                     {
                        ClearLogWindow(Options.PacketLog);
                     }break;
                  case ID_OPTIONS_TOP:
                     {
                        static int on=0;
                        on=!on;
                        CheckMenuItem(MainMenu, ID_OPTIONS_TOP, on?MF_CHECKED:MF_UNCHECKED);
                        if(on)
                           SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        else
                           SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                     }break;
                  case ID_OPTIONS_AUTOSCROLL:
                     {
                        Options.AutoScroll=!Options.AutoScroll;
                        CheckMenuItem(MainMenu, ID_OPTIONS_AUTOSCROLL, Options.AutoScroll?MF_CHECKED:MF_UNCHECKED);
                     }break;
                  case ID_LOGGING_RAW:
                     {
                        Options.RawLogging=!Options.RawLogging;
                        CheckMenuItem(MainMenu, ID_LOGGING_RAW, Options.RawLogging?MF_CHECKED:MF_UNCHECKED);
                     }break;
                  case ID_LOGGING_PACKETWINDOW:
                     {
                        Options.WindowLogging=!Options.WindowLogging;
                        CheckMenuItem(MainMenu, ID_LOGGING_PACKETWINDOW, Options.WindowLogging?MF_CHECKED:MF_UNCHECKED);
                     }break;
                  case ID_LOGGING_LOGFILE:
                     {
                        Options.FileLogging=!Options.FileLogging;
                        CheckMenuItem(MainMenu, ID_LOGGING_LOGFILE, Options.FileLogging?MF_CHECKED:MF_UNCHECKED);
                     }break;
               }
            }
         }break;
      case WM_CLOSE:
         {
            int sure=1;
            if(GetMenuState(MainMenu, ID_CLIENT_ATTACH, 0)==MF_GRAYED)
            {
               if(MessageBox(hWnd, "If you close UOLog now, the client will also close. Continue?", "Warning", MB_ICONWARNING | MB_YESNO)==IDNO)
                  sure=0;
            }
            if(sure)
            {
               FinalizePacketLog();
               RemoveLogWindow(Options.PacketLog);
               if(riched)
                 FreeLibrary(riched);
               PostQuitMessage(0);
            }
         }break;
      case WM_DESTROY:
         {
            PostQuitMessage(0);
         }break;
      default:
         return DefWindowProc(hWnd, uMsg, wParam, lParam);
   }
   return 0;
}


/* LoadClient
 * Displays a file selection dialog and fills the parameters depending on the selection.
 * param: Parameters to be filled.
 * hWnd: Caller's hWnd.
 * Returns 1 on success, 0 on failure.
 */
int LoadClient(DebugLoopParam *param, HWND hWnd)
{
   OPENFILENAME ofn;
   static char clientpath[MAX_PATH];
   char oldpath[MAX_PATH];

   memset(clientpath, 0, sizeof(clientpath));
   memset(&ofn, 0, sizeof(ofn));
   GetCurrentDirectory(MAX_PATH, oldpath);
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = hWnd;
   ofn.lpstrFile = clientpath;
   ofn.nMaxFile = sizeof(clientpath);
   ofn.lpstrFilter = "Exe\0*.Exe\0All\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
   GetOpenFileName(&ofn);
   SetCurrentDirectory(oldpath);
   if(!strlen(clientpath))
      return 0;
   param->path=clientpath;
   return 1;
}

/* AttachToClient
 * Attaches UOLog to the running client.
 * param: Parameters to be filled
 * Returns an errorstring on failure, 0 on success
 */
char *AttachToClient(DebugLoopParam *param)
{
   HWND TargetWindow;
   DWORD TargetPID;
   HINSTANCE kernel=LoadLibrary("KERNEL32");
   int found=GetProcAddress(kernel, "OpenThread")!=0;
   FreeLibrary(kernel);
   if(!found)
      return "Sorry, attaching works with Windows ME, 2000 and XP only.";
   TargetWindow=FindWindow("Ultima Online", 0);
   if(!TargetWindow) return "Couldn't find the UO window";
   GetWindowThreadProcessId(TargetWindow, &TargetPID);
   param->TargetPID=TargetPID;
   check(TargetPID!=0);
   return 0;
}

/* MessageLoop for the options dialog. */
int CALLBACK OptionsDlgProc(HWND hDlg, UINT uMsg, UINT wParam, LONG lParam)
{
   switch(uMsg)
   {
      case WM_INITDIALOG:
         {
            switch(Options.FilterMode)
            {
               case FILTER_MODE_ALL: CheckRadioButton(hDlg, IDC_ALL, IDC_OUTGOING, IDC_ALL); break;
               case FILTER_MODE_INCOMING: CheckRadioButton(hDlg, IDC_ALL, IDC_OUTGOING, IDC_INCOMING); break;
               case FILTER_MODE_OUTGOING: CheckRadioButton(hDlg, IDC_ALL, IDC_OUTGOING, IDC_OUTGOING); break;
            }
            switch(Options.FilterType)
            {
               case FILTER_TYPE_NONE: break;
               case FILTER_TYPE_ONLY: SetDlgItemText(hDlg, IDC_ONLY, Options.Filter); break;
               case FILTER_TYPE_NOT: SetDlgItemText(hDlg, IDC_NOT, Options.Filter); break;
               case FILTER_HEXLEX: SetDlgItemText(hDlg, IDC_HEXLEX, Options.Filter); break;
            }
         }break;
      case WM_COMMAND:
         {
            switch(LOWORD(wParam))
            {
               case IDOK:
                  {
                     char only[FILTER_BUF_LEN], not[FILTER_BUF_LEN], lex[FILTER_BUF_LEN];
                     GetDlgItemText(hDlg, IDC_ONLY, only, sizeof(only));
                     GetDlgItemText(hDlg, IDC_NOT, not, sizeof(not));
                     GetDlgItemText(hDlg, IDC_HEXLEX, lex, sizeof(lex));
                     if(strlen(only) && strlen(not))
                     {
                        MessageBox(hDlg, "Use either 'Only' or 'Not', not both!", "Error", MB_ICONEXCLAMATION);
                        break;
                     }
                     if(strlen(lex)) {
                        if(strstr(lex, "| ") || lex[strlen(lex) - 1] == '|' || lex[0] == '|') {
                           MessageBox(hDlg, "Invalid search string, no spaces before / after |", "Error", MB_ICONEXCLAMATION);
                           break;
                        }
                        strcpy(Options.Filter, lex);
                        Options.FilterType = FILTER_HEXLEX;
                     } else if (strlen(only)) {
                        int i;
                        for(i=0; i<(int)strlen(only); i++)
                           only[i]=toupper(only[i]);
                        strcpy(Options.Filter, only);
                        Options.FilterType=FILTER_TYPE_ONLY;
                     }else if(strlen(not)) {
                        int i;
                        for(i=0; i<(int)strlen(not); i++)
                           not[i]=toupper(not[i]);
                        strcpy(Options.Filter, not);
                        Options.FilterType=FILTER_TYPE_NOT;
                     }else{
                        strcpy(Options.Filter, "");
                        Options.FilterType=FILTER_TYPE_NONE;
                     }

                     if(IsDlgButtonChecked(hDlg, IDC_ALL))
                        Options.FilterMode=FILTER_MODE_ALL;
                     else if(IsDlgButtonChecked(hDlg, IDC_INCOMING))
                        Options.FilterMode=FILTER_MODE_INCOMING;
                     else if(IsDlgButtonChecked(hDlg, IDC_OUTGOING))
                        Options.FilterMode=FILTER_MODE_OUTGOING;
                     EndDialog(hDlg, 0);
                  }break;
            }
         }break;
      case WM_CLOSE:
         {
            EndDialog(hDlg, 0);
         }break;
   }
   return 0;
}

void LoadOptions(void)
{
   char line[MAX_LINE];
   char *found=0;
   FILE *cfg=0;

   Options.AutoScroll=1;
   Options.FilterMode=FILTER_MODE_ALL;
   Options.FilterType=FILTER_TYPE_NOT;
   Options.FileLogging=1;
   Options.WindowLogging=1;
   strcpy(Options.Filter, "02, 22 //Dont display walk stuff");

   cfg=fopen("UOLog.cfg", "r");
   if(!cfg) return;
   while(fgets(line, sizeof(line), cfg))
   {
      if(strstr(line, "AutoScrolling=")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.AutoScroll);
      }else if(strstr(line, "FilterMode")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.FilterMode);
      }else if(strstr(line, "FilterType")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.FilterType);
      }else if(strstr(line, "FileLogging")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.FileLogging);
      }else if(strstr(line, "WindowLogging")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.WindowLogging);
      }else if(strstr(line, "RawLogging")){
         found=strchr(line, '=')+1;
         sscanf(found, "%d", &Options.RawLogging);
      }else if(strstr(line, "Filter")){
         int i=0;
         memset(&Options.Filter, 0, sizeof(Options.Filter));
         found=strchr(line, '=')+1;
         while(found[i++]!='\n') ;
         strncpy(Options.Filter, found, i-1);
      }
   }
   fclose(cfg);
   return;
}

void SaveOptions(void)
{
   FILE *cfg=0;
   cfg=fopen("UOLog.cfg", "w");
   if(!cfg) return;
   fprintf(cfg, "AutoScrolling=%d\n", Options.AutoScroll);
   fprintf(cfg, "FilterMode=%d\n", Options.FilterMode);
   fprintf(cfg, "FilterType=%d\n", Options.FilterType);
   fprintf(cfg, "WindowLogging=%d\n", Options.WindowLogging);
   fprintf(cfg, "FileLogging=%d\n", Options.FileLogging);
   fprintf(cfg, "RawLogging=%d\n", Options.RawLogging);
   fprintf(cfg, "Filter=%s\n", Options.Filter);
   fclose(cfg);
}
