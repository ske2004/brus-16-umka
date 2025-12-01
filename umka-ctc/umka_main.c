#include <stdio.h>
#include "umka_lexer.h"
#include "umka_parser.h"

int readFile(const char *path, char *fileBuf, int max)
{
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    fprintf(stderr, "can't open %s\n", path);
    return 1;
  }
  fread(fileBuf, 1, max, f);
  fclose(f);
  return 0;
}

void tellTokens(const char *fileBuf)
{
  Lexer lexer = lexerInit(fileBuf);
  while (!lexerIsEof(&lexer)) {
    Token t = lexerNext(&lexer);
    char v[64] = {0};
    lexerTokenValue(&lexer, t, v);
    printf("%12s %12s %d:%d:%d\n", lexerTokenTypeName(t.tt), v, t.begin.line+1, t.begin.col+1, t.begin.pos);
  }
}

void printAst(const AstBuf *buf, const Lexer *lexer, AstID id, int depth)
{
  Ast *node = astbufGet(buf, id);
  char chrBuf[64] = { 0 };
  lexerTokenValue(lexer, node->token, chrBuf);
  printf("%*s%d - %s (%s)\n", depth*2, "", id, chrBuf, parserAstTypeName(node->astt));

  if (node->child) {
    printAst(buf, lexer, node->child, depth+1);
  } 

  if (node->sibling) {
    printAst(buf, lexer, node->sibling, depth);
  }
}

int main(int argc, const char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s <dingus.um>\n", argv[0]);
    return 1;
  }

  char fileBuf[1<<16];
  if (readFile(argv[1], fileBuf, sizeof fileBuf)) { return 1; }

  tellTokens(fileBuf);

  AstBuf buf = astbufNew(1024);
  Parser p = parserInit(fileBuf, &buf);

  AstID root = parserParse(&p);

  printAst(&buf, &p.lexer, root, 0);

  astbufDel(&buf);

  printf("Ok\n");
}