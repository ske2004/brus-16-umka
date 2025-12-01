#include "umka_lexer.h"
#include <stdbool.h>

#define CUR lexer->source[lexer->loc.pos]
#define CURP(i) lexer->source[lexer->loc.pos+(i)]
#define ERR(msg) errorShow(t.begin, (msg))

static bool isSpaceChar(char c)
{
  return c == '\r' || c == '\n' || c == ' ' || c == '\t' || c == '\v';
}

static bool isIdentChar(char c, bool first)
{
  return (c == '_') || 
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (!first && (c >= '0' && c <= '9'));
}

static void lexerNextChar(Lexer *lexer)
{
  if (CUR == '\n')  {
    lexer->loc.col = 0;
    lexer->loc.line++;
  } else {
    lexer->loc.col++;
  }
  lexer->loc.pos++;
}

static bool lexerSkipJunk(Lexer *lexer)
{
  bool hasJunk = false;

  while (isSpaceChar(CUR)) {
    hasJunk = true;
    lexerNextChar(lexer);
  }

  if (CURP(0) == '/' && CURP(1) == '/') {
    hasJunk = true;
    while (CUR && CUR != '\n')
      lexerNextChar(lexer);
  }

  return hasJunk;
}

Lexer lexerInit(const char *source)
{
  return (Lexer){
    source
  };
}

Token lexerNext(Lexer *lexer)
{
  while (lexerSkipJunk(lexer))
    ;

  Token t = {
    .tt = TT_INVALID,
    .begin = lexer->loc,
  };

  switch (CUR) {
  case '\0':
    break;
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    t.tt = TT_NUMLIT;
    while (CUR >= '0' && CUR <= '9')
      lexerNextChar(lexer);
    break;
  case '"':
    t.tt = TT_STRLIT;
    lexerNextChar(lexer);
    while (CUR && CUR != '"')
      lexerNextChar(lexer);
    if (!CUR)
      ERR("unclosed string");
    lexerNextChar(lexer);
    break;
  case '{': t.tt = TT_LBRACE; lexerNextChar(lexer); break;
  case '}': t.tt = TT_RBRACE; lexerNextChar(lexer); break;
  case '[': t.tt = TT_LBRACK; lexerNextChar(lexer); break;
  case ']': t.tt = TT_RBRACK; lexerNextChar(lexer); break;
  case '(': t.tt = TT_LPAREN; lexerNextChar(lexer); break;
  case ')': t.tt = TT_RPAREN; lexerNextChar(lexer); break;
  case ',': t.tt = TT_COMMA;  lexerNextChar(lexer); break;
  case ':':
    lexerNextChar(lexer);
    if (CUR == ':') {
      t.tt = TT_COLONCOLON;
      lexerNextChar(lexer);
    } else {
      ERR("stray ':'");
    }
    break;
  default:
    // ident-like
    if (isIdentChar(CUR, true)) {
      t.tt = TT_IDENT;
      while (isIdentChar(CUR, false))
        lexerNextChar(lexer);
      
      if (lexerTokenValueEq(lexer, t, "fn"))
        t.tt = TT_KW_FN;
      else if (lexerTokenValueEq(lexer, t, "for"))
        t.tt = TT_KW_FOR;
      else if (lexerTokenValueEq(lexer, t, "import"))
        t.tt = TT_KW_IMPORT;
    } else {
      ERR("unknown character");
    }
    break;
  }

  t.end = lexer->loc;
  return t;
}

bool lexerIsEof(Lexer *lexer)
{
  // hmm, not sure about this but we ball
  while (lexerSkipJunk(lexer))
    ;
 
  return CUR == '\0';
}

const char *lexerTokenTypeName(TokenType tt)
{
  switch (tt) {
    case TT_NUMLIT:     return "numlit";
    case TT_STRLIT:     return "strlit";
    case TT_IDENT:      return "ident";
    case TT_KW_FN:      return "kw_fn";
    case TT_KW_FOR:     return "kw_for";
    case TT_KW_IMPORT:  return "kw_import";
    case TT_LBRACE:     return "lbrace";
    case TT_RBRACE:     return "rbrace";
    case TT_LBRACK:     return "lbrack";
    case TT_RBRACK:     return "rbrack";
    case TT_LPAREN:     return "lparen";
    case TT_RPAREN:     return "rparen";
    case TT_COLONCOLON: return "coloncolon";
    case TT_COMMA:      return "comma";
    default:            return "invalid";
  }
}

bool lexerTokenValueEq(const Lexer *lexer, Token t, const char *ref)
{
  for (int i = t.begin.pos, k = 0; i < lexer->loc.pos; i++, k++)
    if (lexer->source[i] != ref[k])
      return false;

  return true;
}

void lexerTokenValue(const Lexer *lexer, Token t, char *buf)
{
  if (t.tt == TT_INVALID)
    goto end;

  for (int i = t.begin.pos; i < t.end.pos; i++)
    *buf++ = lexer->source[i];

end:
  *buf = 0;
}
