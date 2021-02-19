#include "aura.h"
#include "game.h"
#include "globals.h"
#include "renderer.h"
#include "offsets.h"
#include <fstream>

bool sort_color(CacheEntity* entity) {
	switch (entity->type) {
	default:
		entity->name = XorStr("Unknown Object");
		return false;
	}
}

bool CacheEntity::update_ent() {
	if (!class_object)
		return false;

	position = get_position();
	distance = sdk::get_distance(cache->localplayer->get_position(), position);

	if (!position.x)
		return false;

	if (type == CacheEntityType::VEHICLE) {
		if (settings->visuals.teamVicColors) {
			int id = CacheEntity::GetID();
			if (id == 3)
				color = D3DCOLOR_RGBA((int)round(settings->colors.civilianPlayer[0] * 255.f), (int)round(settings->colors.civilianPlayer[1] * 255.f), (int)round(settings->colors.civilianPlayer[2] * 255.f), 255);
			if (id == 2)
				color = D3DCOLOR_RGBA((int)round(settings->colors.independantPlayer[0] * 255.f), (int)round(settings->colors.independantPlayer[1] * 255.f), (int)round(settings->colors.independantPlayer[2] * 255.f), 255);
			if (id == 1)
				color = D3DCOLOR_RGBA((int)round(settings->colors.blueforPlayer[0] * 255.f), (int)round(settings->colors.blueforPlayer[1] * 255.f), (int)round(settings->colors.blueforPlayer[2] * 255.f), 255);
			if (id == 0)
				color = D3DCOLOR_RGBA((int)round(settings->colors.opforPlayer[0] * 255.f), (int)round(settings->colors.opforPlayer[1] * 255.f), (int)round(settings->colors.opforPlayer[2] * 255.f), 255);
		}
		else {
			string name = CacheEntity::name;
			if (strstr(name.c_str(), XorStr("car")))
				color = D3DCOLOR_RGBA((int)round(settings->colors.car[0] * 255.f), (int)round(settings->colors.car[1] * 255.f), (int)round(settings->colors.car[2] * 255.f), 255);
			if (strstr(name.c_str(), XorStr("helicopter")))
				color = D3DCOLOR_RGBA((int)round(settings->colors.helicopter[0] * 255.f), (int)round(settings->colors.helicopter[1] * 255.f), (int)round(settings->colors.helicopter[2] * 255.f), 255);
			if (strstr(name.c_str(), XorStr("tank")))
				color = D3DCOLOR_RGBA((int)round(settings->colors.tank[0] * 255.f), (int)round(settings->colors.tank[1] * 255.f), (int)round(settings->colors.tank[2] * 255.f), 255);
			if (strstr(name.c_str(), XorStr("boat")))
				color = D3DCOLOR_RGBA((int)round(settings->colors.boat[0] * 255.f), (int)round(settings->colors.boat[1] * 255.f), (int)round(settings->colors.boat[2] * 255.f), 255);
			if (strstr(name.c_str(), XorStr("airplane")))
				color = D3DCOLOR_RGBA((int)round(settings->colors.airplane[0] * 255.f), (int)round(settings->colors.airplane[1] * 255.f), (int)round(settings->colors.airplane[2] * 255.f), 255);
		}
	}

	return true;
}

