#include <catch2/catch.hpp>

#include <odb/server/debugger.hh>
#include <odb/server_capi/vm-api.h>

#include "test_utils.hh"

namespace {

void init_emu(oc8_bin_file_t *bf) {
  oc8_emu_init();
  oc8_emu_load_bin(bf);
}

inline std::uint16_t db_get_reg(odb::Debugger &db, odb::vm_reg_t idx) {
  std::uint16_t res = 0;
  db.get_reg(idx, reinterpret_cast<std::uint8_t *>(&res));
  return res;
}

inline void db_set_reg(odb::Debugger &db, odb::vm_reg_t idx,
                       std::uint16_t val) {
  db.set_reg(idx, reinterpret_cast<std::uint8_t *>(&val));
}

inline std::uint16_t db_read_u16(odb::Debugger &db, std::size_t addr) {
  std::uint16_t res;
  db.read_mem(addr, 2, reinterpret_cast<std::uint8_t *>(&res));
  return res;
}

inline void db_write_u16(odb::Debugger &db, std::size_t addr,
                         std::uint16_t val) {
  db.write_mem(addr, 2, reinterpret_cast<std::uint8_t *>(&val));
}

inline void db_resume(odb::Debugger &db, odb::ResumeType type) {
  db.resume(type);
  while (db.get_state() != odb::Debugger::State::EXIT &&
         db.get_state() != odb::Debugger::State::ERROR &&
         db.get_state() != odb::Debugger::State::STOPPED) {
    oc8_emu_cpu_step();
    db.on_update();
  }
}

const char *regs_names[] = {"v0", "v1", "v2", "v3", "v4", "v5", "v6",
                            "v7", "v8", "v9", "va", "vb", "vc", "vd",
                            "ve", "vf", "pc", "i",  "sp", "dt", "st"};

} // namespace

