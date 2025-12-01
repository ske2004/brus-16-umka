#ifndef _CTC_UMKA_CODEGEN_BRUS16_H_
#define _CTC_UMKA_CODEGEN_BRUS16_H_

#include "umka_parser.h"
#include <stdint.h>

typedef struct {
  const AstBuf *astBuf;
  const Lexer *lexer;
  AstID root;

  uint16_t mainOffs;

  uint16_t codeBuf[8192];
  size_t codeSize;
  uint16_t dataBuf[8192];
  size_t dataSize;
} Codegen;

Codegen codegenInit(const AstBuf *astBuf, const Lexer *lexer, AstID root);
void codegenGenerate(Codegen *cg, uint16_t *out_rom);

#endif