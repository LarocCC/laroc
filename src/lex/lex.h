#ifndef LAROC_LEX_LEX_H
#define LAROC_LEX_LEX_H

#include "typedef.h"

/// Scan tokens in [source, source+len), return a array of tokens.
Token *lex(const char *source, int len);

#endif
