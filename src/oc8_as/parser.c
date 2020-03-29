#include "oc8_as/parser.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "oc8_as/sfile.h"
#include "oc8_as/stream.h"
#include "oc8_defs/oc8_defs.h"

#define MAX_OPS (3)
#define MAX_INS_NAME_LEN (8)
#define MAX_REG_NAME_LEN (4)

// Wrappper around stream, to count lines / cols, and get better error messages
typedef struct {
  oc8_as_stream_t *is;
  size_t row;       // start at 1
  size_t col;       // start at 1
  const char *name; // file path or (raw) for strings
} reader_t;

static int reader_peekc(reader_t *is) { return oc8_as_stream_peek(is->is); }

static int reader_getc(reader_t *is) {
  int res = oc8_as_stream_get(is->is);
  if (res == '\n') {
    is->col = 1;
    ++is->row;
  } else
    ++is->col;
  return res;
}

static void reader_error(reader_t *is, const char *msg) {
  fprintf(stderr, "oc8_as/parser.c: Syntax error in input file: %s\n", msg);
  fprintf(stderr, "Error near %s:%u:%u\n", is->name, (unsigned)is->row,
          (unsigned)is->col);
  fprintf(stderr, "Aborting !\n");
  PANIC();
}

static void skip_ws(reader_t *is, int skip_nl) {
  for (;;) {
    int c = reader_peekc(is);
    if (c == ' ' || c == '\t') {
      reader_getc(is);
      continue;
    }
    if (skip_nl && c == '\n') {
      reader_getc(is);
      continue;
    }
    break;
  }
}

typedef enum {
  OP_TYPE_IMM,
  OP_TYPE_REG,
  OP_TYPE_REG_IND // register indirect with immediate offset
} op_type_t;

typedef struct {
  int is_sym; // If this operand defined as a symbol
  op_type_t type;

  union {
    char val_reg[MAX_REG_NAME_LEN + 1]; // operand register name
    unsigned val_imm; // operand register value (or 0 if symbol)
  };

  unsigned imm_off; // used only for REG_IND, 0 if symbol
} op_t;

typedef struct {
  op_t ops[MAX_OPS];
  size_t ops_size;
  char name[MAX_INS_NAME_LEN + 1];

  // when one of the ops has imm sym value, stored here
  // no ins have 2 immediates
  // if '\0', no syms
  char sym[OC8_MAX_SYM_SIZE + 1];

} as_ins_t;

#define HEX_UNDEF ((unsigned)-1)

static inline unsigned hex_c2i(int c) { // HEX_UNDEF if not in base
  if (c >= '0' && c <= '9')
    return (unsigned)(c - '0');
  else if (c >= 'a' && c <= 'f')
    return (unsigned)(c - 'a');
  else if (c >= 'A' && c <= 'A')
    return (unsigned)(c - 'A');
  else
    return HEX_UNDEF;
}

static unsigned r_int(reader_t *is, unsigned base, const char *err_msg) {
  unsigned res = 0;
  unsigned len = 0;

  for (;;) {
    unsigned digit = hex_c2i(reader_peekc(is));
    if (digit >= base)
      break;

    reader_getc(is);
    res = base * res + digit;
    ++len;
  }

  if (!len)
    reader_error(is, err_msg);
  return res;
}

static int is_id_char(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_';
}

