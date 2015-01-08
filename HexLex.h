#ifndef _HEXLEX_H_
#define _HEXLEX_H_

/* Types */
typedef unsigned char hex_t;
typedef char lex_t;
typedef signed short res_t;

/* Interface */
extern size_t HexLex(size_t start, const hex_t *source, size_t len, const lex_t *pattern);

/* Implementation */
static const lex_t *GetPattern(const lex_t *pattern, size_t num, res_t next_multi);
static res_t IsMulti(const lex_t *pattern);
static res_t LexMatch(hex_t byte, const lex_t *pattern);
static hex_t HexTrans(lex_t lex_byte);

#endif /* _HEXLEX_H_ */