#include "aura.h"
#include "game.h"
#include "entityloop.h"

void update_localplayer(CachePlayer player) {
	cache->localplayer->class_object = player.class_object;
	cache->localplayer->position = player.position;
	cache->localplayer->base_player = player.base_player;
	cache->localplayer->health = player.health;
	cache->localplayer->name = player.name;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return NULL;
}

namespace aura {
	Access* access = new Access();
	Process* process = new Process();
	Cache* cache = new Cache();
	
	Renderer* renderer = new Renderer();
	Overlay* overlay = new Overlay();
	HackSettings* settings = new HackSettings();

	void Cache::update() {
		for (auto i = 0; i < cache->players.size(); i++) {
			auto& player = cache->players[i];
			if (!player.update())
				cache->players.erase(cache->players.begin() + i);
		}

		for (auto i = 0; i < cache->other.size(); i++) {
			auto& entity = cache->other[i];
			if (!entity.update_ent())
				cache->other.erase(cache->other.begin() + i);
		}
	}

	uintptr_t tables[8] = { NearTable, NearTableSize, FarTable, FarTableSize, FarFarTable, FarFarTableSize, FarFarFarTable, FarFarFarTableSize };
	void Cache::collect() {
		auto world_ptr = access->read<uintptr_t>(process->base_address + World);
		cout << "world_ptr: " << hex << world_ptr << endl;
		cache->localplayer->class_object = access->read_chain<uintptr_t>(world_ptr, { CameraOn, 0x8 });
		cache->localplayer->TeamID = access->read<int>(cache->localplayer->class_object + teamID);

		for (auto current_table = 0; current_table < 7; current_table++) {
			auto current_list = access->read<uintptr_t>(world_ptr + tables[current_table]);
			auto current_list_size = access->read<int>(world_ptr + tables[current_table + 1]);

			if (!current_list || !current_list_size)
				continue;

			cout << "[updating table " << current_table << " ]" << endl
				<< "list: " << hex << current_list << endl
				<< "size: " << dec << current_list_size << endl;

			for (int i = 0; i < current_list_size; i++) {
				if (auto entity_ptr = access->read<uintptr_t>(current_list + (i * 0x8))) {
					auto class_name_ptr = access->read_chain<uintptr_t>(entity_ptr, { 0x150, 0xD0 });
					auto class_name_size = access->read<int>(class_name_ptr + 0x8);

					char class_name[128];
					access->read_string(class_name_ptr + 0x10, class_name_size, class_name);

					CachePlayer entity;
					entity.class_object = entity_ptr;
					entity.position = entity.get_position();
					entity.isDead = entity.getDead();
					entity.TeamID = entity.GetID();
					entity.health = roundf((((entity.get_health()) - 1) * -100));

					cout << "ent->" << hex << entity_ptr << " (class name: " << class_name << ") - position: " << entity.position.x << " " << entity.position.y << " " << entity.position.z << " Team: " << entity.TeamID << " Health: " << entity.health << endl;
					if (strstr(class_name, XorStr("soldier"))) {
						auto exists = false;
						for (auto p : cache->players) {
							if (p.class_object == entity_ptr) {
								exists = true;
								break;
							}
						}

						if (exists)
							continue;

						entity.localplayer = (entity_ptr == cache->localplayer->class_object);
						entity.position = entity.get_position();

						cache->players.push_back(entity);
					} else if (strstr(class_name, XorStr("car")) || strstr(class_name, XorStr("helicopter")) || strstr(class_name, XorStr("tank")) || strstr(class_name, XorStr("boat")) || strstr(class_name, XorStr("airplane"))) {
						auto exists = false;
						for (auto p : cache->other) {
							if (p.class_object == entity_ptr) {
								exists = true;
								break;
							}
						}

						if (exists)
							continue;

						entity.position = entity.get_position();
						entity.name = class_name;
						entity.type = CacheEntityType::VEHICLE;

						cache->other.push_back(entity);
					}
				}
			}

			current_table++;
		}
	}

	void Cache::clear() {
		players.clear();
		other.clear();

		localplayer = new CacheLocalPlayer();
	}

	bool Overlay::create() {
		GetWindowRect(process->window_handle, &process->window_rect);
		process->window_size.x = process->window_rect.right - process->window_rect.left;
		process->window_size.y = process->window_rect.bottom - process->window_rect.top;
		if(settings->misc.console_output)
			cout << XorStr(" - Window Size => Width:") << process->window_size.x << XorStr(" Height: ") << process->window_size.y << endl;

		auto window_title = Helpers::random_string(8);
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetModuleHandleW(NULL);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);
		wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = window_title.c_str();
		wc.hIconSm = NULL;
		RegisterClassEx(&wc);

		if (overlay->handle = CreateWindowExA(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOPMOST, window_title.c_str(), "", WS_POPUP, process->window_rect.left, process->window_rect.top, process->window_size.x, process->window_size.y, NULL, NULL, wc.hInstance, NULL))
			return true;

		return false;
	}
}