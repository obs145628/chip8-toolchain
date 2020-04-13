#include <stdio.h>
#include <stdlib.h>

#include "args_parser/args_parser.h"
#include "oc8_bin/bin_reader.h"
#include "oc8_bin/file.h"
#include "oc8_bin/printer.h"
#include "oc8_defs/oc8_defs.h"

args_parser_option_t opts[2] = {
    {
        .name = "input",
        .type = ARGS_PARSER_OTY_PRIM,
        .desc = "Path to input binary file (.c8o/.c8bin)",
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
    .bin_name = "oc8-objdump",
    .options_arr = opts,
    .options_size = 2,
    .have_others = 0,
};

static int has_label_def(const char *str) {
  while (*str) {
    if (*str == '\n')
      return 1;
    ++str;
  }
  return 0;
}

int main(int argc, char **argv) {
  args_parser_run(&ap, argc, argv);
  const char *in_path = opts[0].value;

  // Read and check file
  oc8_bin_file_t bf;
  oc8_bin_read_from_file(&bf, in_path);
  oc8_bin_file_check(&bf, /*is_bin=*/0);

  oc8_bin_printer_t printer;
  oc8_bin_printer_init(&printer, &bf);
  uint16_t addr = 0;
  while (addr < bf.rom_size) {
    uint16_t inc;
    size_t txt_len;
    oc8_bin_printer_print_at(&printer, addr + OC8_ROM_START,
                             /*print_sym_defs=*/1, /*print_sym_refs=*/1,
                             /*print_sym_ids=*/0,
                             /*data_hint=*/OC8_BIN_PRINTER_DATA_UNKOWN,
                             &txt_len, &inc);

    int is_label = has_label_def(printer.out_buf);
    if (is_label)
      printf("\n");

    // Cut str to print line by line with addr info
    char *ptr_beg = printer.out_buf;
    for (;;) {
      char *ptr_end = ptr_beg;
      while (*ptr_end != '\n' && *ptr_end != '\0')
        ++ptr_end;
      int reached_end = *ptr_end == '\0';
      *ptr_end = '\0';
      if (is_label)
        printf("%09X ", (unsigned)(addr + OC8_ROM_START));
      else
        printf(" %08X:", (unsigned)(addr + OC8_ROM_START));
      printf(" %s\n", ptr_beg);
      if (reached_end)
        break;
      ptr_beg = ptr_end + 1;
      is_label = 0;
    }

    addr += inc;
  }

  // Cleanup files
  oc8_bin_file_free(&bf);
  oc8_bin_printer_free(&printer);
  return 0;
}
