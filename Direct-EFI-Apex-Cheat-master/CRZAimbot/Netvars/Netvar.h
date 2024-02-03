#pragma once
#include "fnv.h"
#include "Datatable.h"

#include <unordered_map>

// call once to dump netvars
void SetupNetvars();

// Dump
void Dump(const char* baseClass, RecvTable* table, std::uintptr_t offset = 0);

//hold the offsets
inline std::unordered_map<std::uintptr_t, std::uintptr_t> netvars;

#define NETVAR(func_name, netvar, type) type& func_name() \
{ \
	static auto offset = netvars[fnv::HashConst(netvar)]; \
	return *reinterpret_cast<type*>(std::uintptr_t(this) + offset); \
}