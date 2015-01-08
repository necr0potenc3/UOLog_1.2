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
    This project was stored on Folko's CVS server as $Source: /home/cvs/UOLogC/packets.c,v $.
   
    Id tag follows (<filename> <revision> <last changed> <author> <status>):
    $Id: packets.c,v 1.5 2003/08/18 19:09:11 folko Exp $
   
    If you want to report any errors, please include this tag.
    </GENERATED>
*/
#include "UOLog.h"
#include "HexLex.h"

int CheckFilter(unsigned char *Packet, int len)
{
   char curpac[3];
   sprintf(curpac, "%.2X", Packet[0]);
   switch(Options.FilterType)
   {
      /* Note to self: This *is* correct! Don't change it again and release a buggy version! */
      case FILTER_TYPE_NONE: return 1; break;
      case FILTER_TYPE_ONLY: if(strstr(Options.Filter, curpac)) return 1; break;
      case FILTER_TYPE_NOT:  if(!strstr(Options.Filter, curpac)) return 1; break;
      case FILTER_HEXLEX: if(!HexLex(0, Packet, len, Options.Filter)) return 1; break;
   }
   return 0;
}

void OnPacket(HANDLE Process, DEBUG_EVENT *dbgev, CONTEXT *context, int type)
{
   int len=GetRegisterContent(context, (type==TYPE_SEND)?Client.RegLenSend:Client.RegLenRecv)&0xFFFF;
   unsigned char *rawpacket;
   void *pointer=(void*)(LRESULT)GetRegisterContent(context, (type==TYPE_SEND)?Client.RegBufSend:Client.RegBufRecv);
   DWORD read=0;
   int res, filter;

   if(!len || !pointer)
      return;

   rawpacket=(unsigned char*)malloc(sizeof(unsigned char)*len);
   check(rawpacket!=0);
   res=ReadProcessMemory(Process, pointer, rawpacket, len, &read);
   if(!res)
   {
      free(rawpacket);
      return;
   }
   check(read==len);

   len=HandlePacket(rawpacket, len, type);
   WriteProcessMemory(Process, pointer, rawpacket, len, &read);

   freqs[rawpacket[0]][type].bytecount+=len;
   freqs[rawpacket[0]][type].count++;
   filter=(type==TYPE_RECV)?FILTER_MODE_OUTGOING:FILTER_MODE_INCOMING;
   if(Options.FilterMode!=filter && CheckFilter(rawpacket, len))
      DumpPacket(rawpacket, len, type);

   free(rawpacket);
   return;
}

/* HandlePacket - Handle packets!
 * This function is called everytime UOLog gets a packet. You can alter the packets here
 * if you want.
 */
int HandlePacket(unsigned char *packet, int len, int type)
{
   switch(packet[0])
   {
      case 0x73:
         {
            if(type==TYPE_SEND) /* sent by the client each minute */
               minutecounter++;
         }break;
   }
   return len;
}

/* I'm not using an array of strings because I want to add additional info for
 * each packet later
 */
