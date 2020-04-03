#include "args_parser/args_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage_exit(args_parser_t *ap, int code) {
  fprintf(stderr, "Usage: %s", ap->bin_name);
  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type == ARGS_PARSER_OTY_PRIM) {
      if (curr->required)
        fprintf(stderr, " <%s>", curr->name);
      else
        fprintf(stderr, " [%s]", curr->name);
    }
  }
  fprintf(stderr, " [Options...]\n");

  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type == ARGS_PARSER_OTY_PRIM && curr->desc) {
      fprintf(stderr, "  %s: %s", curr->name, curr->desc);
      if (curr->required)
        fprintf(stderr, " (REQUIRED)");
      fprintf(stderr, "\n");
    }
  }

  fprintf(stderr, "Options:\n");

  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type == ARGS_PARSER_OTY_PRIM)
      continue;

    fprintf(stderr, "- %s:", curr->name);
    if (curr->id_short)
      fprintf(stderr, "  -%c", curr->id_short);
    if (curr->id_long)
      fprintf(stderr, "  --%s", curr->id_long);
    if (curr->type == ARGS_PARSER_OTY_VAL)
      fprintf(stderr, "  <value>");
    if (curr->required)
      fprintf(stderr, " (REQUIRED)");
    fprintf(stderr, "\n");
    if (curr->desc)
      fprintf(stderr, "    %s\n", curr->desc);
  }

  exit(code);
}

static int is_short(const char *str) {
  return str[0] == '-' && str[1] != '-' && str[1] != '\0';
}

static int is_long(const char *str) {
  return str[0] == '-' && str[1] == '-' && str[2] != '\0';
}

static int is_val(const char *str) { return !is_short(str) && !is_long(str); }

static void add_prim(args_parser_t *ap, const char *val) {

  args_parser_option_t *node = NULL;
  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type == ARGS_PARSER_OTY_PRIM && curr->found == 0) {
      node = curr;
      break;
    }
  }

  if (!node && ap->have_others)
    return;

  if (!node) {
    fprintf(stderr, "%s: primary value `%s' not allowed here.\n", ap->bin_name,
            val);
    usage_exit(ap, 1);
  }

  node->found = 1;
  node->value = val;
}

static int add_opt(args_parser_t *ap, const char *opt, const char *next) {
  char os = 0;
  const char *ol = NULL;
  if (is_short(opt))
    os = opt[1];
  else
    ol = opt + 2;

  args_parser_option_t *node = NULL;
  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type != ARGS_PARSER_OTY_PRIM &&
        ((curr->id_short && curr->id_short == os) ||
         (curr->id_long && ol && strcmp(curr->id_long, ol) == 0))) {
      node = curr;
      break;
    }
  }

  if (!node) {
    fprintf(stderr, "%s: unknown option `%s'.\n", ap->bin_name, opt);
    usage_exit(ap, 1);
  }

  if (node->type == ARGS_PARSER_OTY_HELP)
    usage_exit(ap, 0);

  if (node->found) {
    fprintf(stderr, "%s: option `%s' specified twice.\n", ap->bin_name,
            node->name);
    usage_exit(ap, 1);
  }

  node->found = 1;

  if (node->type == ARGS_PARSER_OTY_VAL) {
    if (!next || !is_val(next)) {
      fprintf(stderr, "%s: option `%s` was expecting a value.\n", ap->bin_name,
              node->name);
      usage_exit(ap, 1);
    }
    node->value = next;
    return 2;
  }

  return 1;
}

void args_parser_run(args_parser_t *ap, int argc, char **argv) {
  int pos = 1;
  for (size_t i = 0; i < ap->options_size; ++i)
    ap->options_arr[i].found = 0;

  while (pos < argc) {
    if (is_val(argv[pos])) {
      add_prim(ap, argv[pos]);
      ++pos;
    }

    else {
      int inc = add_opt(ap, argv[pos], pos + 1 < argc ? argv[pos + 1] : NULL);
      pos += inc;
    }
  }

  for (size_t i = 0; i < ap->options_size; ++i) {
    args_parser_option_t *curr = &ap->options_arr[i];
    if (curr->type != ARGS_PARSER_OTY_FLAG && curr->required && !curr->found) {
      fprintf(stderr, "%s: required option `%s' is missing.\n", ap->bin_name,
              curr->name);
      usage_exit(ap, 1);
    }
  }
}
