#ifndef _CTC_UMKA_ERROR_H_
#define _CTC_UMKA_ERROR_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int line, col, pos;
} Location;

static void errorShow(Location loc, const char *msg)
{
  fprintf(stderr, "\x1b[31m%d:%d: %s\x1b[0m", loc.line+1, loc.col+1, msg);
  exit(1);
}

#endif