char *GetPacketName(unsigned char *packet, int len)
{
   switch(packet[0])
   {
      case 0x00: return "CreateCharacter";
      case 0x01: return "ReturnToMainMenu";
      case 0x02: return "RequestMove";
      case 0x03: return "SpeakASCII";
      case 0x04: return "ToggleGodMode";
      case 0x05: return "Attack";
      case 0x06: return "DoubleClick";
      case 0x07: return "DragItem";
      case 0x08: return "DropItem";
      case 0x09: return "SingleClick";
      case 0x0A: return "Edit";
      case 0x0B: return "EditArea";
      case 0x0C: return "EditTile";
      case 0x0D: return "EditNPC";
      case 0x0E: return "EditTemplate";

      case 0x10: return "EditHue";
      case 0x11: return "SendStats";
      case 0x12: return "Macro/Cast";
      case 0x13: return "EquipObject";

      case 0x15: return "Follow";
      case 0x16: return "RequestScriptNames";
      case 0x17: return "EditScript";
      case 0x18: return "UploadScript";
      case 0x19: return "EditNPCConvo";
      case 0x1A: return "SendItem";
      case 0x1B: return "InitPlayer";
      case 0x1C: return "SendASCII";
      case 0x1D: return "RemoveObject";

      case 0x1F: return "SendExplosion";
      case 0x20: return "SendChar";
      case 0x21: return "RejectMoveRequest";
      case 0x22: return "AcceptMoveRequest";
      case 0x23: return "SendMoveItem";
      case 0x24: return "SendContainer";
      case 0x25: return "AddSingleItemToContainer";

      case 0x27: return "DenyDragObject"; /* not sure, has to be verified */
      case 0x28: return "DenyDropObject"; /* not sure, has to be verified */
      case 0x29: return "AcceptDropObject"; /* not sure, has to be verified */

      case 0x2B: return "SendGodModeReply";
      case 0x2C: return "Death";

      case 0x2E: return "SendEquipItem";
      case 0x2F: return "Swing";
      case 0x30: return "SendAttackOK";
      case 0x31: return "SendAttackEnd";

      case 0x33: return "EnDisableRedraw";
      case 0x34: return "RequestStatsSkills";

      case 0x3A: return "Skills";
      case 0x3B: return "AcceptTrade";
      case 0x3C: return "AddMultipleObjectsToContainer";

      case 0x4E: return "ChangeLight";
      case 0x4F: return "ChangePersonalLightLevel";
      case 0x50: return "SendBoardHeader";
      case 0x51: return "SendBoardMessage";
      case 0x52: return "PostBoardMessage";
      case 0x53: return "SendMessageBox";
      case 0x54: return "SendSound";
      case 0x55: return "LoginComplete";
      case 0x56: return "PlotCourse";

      case 0x58: return "NewRegion";
      case 0x59: return "NewRegion";

      case 0x5B: return "SendGameTime";

      case 0x5D: return "SelectCharacter";
      case 0x5E: return "ListServers";
      case 0x5F: return "AddServer";
      case 0x60: return "DelServer";

      case 0x65: return "Weather";
      case 0x66: return "Book";

      case 0x6C: return "Target";
      case 0x6D: return "PlayMidi";
      case 0x6E: return "SendAnim";
      case 0x6F: return "SendTrade";
      case 0x70: return "PlayMovingEffect";
      case 0x71:
         {
            if(!len)
               return "BB";
            switch(packet[3])
            {
               case 0: return "BB::DrawBoard";
               case 1: return "BB::ShowPosts";
               case 2:
               case 3: return "BB::Message";
               case 4: return "BB::Summary";
               case 5: return "BB::Post";
               case 6: return "BB::DelMessage";
               default: return "BB::Unknown";
            }
         }break;
      case 0x72: return "ToggleWarmode";
      case 0x73: return "PingPong";
      case 0x74: return "SendShop";
      case 0x75: return "Rename";

      case 0x77: return "SendCharWalk";
      case 0x78: return "SendAddCharToScene";
      case 0x79: return "PickObj";

      case 0x7C: return "SendPickObj";
      case 0x7D: return "PickObj";

      case 0x80: return "AuthRequest";
      case 0x81: return "AuthOK";
      case 0x82: return "AuthFailed";
      case 0x83: return "DeleteChar";

      case 0x86: return "ResendChars";

      case 0x88: return "Paperdoll";
      case 0x89: return "Corpse";

      case 0x8C: return "Relay";

      case 0x90: return "ShowMap";
      case 0x91: return "RelayAuth";

      case 0x93: return "OpenBook";

      case 0x95: return "ColorPicker";

      case 0x99: return "MultiTarget";
      case 0x9A: return "RequestInput";
      case 0x9B: return "RequestHelp";

      case 0x9E: return "SendShop";
      case 0x9F: return "SendShop";
      case 0xA0: return "SelectShard";
      case 0xA1: return "SendHealth";
      case 0xA2: return "SendMana";
      case 0xA3: return "SendStam";
      case 0xA4: return "SendSysInfo";
      case 0xA5: return "LaunchWeb";
      case 0xA6: return "ToolTip";
      case 0xA7: return "ReqToolTip";
      case 0xA8: return "SendShards";
      case 0xA9: return "SendCharsAndCities";
      case 0xAA: return "SendCurrentTarget";
      case 0xAB: return "StringQuery";
      case 0xAC: return "StringResponse";
      case 0xAD: return "SpeakUnicode";
      case 0xAE: return "SendUnicode";
      case 0xAF: return "Death";

      case 0xB0: return "SendGump";
      case 0xB1: return "GumpReply";
      case 0xB8: return "Profile";
      case 0xB9: return "EnableClientFeature";
      case 0xBA: return "QuestPointer";
      case 0xBB: return "AcctID";
      case 0xBC: return "SetSeason";
      case 0xBD: return "SendClientVersion";
      case 0xBE: return "SendClientVersion";
      case 0xBF:
         {
            if(len < 5)
               return "NewCommand";
            switch(packet[4])
            {
               case 0:  return  "NewCommand::FastWalk";
               case 2:  return  "NewCommand::AddWalkKey";
               case 4:  return  "NewCommand::CloseGump";
               case 6:  return  "NewCommand::Party";
               case 8:  return  "NewCommand::SetCursorHue";
               case 11: return  "NewCommand::SetLanguage";
               case 12: return  "NewCommand::CloseStatusGump";
               case 18: return  "NewCommand::MapDiff";
               case 19: return  "NewCommand::RequestPopup";
               default: return  "NewCommand::Unknown";
            }
         }break;

     case 0xC1: return "DisplayCliloc";

     case 0xC7: return "ParticleEffect";
     case 0xC8: return "UpdateRangeChange";
     case 0xC9: return "GetAreaServerPing";
     case 0xCA: return "GetUserServerPing";
     case 0xCB: return "SendGQCount";
     case 0xCC: return "LocalizedMessageAffix";
     case 0xD0: return "TransmitConfigFile";
     case 0xD1: return "SyncLogoutStatus";
     case 0xD2: return "SendCharNew";
     case 0xD3: return "SendAddCharNew";
     case 0xD4: return "SendNewBook";
     case 0xD6: return "AOSToolTip";
     case 0xD7: return "AOSStuff";
     case 0xD8: return "SendCustomHouse";

     default: return "Unknown";
   };
}