bool CachePlayer::update() {
	if (!class_object)
		return false;

	health = roundf((((get_health()) - 1) * -100));
	position = get_position();
	head_position = get_headposition();
	body_position = get_bodyposition();
	isDead = getDead();

	if (!position.x || !head_position.x)
		return false;

	distance = sdk::get_distance(cache->localplayer->get_position(), position);

	if (CachePlayer::TeamID == 3)
		color = D3DCOLOR_RGBA((int)round(settings->colors.civilianPlayer[0] * 255.f), (int)round(settings->colors.civilianPlayer[1] * 255.f), (int)round(settings->colors.civilianPlayer[2] * 255.f), 255);
	if (CachePlayer::TeamID == 2)
		color = D3DCOLOR_RGBA((int)round(settings->colors.independantPlayer[0] * 255.f), (int)round(settings->colors.independantPlayer[1] * 255.f), (int)round(settings->colors.independantPlayer[2] * 255.f), 255);
	if (CachePlayer::TeamID == 1)
		color = D3DCOLOR_RGBA((int)round(settings->colors.blueforPlayer[0] * 255.f), (int)round(settings->colors.blueforPlayer[1] * 255.f), (int)round(settings->colors.blueforPlayer[2] * 255.f), 255);
	if (CachePlayer::TeamID == 0)
		color = D3DCOLOR_RGBA((int)round(settings->colors.opforPlayer[0] * 255.f), (int)round(settings->colors.opforPlayer[1] * 255.f), (int)round(settings->colors.opforPlayer[2] * 255.f), 255);

	if(class_object == settings->misc.magic_target.class_object)
		color = D3DCOLOR_RGBA((int)round(settings->colors.magic_target[0] * 255.f), (int)round(settings->colors.magic_target[1] * 255.f), (int)round(settings->colors.magic_target[2] * 255.f), 255);

	return true;
}

string CachePlayer::get_name() {
	if (auto name_ptr = access->read_chain<uintptr_t>(class_object, { 0x10, 0x520 })){
		char class_name[128];
		access->read_string(name_ptr, 12, class_name);

		return class_name;
	}

	return "player";
}

D3DXVECTOR3 CacheEntity::get_position() {
	if (auto visual_state = access->read<uintptr_t>(class_object + 0x190))
		return access->read<D3DXVECTOR3>(visual_state + 0x2C);

	return D3DXVECTOR3(0.f, 0.f, 0.f);
}

D3DXVECTOR3 CachePlayer::get_headposition() {
	if (auto visual_state = access->read<uintptr_t>(class_object + 0x190))
		return access->read<D3DXVECTOR3>(visual_state + 0x168);

	return D3DXVECTOR3(0.f, 0.f, 0.f);
}

D3DXVECTOR3 CachePlayer::get_bodyposition() {
	if (auto visual_state = access->read<uintptr_t>(class_object + 0x190))
		return access->read<D3DXVECTOR3>(visual_state + 0x174);

	return D3DXVECTOR3(0.f, 0.f, 0.f);
}

bool CachePlayer::getDead() {
	return access->read<bool>(class_object + IsDead);
}

int CachePlayer::GetID() {
	return access->read<int>(class_object + teamID);
}

int CachePlayer::GetItemUID() {
	return access->read<int>(class_object + 0x0018);
}

int CacheEntity::GetID() {
	return access->read<int>(class_object + teamID);
}

float CacheEntity::get_health() {
	return access->read<float>(class_object + 0xF0);
}

void sdk::kill_scriptrestrictions() {
	if (auto network_manager = access->read<uintptr_t>(process->base_address + NetworkManager)) {
		if (auto network_client = access->read<uintptr_t>(network_manager + 0x48)) {
			if (auto callback = access->read<uintptr_t>(network_client + 0x598)) {
				auto patch_state = access->read<BYTE>(callback);
				if (patch_state != 0xC3) {
					auto helper_callback = access->read<uintptr_t>(network_client + 0x588);
					if (!helper_callback)
						return;

					for (auto i = 0; i < 20; i++) {
						auto temp_helper_callback = helper_callback - 20 + i;
						auto op_code = access->read<BYTE>(temp_helper_callback);
						if (op_code == 0xC3) {
							access->write<uintptr_t>(network_client + 0x598, temp_helper_callback);
							MessageBoxA(NULL, XorStr("Restrictions killed."), "", NULL);
						}
					}
				}
			}
		}
	}
}

