#ifndef OC8_AS_STREAM_H_
#define OC8_AS_STREAM_H_

//===--oc8_as/stream.h - Stream struct definition -----------------*- C -*-===//
//
// oc8_as library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Stream struct definition
///
//===----------------------------------------------------------------------===//

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// The actual function that read one byte from the underlying input
typedef int (*oc8_as_functor_fn_f)(void *functor_ptr);

typedef struct {
  oc8_as_functor_fn_f fn;
  char data[];
} oc8_as_functor_t;

/// Struct used to read from an input stream byte by byte, using dynamic
/// dispatch
typedef struct {
  oc8_as_functor_t *getc_fn;
  char last;
  int last_def;
} oc8_as_stream_t;

/// Initialize a stream from a FILE object
/// @param is - uninitialized data, get initialized
/// @param F - already open FILE. You must close it yourself after calling
/// `oc8_as_stream_free`
void oc8_as_stream_init_from_file(oc8_as_stream_t *is, FILE *f);

/// Initialize a stream from a raw bytes array
/// @param is - uninitialized data, get initialized
void oc8_as_stream_init_from_raw(oc8_as_stream_t *is, const void *arr,
                                 size_t len);

/// Release all memory associacted with the stream
/// Doesn't free the stream pointer itself
void oc8_as_stream_free(oc8_as_stream_t *is);

/// Peek at the current char of the stream, doesn't consume it
/// @returns the char, or EOF if none left
int oc8_as_stream_peek(oc8_as_stream_t *is);

/// Read and consume the current char of the stream
/// @return the char, or EOF if none left
int oc8_as_stream_get(oc8_as_stream_t *is);

#ifdef __cplusplus
}
#endif

#endif // !OC8_AS_STREAM_H_
