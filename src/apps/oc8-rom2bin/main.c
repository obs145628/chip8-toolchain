#include <stdio.h>
#include <stdlib.h>

#include "args_parser/args_parser.h"
#include "oc8_bin/bin_writer.h"
#include "oc8_bin/file.h"

static inline void io_err(const char *in_path) {
  fprintf(stderr, "oc8-rom2bin: Failed to read input file `%s'.\n", in_path);
  exit(1);
}

args_parser_option_t opts[3] = {
    {
        .name = "input",
        .type = ARGS_PARSER_OTY_PRIM,
        .desc = "Path to input binary file (.c8bin)",
        .required = 1,
    },

    {
        .name = "output",
        .id_short = 'o',
        .id_long = "output",
        .type = ARGS_PARSER_OTY_VAL,
        .desc = "Path to output CHIP-8 ROM file",
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

  // Read input rom file
  FILE *is = fopen(in_path, "rb");
  if (!is)
    io_err(in_path);
  if (fseek(is, 0, SEEK_END) != 0)
    io_err(in_path);
  size_t rom_size = ftell(is);
  if (!rom_size)
    io_err(in_path);
  if (fseek(is, 0, SEEK_SET) != 0)
    io_err(in_path);
  char *rom_data = malloc(sizeof(rom_size));
  if (fread(rom_data, 1, rom_size, is) != rom_size)
    io_err(in_path);

  // Create bin object
  oc8_bin_file_t bf;
  oc8_bin_file_init(&bf);
  oc8_bin_file_check(&bf, /*is_bin=*/1);
  // @TODO generate empty bin with ROM

  // Write bin to output file
  oc8_bin_write_to_file(&bf, out_path);

  // Cleanup files
  oc8_bin_file_free(&bf);
  fclose(is);
  free(rom_data);
  return 0;
}
