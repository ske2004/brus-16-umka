#include "umka_parser.h"
#include "umka_lexer.h"
#include <assert.h>

#define CUR parser->token
#define ERR(msg) errorShow(parser->token.begin, (msg))

AstBuf astbufNew(int lim)
{
  return (AstBuf){
    .limNodes = lim,
    .nodes = calloc(sizeof(Ast), lim)
  };
}

void astbufDel(AstBuf *buf)
{
  free(buf->nodes);
  *buf = (AstBuf){ 0 };
}

Ast *astbufGet(const AstBuf *buf, AstID id)
{
  if (id == 0 || id > buf->sizeNodes) {
    return NULL;
  }

  return &buf->nodes[id-1];
}

static AstID astbufAllocNode(AstBuf *buf, AstID parent)
{
  assert(buf->sizeNodes < buf->limNodes);

  AstID nodeId = buf->sizeNodes++ + 1;

  if (parent) {
    Ast *node = astbufGet(buf, parent);
    if (!node->child) {
      node->child = nodeId;
    } else {
      node = astbufGet(buf, node->child);
      while (node->sibling) {
        node = astbufGet(buf, node->sibling);
      }
      node->sibling = nodeId;
    }
  }

  return nodeId;
}

static void parserNext(Parser *parser)
{
  parser->token = lexerNext(&parser->lexer);
}

static AstID parserPush(Parser *parser, AstID parent, AstType astt)
{
  AstID id = astbufAllocNode(parser->buf, parent);
  Ast *node = astbufGet(parser->buf, id);

  node->astt = astt;
  return id;
}

static AstID parserPushT(Parser *parser, AstID parent, AstType astt, Token t)
{
  AstID id = astbufAllocNode(parser->buf, parent);
  Ast *node = astbufGet(parser->buf, id);

  node->astt = astt;
  node->token = t;
  return id;
}

static void parserNextAssertTT(Parser *parser, TokenType tt)
{
  if (parser->token.tt != tt) {
    ERR("unexpected token");
  }

  parser->token = lexerNext(&parser->lexer);
}

static void parseSignature(Parser *parser, AstID parent)
{
  AstID signature = parserPushT(parser, parent, ASTT_SIGNATURE, CUR);
  parserNextAssertTT(parser, TT_IDENT);
  parserNextAssertTT(parser, TT_LPAREN);

  while (CUR.tt != TT_RPAREN) {
    parserPushT(parser, signature, ASTT_IDENT, CUR);
    parserNextAssertTT(parser, TT_IDENT);

    if (CUR.tt != TT_RPAREN) {
      parserNextAssertTT(parser, TT_COMMA);
    }
  }

  parserNextAssertTT(parser, TT_RPAREN);
}


static void parseBlock(Parser *parser, AstID parent);

static void parseAtom(Parser *parser, AstID parent)
{
  parserPushT(parser, parent, ASTT_NUMBER, CUR);
  parserNextAssertTT(parser, TT_NUMLIT);
}

static void parseFor(Parser *parser, AstID parent)
{
  AstID for_ = parserPushT(parser, parent, ASTT_FOR, CUR);
  parserNextAssertTT(parser, TT_KW_FOR);

  parseBlock(parser, for_);
}

static void parseCall(Parser *parser, AstID parent)
{
  AstID call = parserPush(parser, parent, ASTT_CALL);
  parserPushT(parser, call, ASTT_CALL, CUR);
  parserNextAssertTT(parser, TT_IDENT);

  AstID params = parserPush(parser, parent, ASTT_PARAMS);
  parserNextAssertTT(parser, TT_LPAREN);

  while (CUR.tt != TT_RPAREN) {
    parseAtom(parser, params);

    if (CUR.tt != TT_RPAREN) {
      parserNextAssertTT(parser, TT_COMMA);
    }
  }

  parserNextAssertTT(parser, TT_RPAREN);
}

static void parseStmt(Parser *parser, AstID parent)
{
  if (CUR.tt == TT_KW_FOR) {
    parseFor(parser, parent);
  } else if (CUR.tt == TT_IDENT) {
    parseCall(parser, parent);
  }
}

static void parseBlock(Parser *parser, AstID parent)
{
  AstID block = parserPush(parser, parent, ASTT_BLOCK);
  parserNextAssertTT(parser, TT_LBRACE);

  while (CUR.tt != TT_RBRACE) {
    parseStmt(parser, block);
  }

  parserNextAssertTT(parser, TT_RBRACE);
}

static void parseFn(Parser *parser, AstID parent)
{
  AstID fn = parserPush(parser, parent, ASTT_FN);

  parserNextAssertTT(parser, TT_KW_FN);
  parseSignature(parser, fn);
  parseBlock(parser, fn);  
}

Parser parserInit(const char *source, AstBuf *buf)
{
  return (Parser){
    .lexer = lexerInit(source),
    .buf = buf
  };
}

AstID parserParse(Parser *parser)
{
  AstID root = parserPush(parser, 0, ASTT_PROGRAM);

  parserNext(parser);

  while (!lexerIsEof(&parser->lexer)) {
    if (CUR.tt == TT_KW_FN) {
      parseFn(parser, root);
    } else {
      ERR("expected `fn`");
    }
  }

  return root;
}

const char *parserAstTypeName(AstType astt)
{
  switch (astt) {
    case ASTT_PROGRAM:   return "program";
    case ASTT_FN:        return "fn";
    case ASTT_SIGNATURE: return "signature";
    case ASTT_CALL:      return "call";
    case ASTT_IDENT:     return "ident";
    case ASTT_BLOCK:     return "block";
    case ASTT_FOR:       return "for";
    case ASTT_NUMBER:    return "number";
    case ASTT_PARAMS:    return "params";
  }
}