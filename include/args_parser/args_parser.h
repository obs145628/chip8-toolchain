#ifndef ARGS_PARSER_H_
#define ARGS_PARSER_H_

//===--args-parser.h - args_parser_t struct definition -------------*- C
//-*-===//
//
// oc8-as program
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// args_parser_t struct definition
/// Struct used to parse command line arguments
///
//===----------------------------------------------------------------------===//

#include <stddef.h>

typedef enum {
  ARGS_PARSER_OTY_PRIM, // primary argument given to the command
  ARGS_PARSER_OTY_VAL,  // argument -v <val> (or --val <val>)
  ARGS_PARSER_OTY_FLAG, // arguent -f or --flag with no value
  ARGS_PARSER_OTY_HELP, // trigger help message + exit if found
} args_parser_option_type_t;

typedef struct {
  // Fields that must be initialized
  const char *name;
  char id_short;       // 0 if no short version
  const char *id_long; // NULL if no long version
  args_parser_option_type_t type;
  const char *desc; // optional description
  int required;     // ignored if type == flag or help

  // Fields filled by parser
  int found; // 1 if the option is found, 0 otherwhise
  const char
      *value; // Used for prim / val types, pointer to argv with actual value
} args_parser_option_t;

typedef struct {
  const char *bin_name;
  args_parser_option_t *options_arr;
  size_t options_size;
  int have_others; // if true, can have options not handled by the arguments
                   // parser
} args_parser_t;

/// Run the arguments parser on `(argc, argv)`
/// `ap` must be already initialized with all the informations
/// the functions fills the other fields.
void args_parser_run(args_parser_t *ap, int argc, char **argv);

#endif //! ARGS_PARSER_H_
