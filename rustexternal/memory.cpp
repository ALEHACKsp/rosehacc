#include "aura.h"
#include "memory.h"
#include <tchar.h>

#define PROCESS_NAME XorStr("arma3_x64.exe")

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam) {
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id)
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window(unsigned long process_id) {
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}

using namespace aura;
bool Access::initialize() {
	auto start = clock();
	settings->misc.console_log(XorStr(" - Looking for arma3_x64.exe..."));
	while (process->id == 0) {
		process->id = retrieve_processid(PROCESS_NAME);

		if (double(clock() - start) / CLOCKS_PER_SEC > 5.f)
			return false;
	}

	settings->misc.console_log(XorStr(" - Looking for arma window..."));
	process->window_handle = find_main_window(process->id);
	if (!process->window_handle)
		return false;

	GetWindowRect(process->window_handle, &process->window_rect);
	process->window_size.x = process->window_rect.right - process->window_rect.left;
	process->window_size.y = process->window_rect.bottom - process->window_rect.top;

	//process->handle = retrieve_handle(PROCESS_NAME);
	driver->store_module(PROCESS_NAME);
	process->base_address = driver->retrieve_module(PROCESS_NAME);

	process_id = process->id;

	if (settings->misc.console_output) {
		cout << XorStr(" [ Process ]\n  - arma3_x64.exe: 0x") << hex << process->base_address << endl
			<< XorStr("  - Process ID: ") << process->id << endl
			<< XorStr("  - Game Handle: 0x") << hex << process->handle << endl;
	}

	return true;
}

int Access::retrieve_processid(std::string name) {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process_entry;
	ZeroMemory(&process_entry, sizeof(process_entry));
	process_entry.dwSize = sizeof(process_entry);

	if (Process32First(snapshot, &process_entry)) {
		do {
			if (std::string(process_entry.szExeFile) == name) {
				CloseHandle(snapshot);
				return process_entry.th32ProcessID;
			}
		} while (Process32Next(snapshot, &process_entry));
	}

	return 0;
}

HWND Access::retrieve_window(const char* name) {
	return FindWindowA(NULL, name);
}