const char* blacklisted[30] = { "infiSTAR", "cheat", "hacker", "spanker", "faggot", "menucheat", "createTask = 'STR';", "(findDisplay 46)closeDisplay 0", "BAN", "KICK", "LOOP", "(findDisplay 0)closeDisplay 0", "illegal", "_log", "RANDVAR", "_worldspace = player getVariable['AHworldspace',[]];", "PVAHR_0", "_puid = _this select 0;_name = _this select 1;", "_btnSave ctrlSetText 'PlayerUID:';", "AH", "SPY-GLASS", "he/she was trying to access commanding menu", "SPYGLASS-FLAG", "SpyGlass", "SPY_fnc_notifyAdmins", "SPY_fnc_cookieJar", "MenuBasedHack", "Cheater Flagged","STAR","infi"  };
void sdk::kill_infistar() {
	if (auto world = access->read<uintptr_t>(process->base_address + World)) {
		if (auto script_vm = access->read<uintptr_t>(world + scriptVM)) {
			auto script_count = access->read<int>(world + scriptVMSize);

			for (auto i = 0; i < script_count; i++) {
				auto thread = access->read<uintptr_t>(script_vm + (i * 10));
				auto content_ptr = access->read<uintptr_t>(thread + 0x488);

				char content[4096];
				access->read_string(content_ptr + 0x10, access->read<int>(content_ptr + 0x8), content);

				cout << endl << endl << endl << content << endl << endl << endl;

				auto contains = false;
				for (auto blacklisted_word : blacklisted) {
					if (strstr(content, blacklisted_word)) {
						contains = true;
						break;
					}

				}

				if (contains && access->write<bool>(thread + 0x508, true))
					MessageBoxA(NULL, "Infistar Killed.", "", NULL);
			}
		}
	}
}

void sdk::inject_script(char* script) {
	strcat(script, ";player removeEventHandler [\"AnimChanged\", 0]; player removealleventhandlers \"AnimChanged\";");
	if (auto world_ptr = access->read<uintptr_t>(process->base_address + World)) {
		if (auto localplayer = access->read_chain<uintptr_t>(world_ptr, { CameraOn, 0x8 })) {
			if (auto eh_table = access->read<uintptr_t>(localplayer + EventHandlerTable)) {
				cout << "table: " << hex << eh_table << endl;
				auto eh_size = access->read<int>(localplayer + EventHandlerSize);
				for (int i = 0; i < 1; i++) {
					auto idx = eh_table + (i * 0x28);
					if (auto container = access->read<uintptr_t>(idx + 0x10)) {
						auto container_size = access->read<int>(container + 0x8);
						auto container_script = access->read<uintptr_t>(container + 0x10);

						char original_script[64000];
						access->driver->read_string(container + 0x10, original_script, container_size);
						
						cout << "[ SCRIPT INJECTION @ " << hex << container_script << " ]\nOriginal: " << original_script << "\n\nInjecting: " << script << endl;
						access->write<int>(container_script + 0x8, strlen(script));
						access->driver->write_string(container_script + 0x10, script, strlen(script));
						
						access->write<int>(eh_table + 0x8, 21);
					}
				}
			}
		}
	}
}

float sdk::get_distance(D3DXVECTOR3 start, D3DXVECTOR3 end) {
	return abs(D3DXVec3Length(&(start - end)));
}

bool sdk::in_game() {
	return true;
}

bool sdk::clamp_angle(D3DXVECTOR2* angles) {
	if (angles->x > 89.0f)
		angles->x = 89.0f;

	else if (angles->x < -89.0f)
		angles->x = -89.0f;

	while (angles->y > 0.0f)
		angles->y -= 360.0f;

	while (angles->y < -360.0f)
		angles->y += 360.0f;

	if (isnan(angles->x) || isnan(angles->y) || isinf(angles->x) || isinf(angles->y))
		return false;

	return true;
}

D3DXVECTOR2 sdk::smooth_angle(D3DXVECTOR2 angle, float amount) {
	auto local_angles = D3DXVECTOR2()/*cache->localplayer->get_viewangles()*/;
	auto offset = angle - local_angles;
	offset /= amount;

	if (!sdk::clamp_angle(&offset))
		return local_angles;

	return local_angles + offset;
}