TEST_CASE("call_add_mem infos", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);

  for (std::size_t i = 0; i < 16; ++i)
    REQUIRE(db_get_reg(db, i) == 0);
  REQUIRE(db_get_reg(db, 16) == 0x200);
  for (std::size_t i = 17; i < 21; ++i)
    REQUIRE(db_get_reg(db, i) == 0);
  REQUIRE_THROWS(db_get_reg(db, 21));
  REQUIRE_THROWS(db_get_reg(db, 22));
  REQUIRE_THROWS(db_get_reg(db, -1));

  for (std::size_t i = 0; i < 16; ++i) {
    auto infos = db.get_reg_infos(i);
    REQUIRE(infos.idx == i);
    REQUIRE(infos.name == regs_names[i]);
    REQUIRE(infos.size == 1);
    REQUIRE(infos.kind == odb::RegKind::general);
  }

  auto infos_pc = db.get_reg_infos(16);
  REQUIRE(infos_pc.idx == 16);
  REQUIRE(infos_pc.name == "pc");
  REQUIRE(infos_pc.size == 2);
  REQUIRE(infos_pc.kind == odb::RegKind::program_counter);

  auto infos_i = db.get_reg_infos(17);
  REQUIRE(infos_i.idx == 17);
  REQUIRE(infos_i.name == "i");
  REQUIRE(infos_i.size == 2);
  REQUIRE(infos_i.kind == odb::RegKind::general);

  auto infos_sp = db.get_reg_infos(18);
  REQUIRE(infos_sp.idx == 18);
  REQUIRE(infos_sp.name == "sp");
  REQUIRE(infos_sp.size == 1);
  REQUIRE(infos_sp.kind == odb::RegKind::stack_pointer);

  auto infos_dt = db.get_reg_infos(19);
  REQUIRE(infos_dt.idx == 19);
  REQUIRE(infos_dt.name == "dt");
  REQUIRE(infos_dt.size == 1);
  REQUIRE(infos_dt.kind == odb::RegKind::general);

  auto infos_st = db.get_reg_infos(20);
  REQUIRE(infos_st.idx == 20);
  REQUIRE(infos_st.name == "st");
  REQUIRE(infos_st.size == 1);
  REQUIRE(infos_st.kind == odb::RegKind::general);

  REQUIRE_THROWS(db.get_reg_infos(21));
  REQUIRE_THROWS(db.get_reg_infos(22));
  REQUIRE_THROWS(db.get_reg_infos(-1));

  for (size_t i = 0; i < 21; ++i)
    REQUIRE(db.find_reg_id(regs_names[i]) == i);
  REQUIRE_THROWS(db.find_reg_id(""));
  REQUIRE_THROWS(db.find_reg_id("v"));
  REQUIRE_THROWS(db.find_reg_id("v50"));
  REQUIRE_THROWS(db.find_reg_id("pop"));
  REQUIRE(db.registers_count() == 21);

  REQUIRE(db.list_regs(odb::RegKind::general) ==
          std::vector<odb::vm_reg_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                                     13, 14, 15, 17, 19, 20});
  REQUIRE(db.list_regs(odb::RegKind::stack_pointer) ==
          std::vector<odb::vm_reg_t>{18});
  REQUIRE(db.list_regs(odb::RegKind::program_counter) ==
          std::vector<odb::vm_reg_t>{16});
  REQUIRE(db.list_regs(odb::RegKind::flags) == std::vector<odb::vm_reg_t>{});
  REQUIRE(db.list_regs(odb::RegKind::base_pointer) ==
          std::vector<odb::vm_reg_t>{});

  REQUIRE(db.get_memory_size() == 4096);
  REQUIRE(db.pointer_size() == 2);
  REQUIRE(db.integer_size() == 1);
  REQUIRE(db.use_opcode() == true);

  REQUIRE(db.get_symbols(0, 56) == std::vector<odb::vm_sym_t>{});
  REQUIRE(db.get_symbols(512, 2) == std::vector<odb::vm_sym_t>{0});
  REQUIRE(db.get_symbols(512, 2) == std::vector<odb::vm_sym_t>{0});
  REQUIRE(db.get_symbols(512, 4) == std::vector<odb::vm_sym_t>{0, 1});
  REQUIRE(db.get_symbols(599, 200) == std::vector<odb::vm_sym_t>{});
  REQUIRE(db.get_symbols(512, 100) == std::vector<odb::vm_sym_t>{0, 1, 3, 2});

  auto sym0 = db.get_symbol_infos(0);
  REQUIRE(sym0.idx == 0);
  REQUIRE(sym0.name == "_rom_begin");
  REQUIRE(sym0.addr == 512);

  auto sym1 = db.get_symbol_infos(1);
  REQUIRE(sym1.idx == 1);
  REQUIRE(sym1.name == "my_add");
  REQUIRE(sym1.addr == 514);

  auto sym2 = db.get_symbol_infos(2);
  REQUIRE(sym2.idx == 2);
  REQUIRE(sym2.name == "args");
  REQUIRE(sym2.addr == 526);

  auto sym3 = db.get_symbol_infos(3);
  REQUIRE(sym3.idx == 3);
  REQUIRE(sym3.name == "_start");
  REQUIRE(sym3.addr == 518);

  REQUIRE_THROWS(db.get_symbol_infos(4));
  REQUIRE_THROWS(db.get_symbol_infos(5));
  REQUIRE_THROWS(db.get_symbol_infos(-1));

  REQUIRE(db.get_symbol_at(512) == 0);
  REQUIRE(db.get_symbol_at(513) == odb::VM_SYM_NULL);
  REQUIRE(db.get_symbol_at(514) == 1);
  REQUIRE(db.get_symbol_at(516) == odb::VM_SYM_NULL);
  REQUIRE(db.get_symbol_at(518) == 3);
  REQUIRE(db.get_symbol_at(520) == odb::VM_SYM_NULL);
  REQUIRE(db.get_symbol_at(526) == 2);
  REQUIRE(db.get_symbol_at(19) == odb::VM_SYM_NULL);
  REQUIRE(db.get_symbol_at(0) == odb::VM_SYM_NULL);

  REQUIRE(db.find_sym_id("_rom_begin") == 0);
  REQUIRE(db.find_sym_id("my_add") == 1);
  REQUIRE(db.find_sym_id("args") == 2);
  REQUIRE(db.find_sym_id("_start") == 3);
  REQUIRE_THROWS(db.find_sym_id("__start"));
  REQUIRE_THROWS(db.find_sym_id("_start_"));
  REQUIRE_THROWS(db.find_sym_id(""));
  REQUIRE_THROWS(db.find_sym_id("nop"));
  REQUIRE(db.symbols_count() == 4);

  odb::vm_size_t un;
  REQUIRE(db.get_code_text(0, un) == "");
  REQUIRE(db.get_code_text(510, un) == "");
  REQUIRE(db.get_code_text(512, un) == "jmp <{3}>");
  REQUIRE(db.get_code_text(514, un) == "add %v1, %v0");
  REQUIRE(db.get_code_text(516, un) == "ret");
  REQUIRE(db.get_code_text(518, un) == "mov <{2}>, %i");
  REQUIRE(db.get_code_text(520, un) == "movm %i, %v1");
  REQUIRE(db.get_code_text(522, un) == "call <{1}>");
  REQUIRE(db.get_code_text(524, un) == "mov %v0, %vf");
  REQUIRE(un == 2);
  REQUIRE(db.get_code_text(526, un) == ".word 0x0D08");
  REQUIRE(un == 2);
  REQUIRE(db.get_code_text(528, un) == "");
  REQUIRE(db.get_code_text(529, un) == "");
  REQUIRE(db.get_code_text(530, un) == "");
  REQUIRE(db.get_code_text(4095, un) == "");
  REQUIRE_THROWS(db.get_code_text(4096, un));
  REQUIRE_THROWS(db.get_code_text(4097, un));
  REQUIRE_THROWS(db.get_code_text(-1, un));

  REQUIRE(db.get_execution_point() == 512);

  REQUIRE(db_read_u16(db, 526) == 0x0D08);
  REQUIRE(db_read_u16(db, 510) == 0x0000);
  REQUIRE(db_read_u16(db, 514) == 0x1480);
  REQUIRE(db_read_u16(db, 516) == 0xEE00);
  REQUIRE(db_read_u16(db, 4094) == 0);
  REQUIRE_THROWS(db_read_u16(db, 4095));
  REQUIRE_THROWS(db_read_u16(db, 4096));
  REQUIRE_THROWS(db_read_u16(db, -1));
}

