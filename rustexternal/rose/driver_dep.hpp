#pragma once
#include <windows.h>
#include <iostream>
#include <string_view>
#include <string>
#include <tlhelp32.h>
#include <cstdint>

enum type : int
{
	type_init = 0,
	type_module_base,
	type_read,
	type_write,
	type_get_thread_ctx,
	type_set_thread_ctx,
	type_write_protected
};

struct HOLDING_STRUCT
{
	type incoming_type;
};

struct RWMEMORY_STRUCT
{
	type incoming_type;
	void* buffer;
	uintptr_t addr;
	unsigned long long sz;
};

struct BASE_STRUCT
{
	type incoming_type;
	unsigned long long base;
	const char* incoming_module_name;
};

struct INIT_STRUCT
{
	type incoming_type;
	unsigned long pid;
};

struct GENERIC_THREAD_CTX
{
	type incoming_type;
	uint64_t window_handle;
	uint64_t thread_pointer;
};