CachePlayer sdk::resolve_closest_crosshair(bool filtered) {
	auto lowest_distance = 9999;
	CachePlayer target;
	
	for (auto player : cache->players) {
		if (!player.class_object || player.localplayer || player.isDead)
			continue;
	
		if (!settings->aimbot.team && player.TeamID == cache->localplayer->TeamID)
			continue;
	
		if (!sdk::world_to_screen(player.head_position, &player.w2s_position))
			continue;

		if (!sdk::world_to_screen(player.body_position, &player.w2s_body))
			continue;
	
		auto fov = D3DXVECTOR2(process->window_size.x / 2.f, process->window_size.y / 2.f);
		auto distance = sqrt(pow(fov.x - player.w2s_position.x, 2) + pow(fov.y - player.w2s_position.y, 2));
		if (distance < lowest_distance) {
			if (filtered && settings->aimbot.fov && distance > settings->aimbot.ifov * 2)
				continue;
	
			lowest_distance = distance;
			player.position = player.get_position();
			sdk::world_to_screen(player.head_position, &player.w2s_head);
			sdk::world_to_screen(player.body_position, &player.w2s_body);
	
			target = player;
			target.w2s_position = player.w2s_position;
		}
	} 
	return target;
}

float get_random()
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(-1000, 1000); // rage 0 - 1
	return dis(e);
}

void sdk::TeleportAllPlayers() {
	for (auto player : cache->other) {
		if (auto visual_state = access->read<uintptr_t>(player.class_object + 0xD0))
			access->write<D3DXVECTOR3>(visual_state + 0x2C, D3DXVECTOR3(player.position.x, 300, player.position.z));
	}
}

std::string ReadArmaString(std::intptr_t address) {
	std::vector<char> buffer(size_t(access->read<unsigned __int32>(address + 0x8)), char{ 0 });
	access->read_raw(address + 0x10, static_cast<void*>(&buffer[0]), buffer.size());
	return std::string(buffer.data());
}

std::vector<std::pair<std::string, std::pair<float, std::intptr_t>>> sdk::GetAllMissionVariables() {
	std::vector<std::pair<std::string, std::pair<float, std::intptr_t>>> variables_list;

	auto world_ptr = access->read<uintptr_t>(process->base_address + World);
	auto namespace_rva = access->read<std::intptr_t>(world_ptr + 0x1220);
	auto curr_namespace = access->read<std::intptr_t>(namespace_rva + 0x20);

	if (!curr_namespace)
		return {};

	auto curr_namespace_size = access->read<unsigned __int32>(namespace_rva + 0x28);

	for (auto i = 0u; i <= curr_namespace_size; ++i)
	{
		auto curr_namespace_entry = access->read<std::intptr_t>(curr_namespace + (0x18 * i));

		if (!curr_namespace_entry)
			return {};

		auto curr_namespace_entry_size = access->read<unsigned __int32>(curr_namespace + ((i * 0x18) + 0x8));

		if (curr_namespace_entry) {

			for (auto i_second = 0u; i_second <= curr_namespace_entry_size; ++i_second)
			{
				auto namespace_table_entry = access->read<std::intptr_t>(curr_namespace_entry + ((0x28 * i_second) + 0x8));

				if (!namespace_table_entry)
					return {};

				auto curr_variable_tag = ReadArmaString(namespace_table_entry);

				auto curr_variable = access->read<std::intptr_t>(namespace_table_entry + ((0x28 * i_second) + 0x18));

				if (!curr_variable)
					return {};

				float variable_value = access->read<float>(curr_variable + 0x20);

				variables_list.push_back(make_pair(curr_variable_tag, make_pair(variable_value, curr_variable)));
			}
		}
	}
}

