#include "aura.h"
#include "globals.h"
#include "game.h"
#include "memory.h"
#include "renderer.h"
#include "entityloop.h"
#include "auth.h"
#include <ctime>

void cache_thread();
void cheat_thread();

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	SetConsoleTitleA(Helpers::random_string(12).c_str());

	//auto auth = new Session();
	//auth->handle_login();
	
	cout << XorStr("[+] Loading protection");
	access->driver->set_process("arma3_x64.exe");
	if (!access->driver->initialized) {
		Beep(250, 500);
		return 0;
	}
	cout << XorStr(" Protection loaded.") << endl;

	cout << XorStr("[+] Press the END key to boot the cheat.") << endl;
	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

	Beep(500, 500);

	settings->misc.console_log(XorStr("[+] Initializing cheat..."));
	if (!access->initialize()) {
		MessageBoxA(NULL, XorStr("Initialization failed."), XorStr("Critical Failure"), NULL);
		return 0;
	}

	settings->misc.console_log(XorStr("[+] Creating overlay..."));
	if (!overlay->create()) {
		MessageBoxA(NULL, XorStr("Overlay creation failed."), XorStr("Critical Failure"), NULL);
		return 0;
	}

	settings->misc.console_log(XorStr("[+] Creating DirectX Device..."));
	if (!renderer->create_device(overlay->handle)) {
		renderer->cleanup_device();
		MessageBoxA(NULL, XorStr("DirectX Device creation failed."), XorStr("Critical Failure"), NULL);
		return 0;
	}

	settings->misc.console_log(XorStr("[+] Starting threads..."));
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)cache_thread, NULL, NULL, NULL);         // Updating entities in cache.
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)cheat_thread, NULL, NULL, NULL);         // Cheats are done here.
	cout << XorStr("[+] Hack setup complete!") << endl;

	renderer->render();
}


void cache_thread() {
	auto last_updated = clock();
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(3));

		cache->update();

		auto start = clock();
		if (double(start - last_updated) / CLOCKS_PER_SEC < 3.f)
			continue;

		last_updated = start;
		cache->collect();
	}
}

void cheat_thread() {
	while (true) {
		auto start = clock();
		while (GetAsyncKeyState(VK_END)) {
			if (double(clock() - start) / CLOCKS_PER_SEC > 2.f)
				ExitProcess(0);
		}

		if (GetAsyncKeyState(VK_TAB) & 1)
			settings->aimbot.head = !settings->aimbot.head;

		if (GetAsyncKeyState(settings->misc.menu_key) & 1)
			settings->misc.menu_open = !settings->misc.menu_open;

		std::this_thread::sleep_for(std::chrono::milliseconds(3));

		if (settings->misc.no_clip) {
			if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState(0x57)) { //forward
				auto position = cache->localplayer->get_position();
				if (auto visual_state = access->read<uintptr_t>(cache->localplayer->class_object + 0xD0))
					access->write<D3DXVECTOR3>(visual_state + 0x2C, D3DXVECTOR3((position.x + 0.1f), position.y, position.z));
			}

			if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState(0x53)) { //backwards
				auto position = cache->localplayer->get_position();
				if (auto visual_state = access->read<uintptr_t>(cache->localplayer->class_object + 0xD0))
					access->write<D3DXVECTOR3>(visual_state + 0x2C, D3DXVECTOR3((position.x - 0.1f), position.y, position.z));
			}

			if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState(VK_SPACE)) { //up
				auto position = cache->localplayer->get_position();
				if (auto visual_state = access->read<uintptr_t>(cache->localplayer->class_object + 0xD0))
					access->write<D3DXVECTOR3>(visual_state + 0x2C, D3DXVECTOR3(position.x, (position.y + 0.1f), position.z));
			}

			if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState(0x5A)) { //down
				auto position = cache->localplayer->get_position();
				if (auto visual_state = access->read<uintptr_t>(cache->localplayer->class_object + 0xD0))
					access->write<D3DXVECTOR3>(visual_state + 0x2C, D3DXVECTOR3(position.x, (position.y - 0.1f), position.z));
			}
		}

		if (settings->aimbot.silent) {
			if (GetAsyncKeyState(settings->aimbot.target_hotkey))
				settings->misc.magic_target = sdk::resolve_closest_crosshair(true);

			if (GetAsyncKeyState(settings->aimbot.frame_hotkey))
				settings->aimbot.frame_target = sdk::resolve_closest_crosshair(true);

			if (GetAsyncKeyState(settings->aimbot.hotkey) && settings->misc.magic_target.class_object) {
				if (auto world = access->read<uintptr_t>(process->base_address + World)) {
					if (auto bullet_table = access->read<uintptr_t>(world + bulletTable)) {
						cout << hex << "table " << bullet_table << endl;
						auto bullet_count = access->read<int>(world + bulletTableSize);

						for (auto i = 0; i < bullet_count; i++) {
							auto bullet = access->read<uintptr_t>(bullet_table + (i * 10));
							cout << hex << "bullet " << bullet << endl;
							auto position = D3DXVECTOR3(0.f, 0.f, 0.f);
							if (settings->aimbot.head)
								position = settings->misc.magic_target.get_headposition();
							else if (!settings->aimbot.head)
								position = settings->misc.magic_target.get_bodyposition();
							if (position.x) {
								if (auto visual_state = access->read<uintptr_t>(bullet + 0xD0)) {
									//access->write<DWORD>(bullet + 0x2FC, settings->aimbot.frame_target.class_object);
									access->write<D3DXVECTOR3>(visual_state + 0x2C, position);
								}
							}
						}
					}
				}
			}
		}

		if (settings->aimbot.aim)
			sdk::run_aimbot();

		settings->colors.menu_theme = settings->misc.rgb_theme ? sdk::resolve_rgb_color() : D3DXVECTOR3(settings->colors.menu_color[0] * 255.f, settings->colors.menu_color[1] * 255.f, settings->colors.menu_color[2] * 255.f);
	}
}