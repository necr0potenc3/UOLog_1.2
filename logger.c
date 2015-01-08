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
    This project was stored on Folko's CVS server as $Source: /home/cvs/UOLogC/logger.c,v $.
   
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: logger.c,v 1.4 2003/08/27 19:53:29 folko Exp $
   
    If you want to report any errors, please include this tag.
    </GENERATED>
*/

#include "UOLog.h"

Frequency freqs[255][2];
int minutecounter;
FILE *logfile;

char *InitLogger(void)
{
   char timebuf[MAX_PATH];
   time_t tmptime;
   struct tm *eventtime;

   tmptime=time(NULL);
   eventtime=localtime(&tmptime);

   strftime(timebuf, MAX_PATH, "UOLog_%B %d-%H_%M_%S.log", eventtime );
   memset(&freqs, 0, sizeof(freqs)); 
   minutecounter=0;
   logfile=fopen(timebuf, "wb");
   if(!logfile)
      return "Couldn't create log..";
   AppendLogWindow(Options.PacketLog, TimeStamp("Created packetlog %s\r\n", timebuf));
   fprintf(logfile, "This log was recorded with Folko's UOLog.\r\n");
   fprintf(logfile, "Using client %s.\r\n\r\n", Client.Version);
   fflush(logfile);
   return 0;
}

/*
 * This is a debugging function. It doesn't get into a release.
 */
void NumOut(char *format, ...)
{
   va_list args;
   char final[4096];
   
   memset(final, 0, sizeof(final));
   va_start(args, format);
   vsprintf(final, format, args);
   va_end(args);
   MessageBox(0, final, "NumOut:", 0);
}

char *TimeStamp(char *format, ...)
{
   va_list args;
   char temp[MAX_TIME];
   static char timebuf[MAX_TIME];
   time_t tmptime=time(NULL);
   struct tm *eventtime=localtime(&tmptime);

   strftime(timebuf, sizeof(timebuf), "%H:%M:%S: ", eventtime );

   va_start(args, format);
   vsprintf(temp, format, args);
   va_end(args);
   strcat(timebuf, temp);
   return timebuf;
}

char *FormatLogMessage(unsigned char *rawpacket, unsigned int len, int type)
{
   unsigned int i=0, j=0;
   unsigned long pointer=0;
   char timebuf[10], *entry;
   time_t tmptime=time(NULL);
   struct tm *eventtime=localtime(&tmptime);
   SYSTEMTIME djg;
   unsigned long tv;

   strftime(timebuf, 30, "%H:%M:%S", eventtime );
   GetLocalTime(&djg);
   entry=(char *)malloc(sizeof(char)*len*11+255);
   check(entry!=0);
   tv = djg.wMilliseconds;

   if(type==TYPE_RECV)
      pointer+=sprintf(entry, "%s.%d Server -> Client: 0x%.2X (%s), frequ: %d, len: 0x%.2X\r\n", timebuf, tv, rawpacket[0], GetPacketName(rawpacket, len), freqs[rawpacket[0]][TYPE_SEND].count+freqs[rawpacket[0]][TYPE_RECV].count, len);
   else
      pointer+=sprintf(entry, "%s.%d Client -> Server: 0x%.2X (%s), frequ: %d, len: 0x%.2X\r\n", timebuf, tv, rawpacket[0], GetPacketName(rawpacket, len), freqs[rawpacket[0]][TYPE_SEND].count+freqs[rawpacket[0]][TYPE_RECV].count, len);

   for(i=0; i<len; i++)
   {
      if(i%16==0)
      {
         if(i && !Options.RawLogging)
         {
            pointer+=sprintf(entry+pointer, "->");
            for(j=i-16; j<i; j++)
            {
               if(isprint(rawpacket[j]))
                  pointer+=sprintf(entry+pointer, "%c", rawpacket[j]);
               else
                  pointer+=sprintf(entry+pointer, ".");
            }
            pointer+=sprintf(entry+pointer, "\r\n");
         }
         if(!Options.RawLogging)
            pointer+=sprintf(entry+pointer, "%.4X: ", i);
      }
      pointer+=sprintf(entry+pointer, "%.2X", rawpacket[i]);
      if(!Options.RawLogging)
         pointer+=sprintf(entry+pointer, " ");
   }

   if(i%16 && !Options.RawLogging)
   {
      for(j=i%16; j<16; j++)
         pointer+=sprintf(entry+pointer, "   ");
   }
   if(!Options.RawLogging)
      pointer+=sprintf(entry+pointer, "->");

   for(j=((i%16)?i-(i%16):i-16); j<i; j++)
   {
      if(Options.RawLogging)
         break;
      if(isprint(rawpacket[j]))
      {
         pointer+=sprintf(entry+pointer, "%c", rawpacket[j]);
      }
      else
         pointer+=sprintf(entry+pointer, ".");
   }
   pointer+=sprintf(entry+pointer, "\r\n\r\n");
   return entry;
}

