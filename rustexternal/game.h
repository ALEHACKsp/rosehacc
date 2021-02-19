#pragma once
#include "aura.h"
#include "memory.h"
#include "offsets.h"
#include "globals.h"
#include <intrin.h>

using namespace aura;

typedef struct {
	float x, y, z;
}Vector;

#ifdef DEBUG
static bool debugging_func = true;
#else
static bool debugging_func = false;
#endif // DEBUG
#define M_PI 3.14159265359

namespace aura {
	class sdk {
	public:
		/* CHEATS */
		static void run_aimbot();

		static void inject_script(char* name);
		static void kill_infistar();
		static void kill_scriptrestrictions();

		/* Resolves */
		static CachePlayer resolve_closest_crosshair(bool filtered);
		static D3DXVECTOR3 resolve_rgb_color();

		static float get_distance(D3DXVECTOR3 start, D3DXVECTOR3 end);
		static bool clamp_angle(D3DXVECTOR2* angles);
		static D3DXVECTOR2 smooth_angle(D3DXVECTOR2 angle, float amount);
		static D3DXVECTOR2 calculate_angle(D3DXVECTOR3 camera_pos, D3DXVECTOR3 target_pos);
		static bool world_to_screen(D3DXVECTOR3 position, D3DXVECTOR2* out);

		static bool in_game();
		static void TeleportAllPlayers();
		static std::vector<std::pair<std::string, std::pair<float, std::intptr_t>>> GetAllMissionVariables();
		static void DumpVariables();
	};
}
