/* HexLex 1.0 - A Tiny Hex Lex Engine
 *
 * Copyright (C) 2004 by Folke 'Folko' Will <folko@elitecoder.net> of x90
 *
 * HexLex is a function to search an expression inside a byte stream.
 * The HexLex function has four parameters, the starting address,
 * source pointer, the end address and the pattern to search for.
 *
 * If the pattern was found in the source, the position will be
 * returned.
 * If the pattern was not found, -1 will be returned.
 *
 * The pattern has to have one of the following formats:
 *
 * "01 02 03": matches {..., 0x01, 0x02, 0x03, ...}
 * "?1 0? 03": matches {..., 0x?1, 0x0?, 0x03, ...}
 * "01|02 03": matches {..., 0x01, 0x03, ...} and {..., 0x02, 0x03, ...}
 *
 * Do not put spaces before or after the |, as HexLex uses spaces as byte
 * seperators.
 *
 * Complex example: "1? 02|03|?4|05 06 ?? 07|F?"
 *
 */

#include <string.h>
#include "HexLex.h"

/*
 * HexLex: Find pattern in byte stream
 */
extern size_t HexLex(size_t start, const hex_t *source, size_t len, const lex_t *pattern)
{
   size_t s_pos, p_pos;

   p_pos = 0;

   for(s_pos = start; s_pos < len; s_pos++) {
      const lex_t *cur_pat = GetPattern(pattern, p_pos, 0);

      if(!cur_pat) {
         return(-1);
      }
      if(LexMatch(source[s_pos], cur_pat)) { /* Matched */
         p_pos++;
      } else {                               /* Not matched */
         if(p_pos) {
            s_pos--; /* Evaluate this byte again */
         }
         p_pos = 0;
      }

      if(!GetPattern(pattern, p_pos, 0)) {
         return(s_pos + 1 - p_pos);
      }
   }
   return(-1);
}

/*
 * IsMulti: Is the expression a multi expression?
 */
static res_t IsMulti(const lex_t *pattern)
{
   size_t pos;

   for(pos = 0; pos < strlen(pattern); pos++) {
      if(pattern[pos] == ' ') {
         return(0);
      }
      if(pattern[pos] == '|') {
         return(1);
      }
   }
   return(0);
}

/*
 * GetPattern: Returns the nth expression of a pattern
 */
static const lex_t *GetPattern(const lex_t *pattern, size_t num, res_t next_multi)
{
   size_t pos, pat_pos;

   pat_pos = 0;
   for(pos = 0; pos < strlen(pattern) + 1; pos++) {
      if(pattern[pos] == ' ' || !pattern[pos]) {
         pat_pos++;
         continue;
      }
      if(pattern[pos] == '|' && next_multi) {
         pat_pos++;
         continue;
      }
      if(pat_pos == num && strlen(pattern + pos) >= 2) {
         return(pattern + pos);
      }
   }

   return(0);
}

/*
 * LexMatch: Matches byte of the source stream vs the current expression
 */
static res_t LexMatch(hex_t byte, const lex_t *pattern)
{
   hex_t left, right;

   left = (byte & 0xF0) >> 4;
   right = byte & 0x0F;

   if(!pattern) {
      return(0);
   }

   if(pattern[0] != '?' && left != HexTrans(pattern[0])) {
      if(IsMulti(pattern)) {
         return(LexMatch(byte, GetPattern(pattern, 1, 1)));
      }
      return(0);
   }
   if(pattern[1] != '?' && right != HexTrans(pattern[1])) {
      if(IsMulti(pattern)) {
         return(LexMatch(byte, GetPattern(pattern, 1, 1)));
      }
      return(0);
   }
   return(1);
}

/*
 * HexTrans: Returns the hex value of the ASCII lex value
 */
static hex_t HexTrans(lex_t lex_byte)
{
   if(lex_byte >= '0' && lex_byte <= '9') {
      return(lex_byte - '0');
   }
   if(lex_byte >= 'A' && lex_byte <= 'F') {
      return(lex_byte - 'A' + 10);
   }
   if(lex_byte >= 'a' && lex_byte <= 'f') {
      return(lex_byte - 'a' + 10);
   }
   return(0);
}