TEST_CASE("call_add_mem exec_step", "") {
  oc8_bin_file_t bf;
  odb::vm_size_t un;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);

  auto cs = db.get_call_stack();
  REQUIRE(cs.size() == 1);
  REQUIRE(cs[0].caller_start_addr == 512);
  REQUIRE(db.get_execution_point() == 512);
  REQUIRE(db.get_code_text(512, un) == "jmp <{3}>");

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 518);
  REQUIRE(db.get_code_text(518, un) == "mov <{2}>, %i");
  REQUIRE(db_get_reg(db, 17) == 0);
  REQUIRE(db_get_reg(db, 16) == 518);
  REQUIRE(db_get_reg(db, 18) == 0);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 520);
  REQUIRE(db.get_code_text(520, un) == "movm %i, %v1");
  REQUIRE(db_get_reg(db, 17) == 526);
  REQUIRE(db_get_reg(db, 0) == 0);
  REQUIRE(db_get_reg(db, 1) == 0);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 522);
  REQUIRE(db.get_code_text(522, un) == "call <{1}>");
  REQUIRE(db_get_reg(db, 0) == 8);
  REQUIRE(db_get_reg(db, 1) == 13);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 514);
  REQUIRE(db.get_code_text(514, un) == "add %v1, %v0");
  cs = db.get_call_stack();
  REQUIRE(cs.size() == 2);
  REQUIRE(cs[1].caller_start_addr == 514);
  REQUIRE(cs[0].call_addr == 522);
  REQUIRE(cs[0].caller_start_addr == 512);
  REQUIRE(db_get_reg(db, 18) == 1);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 516);
  REQUIRE(db.get_code_text(516, un) == "ret");
  REQUIRE(db_get_reg(db, 0) == 21);
  REQUIRE(db_get_reg(db, 1) == 13);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 524);
  REQUIRE(db.get_code_text(524, un) == "mov %v0, %vf");
  REQUIRE(db_get_reg(db, 0xf) == 0);
  cs = db.get_call_stack();
  REQUIRE(cs.size() == 1);
  REQUIRE(cs[0].caller_start_addr == 512);

  db_resume(db, odb::ResumeType::Step);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 526);
  REQUIRE(db.get_code_text(526, un) == ".word 0x0D08");
  REQUIRE(db_get_reg(db, 0xf) == 21);
}

