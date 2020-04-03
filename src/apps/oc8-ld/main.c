#include <stdio.h>
#include <stdlib.h>

#include "args_parser/args_parser.h"
#include "oc8_bin/bin_reader.h"
#include "oc8_bin/file.h"
#include "oc8_ld/linker.h"

#define MAX_IN_FILES 64

#if 0

static inline void read_err(const char *in_path) {
  fprintf(stderr, "oc8-ld: Failed to read input file `%s'.\n", in_path);
  exit(1);
}

static inline void write_err(const char *out_path) {
  fprintf(stderr, "oc8-ld: Failed to write output file `%s'.\n", out_path);
  exit(1);
}

#endif

args_parser_option_t opts[2] = {
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
    .bin_name = "oc8-ld",
    .options_arr = opts,
    .options_size = 2,
    .have_others = 1,
};

int main(int argc, char **argv) {
  args_parser_run(&ap, argc, argv);
  const char *out_path = opts[0].value;
  size_t nb_inputs = 0;

  oc8_ld_linker_t ld;
  oc8_bin_file_t in_objs[MAX_IN_FILES];
  oc8_ld_linker_init(&ld, 1);

  for (int i = 1; i < argc; ++i) {
    if (argv[i] == out_path || (i + 1 < argc && argv[i + 1] == out_path))
      continue;

    oc8_bin_file_t *obj = &in_objs[nb_inputs++];
    if (nb_inputs > MAX_IN_FILES) {
      fprintf(stderr, "oc8-ld: Cannot have more than %d input files.\n",
              MAX_IN_FILES);
      return 1;
    }

    const char *in_path = argv[i];
    oc8_bin_read_from_file(obj, in_path);
    oc8_bin_file_check(obj, /*is_bin=*/0);
    if (obj->header.type != OC8_BIN_FILE_TYPE_OBJ) {
      fprintf(stderr, "oc8-ld: Input file `%s' must be of type object.\n",
              in_path);
      return 1;
    }

    oc8_ld_linker_add(&ld, obj);
  }

  printf("output file is `%s'\n", out_path);

#if 0
  
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

#endif

  for (size_t i = 0; i < nb_inputs; ++i)
    oc8_bin_file_free(&in_objs[i]);
  oc8_ld_linker_free(&ld);

  return 0;
}
