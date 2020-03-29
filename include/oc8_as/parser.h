#ifndef OC8_AS_PARSER_H_
#define OC8_AS_PARSER_H_

//===--oc8_as/parser.h - Parser struct definition -----------------*- C -*-===//
//
// oc8_as library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// parser_t struct definition
/// Parse data from a stream to build a Sfile struct
///
//===----------------------------------------------------------------------===//

#include <stddef.h>
#include <stdint.h>

#include "sfile.h"
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// @TODO

/// Parse an input stream to build an sfile
/// User is reponsible for deallocating the sfile and `is`
/// @param is_name name given to the stream for better error messages. optional
/// @returns pointer to newly allocated sfile
oc8_as_sfile_t *oc8_as_run_parser(oc8_as_stream_t *is, const char *is_name);

/// Parse an input file to build an sfile
/// User is reponsible for deallocating the sfile
/// @returns pointer to newly allocated sfile
oc8_as_sfile_t *oc8_as_parse_file(const char *path);

/// Parse a raw ascii string to build an sfile
/// User is reponsible for deallocating the sfile
/// @returns pointer to newly allocated sfile
oc8_as_sfile_t *oc8_as_parse_raw(const char *str, size_t len);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_PARSER_H_
