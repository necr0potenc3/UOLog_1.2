#ifndef _UOLOG_H_
#define _UOLOG_H_

#include <windows.h>
#include <process.h>
#include <richedit.h>
#include <dbghelp.h>

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <malloc.h>

#include "Logwindow.h"
#include "globals.h"
#include "resource.h"
#include "Debugger.h"
#include "Breakpoint.h"
#include "packets.h"
#include "logger.h"

#define FILTER_MODE_ALL 0
#define FILTER_MODE_INCOMING 1
#define FILTER_MODE_OUTGOING 2

#define FILTER_TYPE_NONE 0
#define FILTER_TYPE_ONLY 1
#define FILTER_TYPE_NOT 2
#define FILTER_HEXLEX 3

#define TYPE_SEND 0
#define TYPE_RECV 1

#define ASSERT_BUF_LEN 500
#define FILTER_BUF_LEN 250
#define MAX_LINE 500

#define check(exp) {if(!exp) _check(__FILE__, __LINE__, #exp); }
void _check(char *file, int line, char *exp);

typedef unsigned char BYTE;
typedef signed char SBYTE;
typedef unsigned short WORD;
typedef signed short SWORD;
typedef unsigned long DWORD;
typedef signed long SDWORD;

#endif /* _UOLOG_H_ */
