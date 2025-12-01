#ifndef _CTC_UMKA_PARSER_H_
#define _CTC_UMKA_PARSER_H_

#include "umka_lexer.h"

typedef int AstID;

typedef enum {
  ASTT_PROGRAM,
  ASTT_FN,
  ASTT_SIGNATURE,
  ASTT_CALL,
  ASTT_IDENT,
  ASTT_BLOCK,
  ASTT_FOR,
  ASTT_NUMBER,
  ASTT_PARAMS,
} AstType;

typedef struct {
  AstType astt;
  Token token;
  Location begin, end;
  AstID child, sibling;
} Ast;

typedef struct {
  Ast *nodes;
  size_t limNodes;
  size_t sizeNodes;
} AstBuf;

typedef struct {
  Token token;
  Lexer lexer;
  AstBuf *buf;
} Parser;

AstBuf astbufNew(int lim);
void astbufDel(AstBuf *buf);
Ast *astbufGet(const AstBuf *buf, AstID id);

Parser parserInit(const char *source, AstBuf *buf);
AstID parserParse(Parser *parser);

const char *parserAstTypeName(AstType astt);

#endif
