#include <catch2/catch.hpp>

#include "oc8_smap/oc8_smap.h"
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace {

using item_t = std::pair<std::string, std::size_t>;

std::set<std::string> get_keys(oc8_smap_t *sm) {
  std::set<std::string> res;
  auto it = oc8_smap_get_it(sm);
  while (oc8_smap_it_get(&it)) {
    res.insert(oc8_smap_it_get(&it)->key);
    oc8_smap_it_next(&it);
  }
  return res;
}

std::vector<item_t> get_all(oc8_smap_t *sm) {
  auto keys = get_keys(sm);
  std::vector<item_t> res;
  for (const auto &k : keys) {
    auto node = oc8_smap_find(sm, k.c_str());
    REQUIRE(node != nullptr);
    res.emplace_back(k, node->val);
  }
  return res;
}

} // namespace

TEST_CASE("oc8_smap_init_free", "") {
  oc8_smap_t sm;
  oc8_smap_init(&sm);
  REQUIRE(sm.len == 0);
  REQUIRE(get_all(&sm) == std::vector<item_t>{});
  oc8_smap_free(&sm);
}

TEST_CASE("oc8_smap insert one", "") {
  oc8_smap_t sm;
  oc8_smap_init(&sm);
  REQUIRE(oc8_smap_insert(&sm, "abc", 56) == 1);
  REQUIRE(sm.len == 1);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"abc", 56}});
  oc8_smap_free(&sm);
}

TEST_CASE("oc8_smap insert squares", "") {
  oc8_smap_t sm;
  oc8_smap_init(&sm);
  std::vector<item_t> exp;

  for (int i = 0; i < 10; ++i) {
    auto si = std::to_string(i);
    exp.emplace_back(si, i);
    REQUIRE(oc8_smap_insert(&sm, si.c_str(), i) == 1);
  }
  REQUIRE(sm.len == 10);
  REQUIRE(get_all(&sm) == exp);

  for (int i = 0; i < 10; ++i) {
    auto si = std::to_string(i);
    exp[i].second = i * i;
    REQUIRE(oc8_smap_insert(&sm, si.c_str(), i * i) == 0);
  }
  REQUIRE(sm.len == 10);
  REQUIRE(get_all(&sm) == exp);

  oc8_smap_free(&sm);
}

TEST_CASE("oc8_smap insert remove", "") {
  oc8_smap_t sm;
  oc8_smap_init(&sm);

  REQUIRE(oc8_smap_insert(&sm, "abc", 56) == 1);
  REQUIRE(sm.len == 1);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"abc", 56}});

  REQUIRE(oc8_smap_insert(&sm, "brt", 12) == 1);
  REQUIRE(sm.len == 2);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"abc", 56}, {"brt", 12}});

  REQUIRE(oc8_smap_insert(&sm, "abd", 57) == 1);
  REQUIRE(sm.len == 3);
  REQUIRE(get_all(&sm) ==
          std::vector<item_t>{{"abc", 56}, {"abd", 57}, {"brt", 12}});

  REQUIRE(oc8_smap_insert(&sm, "abd", 58) == 0);
  REQUIRE(sm.len == 3);
  REQUIRE(get_all(&sm) ==
          std::vector<item_t>{{"abc", 56}, {"abd", 58}, {"brt", 12}});

  REQUIRE(oc8_smap_remove(&sm, "abd-") == 0);
  REQUIRE(sm.len == 3);
  REQUIRE(get_all(&sm) ==
          std::vector<item_t>{{"abc", 56}, {"abd", 58}, {"brt", 12}});

  REQUIRE(oc8_smap_remove(&sm, "abd") == 1);
  REQUIRE(sm.len == 2);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"abc", 56}, {"brt", 12}});

  REQUIRE(oc8_smap_insert(&sm, "brt", 24) == 0);
  REQUIRE(sm.len == 2);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"abc", 56}, {"brt", 24}});

  REQUIRE(oc8_smap_insert(&sm, "nnn", 111) == 1);
  REQUIRE(sm.len == 3);
  REQUIRE(get_all(&sm) ==
          std::vector<item_t>{{"abc", 56}, {"brt", 24}, {"nnn", 111}});

  REQUIRE(oc8_smap_remove(&sm, "abc") == 1);
  REQUIRE(sm.len == 2);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"brt", 24}, {"nnn", 111}});

  REQUIRE(oc8_smap_remove(&sm, "nnn") == 1);
  REQUIRE(sm.len == 1);
  REQUIRE(get_all(&sm) == std::vector<item_t>{{"brt", 24}});

  REQUIRE(oc8_smap_remove(&sm, "brt") == 1);
  REQUIRE(sm.len == 0);
  REQUIRE(get_all(&sm) == std::vector<item_t>{});

  oc8_smap_free(&sm);
}
