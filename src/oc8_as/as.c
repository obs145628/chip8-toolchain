#include "oc8_as/as.h"
#include "oc8_defs/oc8_defs.h"

#include <stdlib.h>
#include <string.h>

#if 0

void oc8_bin_file_init(oc8_bin_file_t *bf);

/// Free all memory allocated by `bf`
void oc8_bin_file_free(oc8_bin_file_t *bf);

/// Check if the file is valid, after it's complete
/// If `is_bin`, do more tests to be sure it can be runned
/// Panics if there is an error
void oc8_bin_file_check(oc8_bin_file_t *bf, int is_bin);

void oc8_bin_file_set_version(oc8_bin_file_t *bf, uint16_t version);

void oc8_bin_file_set_type(oc8_bin_file_t *bf, oc8_bin_file_type_t type);

/// Can only be called once to set the max number of definitions
/// Cannot be changed later
void oc8_bin_file_set_defs_count(oc8_bin_file_t *bf, size_t len);

/// @returns id of the new symbol
uint16_t oc8_bin_file_add_def(oc8_bin_file_t *bf, const char *name,
                              int is_global, oc8_bin_sym_type_t type,
                              uint16_t addr);

/// Can add a ref before it's added in the defs
void oc8_bin_file_add_ref(oc8_bin_file_t *bf, uint16_t ins_addr,
                          uint16_t sym_id);

/// Allocate memory for the ROM, can only be called once,
/// Cannot be resized later
void oc8_bin_file_init_rom(oc8_bin_file_t *bf, size_t rom_size);

#endif

void oc8_as_compile_sfile(oc8_as_sfile_t *sf, oc8_bin_file_t *bf) {
  // Set header
  oc8_bin_file_init(bf);
  oc8_bin_file_set_version(bf, 10);
  oc8_bin_file_set_type(bf, OC8_BIN_FILE_TYPE_OBJ);

  // Init defs_list and array [sf_sym_id => bf_sym_id]
  // Need to substract -1 and add 1 to size because def counts starts at 1 for
  // sfile
  size_t nb_defs = sf->next_sym_idx - 1;
  uint16_t *ids_map = malloc((nb_defs + 1) * sizeof(uint16_t));
  oc8_bin_file_set_defs_count(bf, nb_defs);

  // Go through all the symbols used in the file, and add it to the defs table
  // If symbol defined in file, also add infos about it
  // Symbol order is unspecified
  oc8_smap_it_t it = oc8_smap_get_it(&sf->syms_map);
  size_t bf_sym_id = 0;
  while (oc8_smap_it_get(&it) != NULL) {
    const char *sym_name = oc8_smap_it_get(&it)->key;
    uint16_t sf_sym_id = oc8_smap_it_get(&it)->val;
    oc8_smap_node_t *node_defs = oc8_smap_find(&sf->syms_defs_map, sym_name);
    oc8_as_sym_def_t *def =
        node_defs ? &sf->syms_defs_arr[node_defs->val] : NULL;
    ids_map[sf_sym_id] = bf_sym_id;

    int is_global = !def || def->is_global;
    oc8_bin_sym_type_t type = OC8_BIN_SYM_TYPE_NO;
    if (def && def->type == OC8_AS_DATA_SYM_TYPE_FUN)
      type = OC8_BIN_SYM_TYPE_FUN;
    else if (def && def->type == OC8_AS_DATA_SYM_TYPE_OBJ)
      type = OC8_BIN_SYM_TYPE_OBJ;
    uint16_t addr = def ? def->pos + OC8_ROM_START : 0;

    oc8_bin_file_add_def(bf, sym_name, is_global, type, addr);
    oc8_smap_it_next(&it);
    ++bf_sym_id;
  }

  // Prepare rom
  size_t rom_size = sf->curr_addr;
  oc8_bin_file_init_rom(bf, rom_size);
  uint8_t *rom_ptr = (uint8_t *)bf->rom;

  // Go through all items, write to ROM and build refs table at the same time
  for (size_t i = 0; i < sf->items_size; ++i) {
    oc8_as_data_item_t item = sf->items_arr[i];

    if (item.type == OC8_AS_DATA_ITEM_TYPE_INS) {
      *((uint16_t *)&rom_ptr[item.pos]) = item.ins_opcode;

      if (item.sym_idx != 0) {
        uint16_t ins_addr = OC8_ROM_START + item.pos;
        uint16_t sym_id = ids_map[item.sym_idx];
        oc8_bin_file_add_ref(bf, ins_addr, sym_id);
      }
    }

    else if (item.type == OC8_AS_DATA_ITEM_TYPE_ALIGN) {
      size_t next_pos =
          i + 1 < sf->items_size ? sf->items_arr[i + 1].pos : rom_size;
      size_t nzeros = next_pos - item.pos;
      memset(rom_ptr + item.pos, 0, nzeros);
    }

    else if (item.type == OC8_AS_DATA_ITEM_TYPE_BYTE)
      rom_ptr[item.pos] = item.byte_val;
    else if (item.type == OC8_AS_DATA_ITEM_TYPE_WORD)
      *((uint16_t *)&rom_ptr[item.pos]) = item.word_val;
    else if (item.type == OC8_AS_DATA_ITEM_TYPE_ZERO)
      memset(rom_ptr + item.pos, 0, item.zero_n);
  }

  // Clean up
  free(ids_map);
}