static int is_lhexa_char(int c) {
  return (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9');
}

static void r_id(reader_t *is, char *out_buf, size_t max_len,
                 const char *err_msg) {
  size_t len = 0;

  for (;;) {
    int c = reader_peekc(is);
    if (!is_id_char(c))
      break;
    reader_getc(is);

    if (len == max_len)
      reader_error(is, err_msg);

    out_buf[len++] = (char)c;
  }

  if (!len)
    reader_error(is, err_msg);
  out_buf[len] = 0;
}

// If imm is integer, returns it
// If label, store it in ins->sym
static unsigned r_imm(reader_t *is, as_ins_t *ins) {
  int c = reader_peekc(is);

  if (c == '0') {
    reader_getc(is);
    c = reader_peekc(is);
    if (c == 'x') {
      reader_getc(is);
      return r_int(is, 16, "Base 16 immediate expected");
    }
    if (c == 'b') {
      reader_getc(is);
      return r_int(is, 2, "Base 2 immediate expected");
    }
    return r_int(is, 8, "Base 8 immediate expected");
  }

  if (c >= '1' && c <= '9')
    return r_int(is, 10, "Base 10 immediate expected");

  if (ins->sym[0])
    reader_error(is, "Cannot have 2 symbols in one instruction");

  r_id(is, ins->sym, OC8_MAX_SYM_SIZE, "Invalid symbol");
  return 0;
}

static void r_reg(reader_t *is, op_t *op) {
  if (reader_getc(is) != '%')
    reader_error(is, "Expected a register");

  r_id(is, op->val_reg, MAX_REG_NAME_LEN, "Invalid register name");
}

// returns 1 if found and parsed one op, 0 otherwhise
static int r_op(reader_t *is, as_ins_t *ins) {
  int c = reader_peekc(is);
  if (!is_id_char(c) && c != '%')
    return 0;

  op_t *op = &ins->ops[ins->ops_size++];
  op->is_sym = 0;

  if (c == '%') { // parse reg
    r_reg(is, op);
    op->type = OP_TYPE_REG;
    return 1;
  }

  int has_sym = !!ins->sym[0];
  op->val_imm = r_imm(is, ins);
  op->type = OP_TYPE_IMM;
  if (!has_sym && ins->sym[0])
    op->is_sym = 1;

  if (reader_peekc(is) != '(')
    return 1;

  // <imm> '(' <reg> ')'
  op->type = OP_TYPE_REG_IND;
  op->imm_off = op->val_imm;
  r_reg(is, op);
  if (reader_getc(is) != ')')
    reader_error(is, "expected )");

  return 1;
}

// return 1 if get an ins, or 0 if get a label
// if label, stored in ins->name
static int r_ins_or_label(reader_t *is, as_ins_t *ins) {
  ins->ops_size = 0;
  ins->sym[0] = 0;

  r_id(is, ins->name, MAX_INS_NAME_LEN, "Invalid instruction name");
  if (reader_peekc(is) == ':') {
    reader_getc(is);
    return 0;
  }

  skip_ws(is, /*skip_nl=*/0);

  int want_op = 0;  // 1 when see a comma (must have an op)
  int want_end = 0; // 1 when didn't see a comma (must end)

  for (;;) {
    int c = reader_peekc(is);
    int is_op = c != '\n' && c != '#';
    if (is_op && want_end)
      reader_error(is, "Unexpected operand without ',' separator");
    if (!is_op && want_op)
      reader_error(is, "Unexpected end of instruction after ',' separator");
    if (!is_op)
      break;

    if (!r_op(is, ins))
      reader_error(is, "Failed to parse operand");

    skip_ws(is, /*skip_nl=*/0);
    if (reader_peekc(is) == ',') {
      reader_getc(is);
      want_op = 1;
      want_end = 0;
      skip_ws(is, /*skip_nl=*/0);
    } else {
      want_op = 0;
      want_end = 1;
    }
  }

  return 1;
}

static void r_comment(reader_t *is) {
  if (reader_getc(is) != '#')
    reader_error(is, "Internal error: r_comment expected start with '#'");

  for (;;) {
    int c = reader_getc(is);
    if (c == '\n' || c == EOF)
      break;
  }
}

typedef struct {
  reader_t is;
  oc8_as_sfile_t *sf;
} parser_t;

static int is_op_vreg(op_t *op) {
  return op->type == OP_TYPE_REG && op->val_reg[0] == 'v' &&
         is_lhexa_char(op->val_reg[1]) && op->val_reg[2] == 0;
}

static int is_op_reg_i(op_t *op) {
  return op->type == OP_TYPE_REG && op->val_reg[0] == 'i' &&
         op->val_reg[1] == 0;
}

static int is_op_reg_dt(op_t *op) {
  return op->type == OP_TYPE_REG && op->val_reg[0] == 'd' &&
         op->val_reg[1] == 't' && op->val_reg[2] == 0;
}

static int is_op_reg_st(op_t *op) {
  return op->type == OP_TYPE_REG && op->val_reg[0] == 's' &&
         op->val_reg[1] == 't' && op->val_reg[2] == 0;
}

static unsigned vreg_idx(op_t *op) {
  char c = op->val_reg[1];
  if (c >= '0' && c <= '9')
    return c - '0';
  else
    return (c - 'a') + 10;
}

static void add_ins(parser_t *parser, as_ins_t *ins) {
  oc8_as_sfile_t *sf = parser->sf;
  size_t nops = ins->ops_size;
  op_t *op0 = &ins->ops[0];
  op_t *op1 = &ins->ops[1];
  op_t *op2 = &ins->ops[1];
  const char *sym = ins->sym[0] == 0 ? NULL : &ins->sym[0];

  if (strcmp(ins->name, "add") == 0) {
    // add <Src:NN>, %v<Dst:X> (7XNN)
    if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_vreg(op1)) {
      unsigned i_src = op0->val_imm;
      if (i_src > 0xFF)
        reader_error(&parser->is, "add imediate operand must be <= 0xFF");
      unsigned r_dst = vreg_idx(op1);
      if (sym)
        oc8_as_sfile_sins_add_imm(sf, sym, r_dst);
      else
        oc8_as_sfile_ins_add_imm(sf, i_src, r_dst);
    }

    // add %v<Src:Y>, %v<Dst:X> (8XY4)
    else if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_add(sf, r_src, r_dst);
    }

    // add %v<Src:X>, %i (FX1E)
    else if (nops == 2 && is_op_vreg(op0) && is_op_reg_i(op1)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_add_i(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for add instruction");
  }

  else if (strcmp(ins->name, "and") == 0) {
    // and %v<Src:Y>, %v<Dst:X>	(8XY2)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_and(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for and instruction");
  }

  else if (strcmp(ins->name, "bcd") == 0) {
    // bcd %v<Src:X> (FX33)
    if (nops == 1 && is_op_vreg(op0)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_bcd(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for and instruction");
  }

  else if (strcmp(ins->name, "call") == 0) {
    // call <Addr:NNN> (2NNN)
    if (nops == 1 && op0->type == OP_TYPE_IMM) {
      unsigned i_src = op0->val_imm;
      if (i_src > 0xFFF)
        reader_error(&parser->is,
                     "call instruction: operand 0 must be <= 0xFFF");
      if (sym)
        oc8_as_sfile_sins_call(sf, sym);
      else
        oc8_as_sfile_ins_call(sf, i_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for call instruction");
  }

  else if (strcmp(ins->name, "cls") == 0) {
    // cls (00E0)
    if (nops == 0) {
      oc8_as_sfile_ins_cls(sf);
    }

    else
      reader_error(&parser->is, "Invalid operands for cls instruction");
  }

  else if (strcmp(ins->name, "draw") == 0) {
    // draw %v<X:X>, %v<Y:Y>, <H:N>  (DXYN)
    if (nops == 3 && is_op_vreg(op0) && is_op_vreg(op1) &&
        op2->type == OP_TYPE_IMM) {
      unsigned r_x = vreg_idx(op0);
      unsigned r_y = vreg_idx(op1);
      unsigned i_h = op2->val_imm;
      if (i_h > 0xF)
        reader_error(&parser->is,
                     "draw instruction: operand #2 must be <= 0xF");
      if (sym)
        oc8_as_sfile_sins_draw(sf, r_x, r_y, sym);
      else
        oc8_as_sfile_ins_draw(sf, r_x, r_y, i_h);
    }

    else
      reader_error(&parser->is, "Invalid operands for draw instruction");
  }

  else if (strcmp(ins->name, "fspr") == 0) {
    // fspr %v<Src:X> (FX29)
    if (nops == 1 && is_op_vreg(op0)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_fspr(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for fspr instruction");
  }

  else if (strcmp(ins->name, "jmp") == 0) {
    // jmp <Addr:NNN> (1NNN)
    if (nops == 1 && op0->type == OP_TYPE_IMM) {
      unsigned i_addr = op0->val_imm;
      if (i_addr > 0xFFF)
        reader_error(&parser->is,
                     "jmp instruction: operand #0 must be <= 0xFFF");
      if (sym)
        oc8_as_sfile_sins_jmp(sf, sym);
      else
        oc8_as_sfile_ins_jmp(sf, i_addr);
    }

    // jmp <Addr:NNN>(%v0) (BNNN)
    else if (nops == 1 && op0->type == OP_TYPE_REG_IND &&
             strcmp(op0->val_reg, "v0") == 0) {
      unsigned i_addr = op0->imm_off;
      if (i_addr > 0xFFF)
        reader_error(&parser->is,
                     "jmp instruction: operand #0 must be <= 0xFFF");
      if (sym)
        oc8_as_sfile_sins_jmp_v0(sf, sym);
      else
        oc8_as_sfile_ins_jmp_v0(sf, i_addr);
    }

    else
      reader_error(&parser->is, "Invalid operand for jmp instruction");
  }

  else if (strcmp(ins->name, "mov") == 0) {
    // mov <Src:NN>, %v<Dst:X> (6XNN)
    if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_vreg(op1)) {
      unsigned i_src = op0->val_imm;
      if (i_src > 0xFF)
        reader_error(&parser->is, "mov imediate operand must be <= 0xFF");
      unsigned r_dst = vreg_idx(op1);
      if (sym)
        oc8_as_sfile_sins_mov_imm(sf, sym, r_dst);
      else
        oc8_as_sfile_ins_mov_imm(sf, i_src, r_dst);
    }

    // mov %v<Src:Y>, %v<Dst:X> (8XY0)
    else if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_mov(sf, r_src, r_dst);
    }

    // mov <Addr:NNN>, %i (ANNN)
    else if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_reg_i(op1)) {
      unsigned i_src = op0->val_imm;
      if (i_src > 0xFFF)
        reader_error(&parser->is, "mov imediate operand must be <= 0xFFF");
      if (sym)
        oc8_as_sfile_sins_mov_i(sf, sym);
      else
        oc8_as_sfile_ins_mov_i(sf, i_src);
    }

    // mov %dt, %v<Dst:X> (FX07)
    else if (nops == 2 && is_op_reg_dt(op0) && is_op_vreg(op1)) {
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_mov_fdt(sf, r_dst);
    }

    // mov %v<Src:X>, %dt (FX15)
    else if (nops == 2 && is_op_vreg(op0) && is_op_reg_dt(op1)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_mov_dt(sf, r_src);
    }

    // mov %v<Src:X>, %st (FX18)
    else if (nops == 2 && is_op_vreg(op0) && is_op_reg_st(op1)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_mov_st(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for mov instruction");
  }

  else if (strcmp(ins->name, "movm") == 0) {
    // movm %v<Src:X>, %i (FX55)
    if (nops == 2 && is_op_vreg(op0) && is_op_reg_i(op1)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_movm_st(sf, r_src);
    }

    // movm %i, %v<Dst:X> (FX65)
    else if (nops == 2 && is_op_reg_i(op0) && is_op_vreg(op1)) {
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_movm_ld(sf, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for movm instruction");
  }

  else if (strcmp(ins->name, "or") == 0) {
    // or %v<Src:Y>, %v<Dst:X> (8XY1)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_or(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for or instruction");
  }

  else if (strcmp(ins->name, "rand") == 0) {
    // rand <Mask:NN>, %v<Dst:X> (CXNN)
    if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_vreg(op1)) {
      unsigned i_mask = op0->val_imm;
      if (i_mask > 0xFF)
        reader_error(&parser->is, "rand imediate operand must be <= 0xFF");
      unsigned r_dst = vreg_idx(op1);
      if (sym)
        oc8_as_sfile_sins_rand(sf, sym, r_dst);
      else
        oc8_as_sfile_ins_rand(sf, i_mask, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for rand instruction");
  }

  else if (strcmp(ins->name, "ret") == 0) {
    // ret (00EE)
    if (nops == 0) {
      oc8_as_sfile_ins_ret(sf);
    }

    else
      reader_error(&parser->is, "Invalid operands for ret instruction");
  }

  else if (strcmp(ins->name, "shl") == 0) {
    // shl %v<Src:Y>, %v<Dst:X> (8XYE)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_shl(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for shl instruction");
  }

  else if (strcmp(ins->name, "shr") == 0) {
    // shr %v<Src:Y>, %v<Dst:X> (8XY6)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_shr(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for shr instruction");
  }

  else if (strcmp(ins->name, "skpe") == 0) {
    // skpe <Y:NN>, %v<X:X> (3XNN)
    if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_vreg(op1)) {
      unsigned i_x = op0->val_imm;
      if (i_x > 0xFF)
        reader_error(&parser->is, "skpe imediate operand must be <= 0xFF");
      unsigned r_y = vreg_idx(op1);
      if (sym)
        oc8_as_sfile_sins_skpe_imm(sf, sym, r_y);
      else
        oc8_as_sfile_ins_skpe_imm(sf, i_x, r_y);
    }

    // skpe %v<Y:Y>, %v<X:X> (5XY0)
    else if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_x = vreg_idx(op0);
      unsigned r_y = vreg_idx(op1);
      oc8_as_sfile_ins_skpe(sf, r_x, r_y);
    }

    else
      reader_error(&parser->is, "Invalid operands for skpe instruction");
  }

  else if (strcmp(ins->name, "skpn") == 0) {
    // skpn <Y:NN>, %v<X:X> (4XNN)
    if (nops == 2 && op0->type == OP_TYPE_IMM && is_op_vreg(op1)) {
      unsigned i_x = op0->val_imm;
      if (i_x > 0xFF)
        reader_error(&parser->is, "skpn imediate operand must be <= 0xFF");
      unsigned r_y = vreg_idx(op1);
      if (sym)
        oc8_as_sfile_sins_skpn_imm(sf, sym, r_y);
      else
        oc8_as_sfile_ins_skpn_imm(sf, i_x, r_y);
    }

    // skpn %v<Y:Y>, %v<X:X> (9XY0)
    else if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_x = vreg_idx(op0);
      unsigned r_y = vreg_idx(op1);
      oc8_as_sfile_ins_skpn(sf, r_x, r_y);
    }

    else
      reader_error(&parser->is, "Invalid operands for skpn instruction");
  }

  else if (strcmp(ins->name, "skpkp") == 0) {
    // skpkp %v<K:X> (EX9E)
    if (nops == 1 && is_op_vreg(op0)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_skpkp(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for skpkp instruction");
  }

  else if (strcmp(ins->name, "skpkn") == 0) {
    // skpkn %v<K:X> (EXA1)
    if (nops == 1 && is_op_vreg(op0)) {
      unsigned r_src = vreg_idx(op0);
      oc8_as_sfile_ins_skpkn(sf, r_src);
    }

    else
      reader_error(&parser->is, "Invalid operands for skpkn instruction");
  }

  else if (strcmp(ins->name, "sub") == 0) {
    // sub %v<Src:Y>, %v<Dst:X> (8XY5)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_sub(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for sub instruction");
  }

  else if (strcmp(ins->name, "subn") == 0) {
    // subn %v<Src:Y>, %v<Dst:X> (8XY7)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_subn(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for subn instruction");
  }

  else if (strcmp(ins->name, "sys") == 0) {
    // sys <Addr:NNN> (0NNN)
    if (nops == 1 && op0->type == OP_TYPE_IMM) {
      unsigned i_addr = op0->val_imm;
      if (i_addr > 0xFFF)
        reader_error(&parser->is, "sys imediate operand must be <= 0xFFF");
      if (sym)
        oc8_as_sfile_sins_sys(sf, sym);
      else
        oc8_as_sfile_ins_sys(sf, i_addr);
    } else
      reader_error(&parser->is, "Invalid operands for sys instruction");
  }

  else if (strcmp(ins->name, "waitk") == 0) {
    // waitk %v<Dst:X> (FX0A)
    if (nops == 1 && is_op_vreg(op0)) {
      unsigned r_dst = vreg_idx(op0);
      oc8_as_sfile_ins_waitk(sf, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for waitk instruction");
  }

  else if (strcmp(ins->name, "xor") == 0) {
    // xor %v<Src:Y>, %v<Dst:X> (8XY3)
    if (nops == 2 && is_op_vreg(op0) && is_op_vreg(op1)) {
      unsigned r_src = vreg_idx(op0);
      unsigned r_dst = vreg_idx(op1);
      oc8_as_sfile_ins_xor(sf, r_src, r_dst);
    }

    else
      reader_error(&parser->is, "Invalid operands for subn instruction");
  }

  else
    reader_error(&parser->is, "Unknown instruction");
}

// Try to parse a declaration
// Returns 1 if OK, 0 if EOF reached
static int r_decl(parser_t *ps) {
  skip_ws(&ps->is, /*skip_nl=*/1);
  int c = reader_peekc(&ps->is);
  if (c == EOF)
    return 0;

  // parse comment
  if (c == '#') {
    r_comment(&ps->is);
    return 1;
  }

  // parse directive
  if (c == '.') {
    reader_error(&ps->is, "directive not implemented yet");
    return 1;
    // @TODO Parse directives
  }

  // parse instruction or label
  as_ins_t ins;
  int is_ins = r_ins_or_label(&ps->is, &ins);
  if (!is_ins) {
    oc8_as_sfile_add_sym(ps->sf, ins.name);
    return 1;
  }

  add_ins(ps, &ins);
  return 1;
}

static void r_file(parser_t *ps) {
  while (r_decl(ps))
    continue;
}

oc8_as_sfile_t *oc8_as_run_parser(oc8_as_stream_t *is, const char *is_name) {
  oc8_as_sfile_t *sf = oc8_as_sfile_new();
  parser_t ps;
  ps.is.row = 1;
  ps.is.col = 1;
  ps.is.is = is;
  ps.is.name = is_name ? is_name : "???";
  ps.sf = sf;
  r_file(&ps);
  return sf;
}

/// Parse an input file to build an sfile
/// User is reponsible for deallocating the sfile
/// @returns pointer to newly allocated sfile
oc8_as_sfile_t *oc8_as_parse_file(const char *path) {
  FILE *f = fopen(path, "r");
  if (!f) {
    fprintf(stderr, "oc8_as_parser: failed to open input file `%s`\n", path);
    PANIC();
  }
  oc8_as_stream_t is;
  oc8_as_stream_init_from_file(&is, f);
  oc8_as_sfile_t *res = oc8_as_run_parser(&is, path);
  oc8_as_stream_free(&is);
  fclose(f);
  return res;
}

oc8_as_sfile_t *oc8_as_parse_raw(const char *str, size_t len) {
  oc8_as_stream_t is;
  oc8_as_stream_init_from_raw(&is, str, len);
  oc8_as_sfile_t *res = oc8_as_run_parser(&is, "(raw string)");
  oc8_as_stream_free(&is);
  return res;
}
