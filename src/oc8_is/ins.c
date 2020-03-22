#include "oc8_is/ins.h"

/// Decode the instruction stored in the 2 bytes of `buf`, and
/// Write result in `ins`
/// @returns != 0 if failed to decode instruction
int oc8_is_decode_ins(oc8_is_ins_t *ins, const char *buf) {
  // @TODO
  (void)ins;
  (void)buf;
  return 0;
}

/// Encode the instruction `ins`
/// Write the opcode in 2 bytes of `buf`
/// Also write it in opcode field of `ins`
/// @returnds != 0 il failed to encode instruction
int oc8_is_encode_ins(const oc8_is_ins_t *ins, char *buf) {
  // @TODO
  (void)ins;
  (void)buf;
  return 0;
}
