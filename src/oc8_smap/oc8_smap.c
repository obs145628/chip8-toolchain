#include "oc8_smap/oc8_smap.h"
#include <stdlib.h>
#include <string.h>

void oc8_smap_init(oc8_smap_t *sm) {
  sm->head = NULL;
  sm->len = 0;
}

void oc8_smap_free(oc8_smap_t *sm) {
  oc8_smap_node_t *node = sm->head;
  while (node) {
    oc8_smap_node_t *next = node->next;
    free(node);
    node = next;
  }
}

oc8_smap_it_t oc8_smap_get_it(oc8_smap_t *sm) {
  oc8_smap_it_t res;
  res.node = sm->head;
  return res;
}

oc8_smap_node_t *oc8_smap_find(oc8_smap_t *sm, const char *key) {
  oc8_smap_node_t *node = sm->head;
  while (node) {
    if (strcmp(node->key, key) == 0)
      return node;
    node = node->next;
  }
  return NULL;
}

/// Insert a <key, val> pair in the map
/// If key already exists, update the value
/// Returns 1 if it was an insertion, 0 if it was an update
int oc8_smap_insert(oc8_smap_t *sm, const char *key, size_t val) {
  oc8_smap_node_t *node = oc8_smap_find(sm, key);
  if (node) {
    node->val = val;
    return 0;
  }

  size_t key_len = strlen(key);
  node = malloc(sizeof(oc8_smap_node_t) + key_len + 1);
  node->next = sm->head;
  node->val = val;
  memcpy(node->key, key, key_len + 1);
  sm->head = node;
  ++sm->len;
  return 1;
}

/// Remove a key (and it's associated value) from the map, if it exists
/// Returns 1 if the key was found and removed, 0 if not found
int oc8_smap_remove(oc8_smap_t *sm, const char *key) {
  oc8_smap_node_t *node = sm->head;
  if (!node) // Special :Empty list
    return 0;
  if (strcmp(node->key, key) == 0) { // Special: key at the front
    sm->head = node->next;
    free(node);
    --sm->len;
    return 1;
  }

  while (node->next) {
    oc8_smap_node_t *next = node->next;
    if (strcmp(next->key, key) != 0) {
      node = next;
      continue;
    }

    // Key found, remove node
    node->next = next->next;
    free(next);
    --sm->len;
    return 1;
  }

  return 0;
}
