#include <stdio.h>
#include <stdlib.h>

#include "args_parser/args_parser.h"
#include "oc8_bin/bin_reader.h"
#include "oc8_bin/file.h"

static inline void io_err(const char *out_path) {
  fprintf(stderr, "oc8-bin2rom: Failed to write output file `%s'.\n", out_path);
  exit(1);
}

args_parser_option_t opts[3] = {
    {
        .name = "input",
        .type = ARGS_PARSER_OTY_PRIM,
        .desc = "Path to input CHIP-8 ROM file",
        .required = 1,
    },

    {
        .name = "output",
        .id_short = 'o',
        .id_long = "output",
        .type = ARGS_PARSER_OTY_VAL,
        .desc = "Path to output object bin file (.c8bin)",
        .required = 1,
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
    .bin_name = "oc8-bin2rom",
    .options_arr = opts,
    .options_size = 3,
    .have_others = 0,
};

int main(int argc, char **argv) {
  args_parser_run(&ap, argc, argv);
  const char *in_path = opts[0].value;
  const char *out_path = opts[1].value;

  // Read and check file
  oc8_bin_file_t bf;
  oc8_bin_read_from_file(&bf, in_path);
  oc8_bin_file_check(&bf, /*is_bin=*/1);

  // Write ROM to output file
  FILE *os = fopen(out_path, "wb");
  if (!os)
    io_err(out_path);
  if (fwrite(bf.rom, 1, bf.rom_size, os) != bf.rom_size)
    io_err(out_path);

  // Cleanup files
  oc8_bin_file_free(&bf);
  fclose(os);
  return 0;
}