void sdk::DumpVariables()
{
	ofstream output_file("variable_dump.txt");

	for (std::pair<std::string, std::pair<float, std::intptr_t>> current_variable : GetAllMissionVariables())
	{
		cout << current_variable.first << " = " << current_variable.second.first << " @ " << current_variable.second.second << std::endl;
		output_file << current_variable.first << " = " << current_variable.second.first << " @ " << current_variable.second.second << std::endl;
	}

	output_file.close( );
}

float dot(D3DXVECTOR3 first, D3DXVECTOR3 second) {
	return (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
}

bool sdk::world_to_screen(D3DXVECTOR3 position, D3DXVECTOR2* out) {
	auto world_ptr = access->read<uintptr_t>(process->base_address + World);
	if (!world_ptr)
		return false;

	if (auto camera = access->read<uintptr_t>(world_ptr + 0x828)) {
		auto ViewRight = access->read<D3DXVECTOR3>(camera + 0x8);
		auto ViewUp = access->read<D3DXVECTOR3>(camera + 0x14);
		auto ViewForward = access->read<D3DXVECTOR3>(camera + 0x20);
		auto ViewTranslation = access->read<D3DXVECTOR3>(camera + 0x2C);
		auto ViewPortMatrix3 = access->read<D3DXVECTOR3>(camera + 0x58);
		auto ProjD1 = access->read<D3DXVECTOR3>(camera + 0xD0);
		auto ProjD2 = access->read<D3DXVECTOR3>(camera + 0xDC);

		auto temp = position - ViewTranslation;
		auto projection = D3DXVECTOR3(dot(temp, ViewRight), dot(temp, ViewUp), dot(temp, ViewForward));

		if (projection.z < 0.01f)
			return false;

		out->x = ViewPortMatrix3.x * (1 + (projection.x / ProjD1.x / projection.z));
		out->y = ViewPortMatrix3.y * (1 - (projection.y / ProjD2.y / projection.z));
		return true;
	}
	return false;
}

D3DXVECTOR2 sdk::calculate_angle(D3DXVECTOR3 camera_pos, D3DXVECTOR3 target_pos) {
	D3DXVECTOR3 relative;
	D3DXVec3Subtract(&relative, &camera_pos, &target_pos);
	float magnitude = D3DXVec3Length(&relative);
	double pitch = asin(relative[1] / magnitude);
	double yaw = -atan2(relative[0], -relative[2]);

	yaw = D3DXToDegree(yaw);
	pitch = D3DXToDegree(pitch);

	return D3DXVECTOR2(pitch, yaw);
}

D3DXVECTOR2 silent_angle;
void sdk::run_aimbot() {
	if (GetAsyncKeyState(settings->aimbot.normalHotkey)) {
		D3DXVECTOR2 headPos = D3DXVECTOR2(0.f, 0.f);
		if (settings->aimbot.head)
			headPos = resolve_closest_crosshair(true).w2s_head;
		else if (!settings->aimbot.head)
			headPos = resolve_closest_crosshair(true).w2s_body;

		auto boneX = headPos.x - process->window_size.x / 2.f;
		auto boneY = headPos.y - process->window_size.y / 2.f;

		//if (boneX >= -100 && boneX <= 100 && boneY >= -100 && boneY <= 100 && boneX != 0 && boneY != 0)
		//	mouse_event(MOUSEEVENTF_MOVE, boneX, boneY, NULL, NULL);

		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.mouseData = 0;
		input.mi.time = 0;
		input.mi.dx = boneX;
		input.mi.dy = boneY;
		input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		if (boneX >= -100 && boneX <= 100 && boneY >= -100 && boneY <= 100 && boneX != 0 && boneY != 0)
			SendInput(1, &input, sizeof(input));

		Sleep(settings->aimbot.sleep);
	}
}

static float val = 0;
static float colors[3];
D3DXVECTOR3 sdk::resolve_rgb_color() {
	static float rainbow;
	rainbow += 0.0005f;
	if (rainbow > 1.f)
		rainbow = 0.f;

	return Helpers::FromHSB(rainbow, 1.f, 1.f);
}