TEST_CASE("call_add_mem exec_once", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);
  REQUIRE(db.get_execution_point() == 512);

  db.add_breakpoint(526);
  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 526);
  REQUIRE(db_get_reg(db, 0xf) == 21);
}

TEST_CASE("call_add_mem exec_change_mem", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);
  REQUIRE(db.get_execution_point() == 512);

  db_write_u16(db, 526, 0x3C1F); // 60 & 31
  db.add_breakpoint(526);

  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 526);
  REQUIRE(db_get_reg(db, 0) == 91);
  REQUIRE(db_get_reg(db, 1) == 60);
  REQUIRE(db_get_reg(db, 0xf) == 91);
}

TEST_CASE("call_add_mem exec_change_reg", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);
  REQUIRE(db.get_execution_point() == 512);

  db.add_breakpoint(522);
  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 522);
  REQUIRE(db_get_reg(db, 0) == 8);
  REQUIRE(db_get_reg(db, 1) == 13);

  db_set_reg(db, 1, 19);
  db_resume(db, odb::ResumeType::StepOver);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 524);
  REQUIRE(db_get_reg(db, 0) == 27);
  REQUIRE(db_get_reg(db, 1) == 19);
}

TEST_CASE("call_add_mem exec_change_opcodes", "") {
  oc8_bin_file_t bf;
  compile_str({test_my_add_src, test_call_add_mem_src}, &bf);
  init_emu(&bf);

  odb::Debugger db(
      odb::make_cpp_vm_api(&g_oc8_odb_vm_api_table, oc8_odb_vm_make_data()));
  db.on_init();
  REQUIRE(db.get_state() == odb::Debugger::State::RUNNING_TOFINISH);
  REQUIRE(db.get_execution_point() == 512);

  auto ret_op = db_read_u16(db, 516);
  auto call_op = db_read_u16(db, 522);
  db_write_u16(db, 516, call_op);
  db.add_breakpoint(516);

  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 516);
  REQUIRE(db_get_reg(db, 0) == 21);
  REQUIRE(db.get_call_stack().size() == 2);

  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 516);
  REQUIRE(db_get_reg(db, 0) == 34);
  REQUIRE(db.get_call_stack().size() == 3);

  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 516);
  REQUIRE(db_get_reg(db, 0) == 47);
  auto cs = db.get_call_stack();
  REQUIRE(cs.size() == 4);
  REQUIRE(cs[3].caller_start_addr == 514);
  REQUIRE(cs[2].caller_start_addr == 514);
  REQUIRE(cs[2].call_addr == 516);
  REQUIRE(cs[1].caller_start_addr == 514);
  REQUIRE(cs[1].call_addr == 516);
  REQUIRE(cs[0].call_addr == 522);
  REQUIRE(cs[0].caller_start_addr == 512);

  db_write_u16(db, 516, ret_op);
  db_write_u16(db, 518, ret_op);
  db.del_breakpoint(516);
  db.add_breakpoint(526);

  db_resume(db, odb::ResumeType::Continue);
  REQUIRE(db.get_state() == odb::Debugger::State::STOPPED);
  REQUIRE(db.get_execution_point() == 526);
  REQUIRE(db_get_reg(db, 0) == 47);
  cs = db.get_call_stack();
  REQUIRE(cs.size() == 1);
  REQUIRE(cs[0].call_addr == 522);
  REQUIRE(cs[0].caller_start_addr == 512);
}
