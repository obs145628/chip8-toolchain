#include "oc8_as/stream.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  oc8_as_functor_fn_f fn;
  FILE *is;
} functor_file_t;

typedef struct {
  oc8_as_functor_fn_f fn;
  const void *arr;
  size_t len;
  size_t idx;
} functor_raw_t;

static int get_byte_fn_file(void *functor_ptr) {
  functor_file_t *f_file = (functor_file_t *)functor_ptr;
  return fgetc(f_file->is);
}

static int get_byte_fn_raw(void *functor_ptr) {
  functor_raw_t *f_raw = (functor_raw_t *)functor_ptr;
  if (f_raw->idx == f_raw->len)
    return EOF;
  else
    return ((char *)f_raw->arr)[f_raw->idx++];
}

static void init_stream(oc8_as_stream_t *is, oc8_as_functor_t *functor) {
  is->getc_fn = functor;
  is->last_def = 0;
}

static void load_last(oc8_as_stream_t *is) {
  if (!is->last_def) {
    is->last = is->getc_fn->fn(is->getc_fn);
    is->last_def = 1;
  }
}

static void clear_last(oc8_as_stream_t *is) {
  if (is->last != EOF)
    is->last_def = 0;
}

void oc8_as_stream_init_from_file(oc8_as_stream_t *is, FILE *f) {
  functor_file_t *functor = (functor_file_t *)malloc(sizeof(functor_file_t));
  functor->fn = get_byte_fn_file;
  functor->is = f;
  init_stream(is, (oc8_as_functor_t *)functor);
}

void oc8_as_stream_init_from_raw(oc8_as_stream_t *is, const void *arr,
                                 size_t len) {
  functor_raw_t *functor = (functor_raw_t *)malloc(sizeof(functor_raw_t));
  functor->fn = get_byte_fn_raw;
  functor->arr = arr;
  functor->len = len;
  functor->idx = 0;
  init_stream(is, (oc8_as_functor_t *)functor);
}

void oc8_as_stream_free(oc8_as_stream_t *is) { free(is->getc_fn); }

int oc8_as_stream_peek(oc8_as_stream_t *is) {
  load_last(is);
  return is->last;
}

int oc8_as_stream_get(oc8_as_stream_t *is) {
  load_last(is);
  int res = is->last;
  clear_last(is);
  return res;
}