void DumpPacket(unsigned char *rawpacket, int len, int type)
{
   char *entry=FormatLogMessage(rawpacket, len, type);
   if(Options.WindowLogging)
      AppendLogWindow(Options.PacketLog, entry);
   if(logfile)
   {
      unsigned long len=(unsigned long)strlen(entry);
      fwrite(entry, len, sizeof(char), logfile); /* fprintf=bad! */
      fflush(logfile);
   }
   free(entry);
}

void FinalizePacketLog(void)
{
   if(logfile) /* print packet statistics */
   {
      unsigned long sent=0, recvd=0;
      unsigned char i;
      fprintf(logfile, "Packet statistics\r\n\r\n");
      fprintf(logfile, "Client->Server:\r\n");
      for(i=0; i<255; i++)
      {
         if(freqs[i][TYPE_SEND].count && !freqs[i][TYPE_RECV].count)
         {
            fprintf(logfile, "%.2X: %d times, %lu bytes (~%lu bytes per packet) //%s\r\n",
               i, freqs[i][TYPE_SEND].count, freqs[i][TYPE_SEND].bytecount,
               freqs[i][TYPE_SEND].bytecount/freqs[i][TYPE_SEND].count,
               GetPacketName(&i, 0));
         }
         sent+=freqs[i][TYPE_SEND].bytecount;
      }

      fprintf(logfile, "\r\nServer->Client:\r\n");
      for(i=0; i<255; i++)
      {
         if(freqs[i][TYPE_RECV].count && !freqs[i][TYPE_SEND].count)
         {
            fprintf(logfile, "%.2X: %d times, %lu bytes (~%lu bytes per packet) //%s\r\n",
               i, freqs[i][TYPE_RECV].count, freqs[i][TYPE_RECV].bytecount,
               freqs[i][TYPE_RECV].bytecount/freqs[i][TYPE_RECV].count,
               GetPacketName(&i, 0));
         }
         recvd+=freqs[i][TYPE_RECV].bytecount;
      }

      fprintf(logfile, "\r\nBidirectional packets:\r\n");
      for(i=0; i<255; i++)
      {
         if(freqs[i][TYPE_SEND].count && freqs[i][TYPE_RECV].count)
         {
            fprintf(logfile, "%.2X: %d times, %lu bytes (~%lu bytes per packet) //%s\r\n",
               i, freqs[i][TYPE_SEND].count+freqs[i][TYPE_RECV].count,
               freqs[i][TYPE_SEND].bytecount+freqs[i][TYPE_RECV].bytecount,
               (freqs[i][TYPE_SEND].bytecount+freqs[i][TYPE_RECV].bytecount)/(freqs[i][TYPE_SEND].count+freqs[i][TYPE_RECV].count),
               GetPacketName(&i, 0));
         }
      }
      fprintf(logfile, "End of statistics\r\n\r\n");
      fprintf(logfile, "Bytes sent: %lu.\r\n", sent);
      fprintf(logfile, "Bytes received: %lu.\r\n", recvd);
      fprintf(logfile, "Total %lu bytes.\r\n", sent+recvd);
      if(minutecounter)
      {
         fprintf(logfile, "Client was active for %d minutes\r\n", minutecounter);
         fprintf(logfile, "That results in %.2f kb/s.", ( (sent+recvd)/1024.0 ) / (minutecounter * 60.0) );
      }
      fclose(logfile);
   }
}
