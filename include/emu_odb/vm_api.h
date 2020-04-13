#ifndef EMU_ODB_VM_API_H_
#define EMU_ODB_VM_API_H_

//===--emu_odb/vm_api.h - VMApi C implem --------------------------*- C -*-===//
//
// emu_odb library
// Author: Steven Lariau
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of the C API of ODB for VMApi
/// Required to make oc8_emu works with ODB
///
//===----------------------------------------------------------------------===//

#include <odb/server_capi/vm-api.h>

extern odb_vm_api_vtable_t g_oc8_odb_vm_api_table;

/// Must be called to create an argument for the VMApi object
/// Should be called after loading the bin file to emu debug in order to get
/// debug symbols
/// Allocate memory free only by the destructor in `g_oc8_odb_vm_api_table`
odb_vm_api_data_t oc8_odb_vm_make_data();

#endif // !EMU_ODB_VM_API_H_
