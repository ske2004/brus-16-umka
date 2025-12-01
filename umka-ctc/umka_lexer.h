#ifndef _CTC_UMKA_LEXER_H_
#define _CTC_UMKA_LEXER_H_

#include "umka_error.h"
#include <stdbool.h>

typedef enum {
  TT_INVALID,

  TT_NUMLIT,
  TT_STRLIT,

  TT_IDENT,
  TT_KW_FN,
  TT_KW_FOR,
  TT_KW_IMPORT,

  TT_LBRACE,
  TT_RBRACE,
  TT_LBRACK,
  TT_RBRACK,
  TT_LPAREN,
  TT_RPAREN,
  
  TT_COLONCOLON,
  TT_COMMA,
} TokenType;

typedef struct {
  TokenType tt;
  Location begin, end;
} Token;

typedef struct {
  const char *source;
  Location loc;
} Lexer;

Lexer lexerInit(const char *source);
Token lexerNext(Lexer *lexer);
bool lexerIsEof(Lexer *lexer);
const char *lexerTokenTypeName(TokenType tt);
void lexerTokenValue(const Lexer *lexer, Token t, char *buf);
bool lexerTokenValueEq(const Lexer *lexer, Token t, const char *ref);

#endif