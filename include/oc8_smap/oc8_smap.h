#ifndef OC8_SMAP_OC8_SMAP_H_
#define OC8_SMAP_OC8_SMAP_H_

//===--oc8_smap/oc8_smap.h - oc8_smap_t def -----------------------*- C -*-===//
//
// oc8_is library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct oc8_smap_t
/// Symbol table of type <string, size_t>
///
//===----------------------------------------------------------------------===//

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct oc8_smap_node oc8_smap_node_t;

struct oc8_smap_node {
  oc8_smap_node_t *next;
  size_t val;
  char key[];
};

/// Symbol table <string, size_t>
/// Key is char* (0-terminated)
/// Val is size_t, but often used to store allocated pointers
/// The key is allocated and copied on the Node, and doesn't need to be kept
/// alive by the user
///
/// Implementation based on a linked list
/// All operations are O(n), with n number of items in the table
typedef struct {
  oc8_smap_node_t *head;
  size_t len;
} oc8_smap_t;

/// Iterator to go through all items in the symbol table
/// The order is unspecified
/// Insert / Remove in the map may invalidate the iterator (UB)
typedef struct {
  oc8_smap_node_t *node;
} oc8_smap_it_t;

/// Initialize a new empty symbol table
void oc8_smap_init(oc8_smap_t *sm);

/// Free and destroy all memory associated with the symbol table
void oc8_smap_free(oc8_smap_t *sm);

/// Returns an iterator for the symbol table
/// Points to first item
oc8_smap_it_t oc8_smap_get_it(oc8_smap_t *sm);

/// Look for a key in the symbol table
/// @returns NULL if not found, or a valid pointer to the node if found
oc8_smap_node_t *oc8_smap_find(oc8_smap_t *sm, const char *key);

/// Insert a <key, val> pair in the map
/// If key already exists, update the value
/// Returns 1 if it was an insertion, 0 if it was an update
int oc8_smap_insert(oc8_smap_t *sm, const char *key, size_t val);

/// Remove a key (and it's associated value) from the map, if it exists
/// Returns 1 if the key was found and removed, 0 if not found
int oc8_smap_remove(oc8_smap_t *sm, const char *key);

/// Returns a pointer to the current node of the iterator
/// Or NULL if end reached
static inline oc8_smap_node_t *oc8_smap_it_get(oc8_smap_it_t *it) {
  return it->node;
}

/// Move iterator to next item
static inline void oc8_smap_it_next(oc8_smap_it_t *it) {
  it->node = it->node->next;
}

#ifdef __cplusplus
}
#endif

#endif // !OC8_SMAP_OC8_SMAP_H_
