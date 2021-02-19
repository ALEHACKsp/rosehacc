#pragma once
#include "driver_dep.hpp"
#include <vector>
#include <unordered_map>
#include <ntstatus.h>

class driver_communication
{
public:
	driver_communication( );
	std::unordered_map<std::string, uint64_t> modules;

public:
	bool initialized = false;

private:
	void send_input( void* incoming_struct );

public:
	bool set_process( std::string_view process_name );

	void store_module( const char* module_name );
	uint64_t retrieve_module( const char* module_name );

	//uint64_t get_module( const char* module_name );

	template <typename t>
	t read( const uint64_t& addr )
	{
		if ( !addr )
			return {};

		t ret;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_read;
		request.addr = addr;
		request.buffer = &ret;
		request.sz = sizeof( t );

		send_input( &request );

		return ret;
	}

	bool read_buffer( const uint64_t& addr, void* buf, size_t sz )
	{
		if ( !addr )
			return false;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_read;
		request.addr = addr;
		request.buffer = buf;
		request.sz = sz;

		send_input( &request );

		return true;
	}

	bool write_string(const uint64_t& addr, char* buf, size_t sz)
	{
		if (!addr)
			return false;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_write;
		request.addr = addr;
		request.buffer = buf;
		request.sz = sz;

		send_input(&request);

		return true;
	}

	bool read_string(const uint64_t& addr, char* buf, size_t sz)
	{
		if (!addr)
			return false;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_read;
		request.addr = addr;
		request.buffer = buf;
		request.sz = sz;

		send_input(&request);

		return true;
	}

	template <typename t>
	bool write( const uint64_t& addr, const t& val )
	{
		if ( !addr )
			return false;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_write;
		request.addr = addr;
		request.buffer = ( void* )&val;
		request.sz = sizeof( t );

		send_input( &request );

		return true;
	}

	template <typename t>
	bool write_protected( const uint64_t& addr, const t& val )
	{
		if ( !addr )
			return false;

		RWMEMORY_STRUCT request;
		request.incoming_type = type_write_protected;
		request.addr = addr;
		request.buffer = ( void* )&val;
		request.sz = sizeof( t );

		send_input( &request );

		return true;
	}

	void get_thread( HWND wnd_handle, uint64_t* thread_ctx );
	void set_thread( HWND wnd_handle, uint64_t thread_ctx );
};