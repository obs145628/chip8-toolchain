#include <stdlib.h>
#include <string.h>

#include "args_parser/args_parser.h"
#include "oc8_as/as.h"
#include "oc8_as/parser.h"
#include "oc8_as/sfile.h"
#include "oc8_bin/bin_writer.h"
#include "oc8_bin/file.h"

args_parser_option_t opts[3] = {
    {
        .name = "input",
        .type = ARGS_PARSER_OTY_PRIM,
        .desc = "Path to input text assembly file (.c8s)",
        .required = 1,
    },

    {
        .name = "output",
        .id_short = 'o',
        .id_long = "output",
        .type = ARGS_PARSER_OTY_VAL,
        .desc = "Path to output object file (.c8o)",
        .required = 0,
    },

    {
        .name = "help",
        .id_short = 'h',
        .id_long = "help",
        .type = ARGS_PARSER_OTY_HELP,
        .desc = "Print an help message an exit",
    },
};

args_parser_t ap = {
    .bin_name = "oc8-as",
    .options_arr = opts,
    .options_size = 3,
    .have_others = 0,
};

int main(int argc, char **argv) {
  char *out_buff = NULL;

  args_parser_run(&ap, argc, argv);

  if (opts[1].value == NULL) { // no output, generate file path
    size_t len = strlen(opts[0].value);
    out_buff = malloc(len + 1);
    memcpy(out_buff, opts[0].value, len + 1);
    out_buff[len - 1] = 'o'; // turn c8s into c80
    opts[1].value = out_buff;
  }

  const char *in_path = opts[0].value;
  const char *out_path = opts[1].value;

  // Parse input file
  oc8_as_sfile_t *sf = oc8_as_parse_file(in_path);
  oc8_as_sfile_check(sf);

  // Compile input file to binary object
  oc8_bin_file_t bf;
  oc8_as_compile_sfile(sf, &bf);
  oc8_bin_file_check(&bf, /*is_bin=*/0);

  // Write binary object to file
  oc8_bin_write_to_file(&bf, out_path);

  // Cleanup data
  oc8_as_sfile_free(sf);
  oc8_bin_file_free(&bf);
  free(out_buff);
  return 0;
}
