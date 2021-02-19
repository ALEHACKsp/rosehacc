#pragma once
#include "aura.h"
#include "globals.h"
#include "rose/driver.hpp"

namespace aura {
	class Access {
	public:
		int process_id = 0;
		driver_communication* driver = new driver_communication();

		bool initialize();

		int retrieve_processid(string name);
		HANDLE retrieve_handle(string name);
		uintptr_t retrieve_module(string name);
		HWND retrieve_window(const char* name);

		template<typename T>
		T read(uintptr_t addr) {
			T buffer;

			return driver->read<T>(addr);
		}


		PVOID read_raw(PVOID addr, size_t size) {
			PVOID buffer;

			driver->read_buffer((ULONGLONG)addr, &buffer, size);
			return buffer;
		}

		void read_raw(uintptr_t addr, PVOID buffer, size_t size) {
			driver->read_buffer((ULONGLONG)addr, &buffer, size);
		}

		template<typename T>
		T read_chain(uintptr_t addr, vector<uintptr_t> bytes) {
			T buffer;

			if (buffer = read<T>(addr + bytes[0])) {
				for (int i = 1; i < bytes.size(); i++) {
					auto byte = bytes[i];
					if (buffer = read<T>(buffer + byte))
						continue;

					return NULL;
				}
				return buffer;
			}

			return NULL;
		}

		void write_string(uintptr_t addr, int length, char* string) {
			driver->write_string(addr, string, length);
		}

		void read_string(uintptr_t addr, int length, char* string) {
			driver->read_buffer((ULONGLONG)addr, (LPVOID*)string, length);
		}

		void read_ustring(uintptr_t addr, int length, wchar_t* string) {
			driver->read_buffer((ULONGLONG)addr, (LPVOID*)string, length * 2);
		}

		template<typename T>
		bool write(uintptr_t addr, T value) {
			return driver->write<T>(addr, value);
		}
	};
}



