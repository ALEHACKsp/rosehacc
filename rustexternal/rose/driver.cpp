#include "driver.hpp"

void* ( __stdcall* NtConvertBetweenAuxiliaryCounterAndPerformanceCounter )( void*, void*, void*, void* );

driver_communication driver_communication::dc;

void driver_communication::send_input( void* incoming_struct )
{
	void* status = nullptr;
	NtConvertBetweenAuxiliaryCounterAndPerformanceCounter( 0, &incoming_struct, &status, 0 );
	//return STATUS_SUCCESS;
	//return static_cast< NTSTATUS >( status );
}

driver_communication::driver_communication( )
{
	auto module = LoadLibrary( "ntdll.dll" );
	if ( !module )
	{
		printf( "ntdll not found\n" );
		return;
	}

	*reinterpret_cast< PVOID* >( &NtConvertBetweenAuxiliaryCounterAndPerformanceCounter ) = GetProcAddress( module, "NtConvertBetweenAuxiliaryCounterAndPerformanceCounter" );
	if ( !NtConvertBetweenAuxiliaryCounterAndPerformanceCounter )
	{
		printf( "failed to locate key function\n" );
		return;
	}
}

bool driver_communication::set_process( std::string_view process_name )
{
	const auto snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( snap == INVALID_HANDLE_VALUE )
		return false;

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof( PROCESSENTRY32 );

	for ( bool success = Process32First( snap, &pe ); success == TRUE; success = Process32Next( snap, &pe ) )
	{
		if ( strcmp( process_name.data( ), pe.szExeFile ) == 0 )
		{
			CloseHandle( snap );

			INIT_STRUCT init = { type_init,  pe.th32ProcessID };
			send_input( &init );

			initialized = true;
			return true;
		}
	}

	CloseHandle( snap );

	return false;
}

void driver_communication::store_module( const char* module_name )
{
	BASE_STRUCT base;
	base.incoming_type = type_module_base;
	base.incoming_module_name = module_name;
	send_input( &base );

	modules[ module_name ] = base.base;
}

uint64_t driver_communication::retrieve_module( const char* module_name )
{
	return modules[ module_name ];
}

//uint64_t driver_communication::get_module( const char* module_name )
//{
//	BASE_STRUCT base;
//	base.incoming_type = type_module_base;
//	base.incoming_module_name = module_name;
//	send_input( &base );
//
//	return base.base;
//}

void driver_communication::get_thread( HWND wnd_handle, uint64_t* thread_ctx )
{
	GENERIC_THREAD_CTX request{};

	request.incoming_type = type_get_thread_ctx;
	request.window_handle = reinterpret_cast< uint64_t >( wnd_handle );
	request.thread_pointer = 0;

	send_input( &request );

	*thread_ctx = request.thread_pointer;
}

void driver_communication::set_thread( HWND wnd_handle, uint64_t thread_ctx )
{
	GENERIC_THREAD_CTX request{};

	request.incoming_type = type_set_thread_ctx;
	request.window_handle = reinterpret_cast< uint64_t >( wnd_handle );
	request.thread_pointer = thread_ctx;

	send_input( &request );
}