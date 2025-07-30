#include "../include/parse.h"
#include <stdio.h>
#include <string.h>

void handle_quotes(char **rest, char **tokens, int *token_position) {
  if (*rest[0] == '\"' || *rest[0] == '\'') {
    printf("quote mil gea, sire!: %c\n", *rest[0]);
    char quote_delimiter = *rest[0];
    char *closing_quote = strchr(*rest + 1, quote_delimiter);
    if (!closing_quote)
      return;
    printf("closing quote: %s\n", closing_quote);
    tokens[*token_position] = *rest;
    token_position++;
    *rest = closing_quote + 1;
  }
}

void handle_escape();
