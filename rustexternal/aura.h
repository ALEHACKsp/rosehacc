#pragma once
using namespace std;

#include <Windows.h>
#include <TlHelp32.h>
#include <thread>
#include <dwmapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath> 
#include <psapi.h>

#include "xor.h"
#include "globals.h"
#include "memory.h"
#include "renderer.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

#pragma comment (lib, "dwmapi.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

namespace aura {

	enum PlayerFlags : uint32_t	{
		Unused1 = 1,
		Unused2 = 2,
		IsAdmin = 4,
		ReceivingSnapshot = 8,
		Sleeping = 16,
		Spectating = 32,
		Wounded = 64,
		IsDeveloper = 128,
		Connected = 256,
		VoiceMuted = 512,
		ThirdPersonViewmode = 1024,
		EyesViewmode = 2048,
		ChatMute = 4096,
		NoSprint = 8192,
		Aiming = 16384,
		DisplaySash = 32768,
		Relaxed = 65536,
		SafeZone = 131072,
		ServerFall = 262144,
		Workbench1 = 1048576,
		Workbench2 = 2097152,
		Workbench3 = 4194304
	};

	enum class ModelState : uint32_t {
		Ducked = 1,
		Jumped,
		OnGround = 4,
		Sleeping = 8,
		Sprinting = 16,
		OnLadder = 32,
		Flying = 64,
		Aiming = 128,
		Prone = 256,
		Mounted = 512,
		Relaxed = 1024
	};

	enum class Layers : int {
		kDefaultLayer = 0,
		kNoFXLayer = 1,
		kIgnoreRaycastLayer = 2,
		kIgnoreCollisionLayer = 3,
		kWaterLayer = 4,
		kUILayer = 5,
		kDeployed = 8,//repair bench/research table/tool cupboard/etc
		kRagdoll = 9,//supply signal/heli crates
		kInvisible = 10,
		kAI = 11,
		kPlayerMovement = 12,
		kSkyReflection = 13,
		kSky = 15,
		kWorld = 16,
		kServerPlayer = 17,
		kTrigger = 18,
		kViewModel = 20,
		kConstruction = 21,
		kTerrain = 23,
		kDebris = 26,//dropped items
		kDisplacement = 27,
		kTree = 30
	};

	enum class ObjectTag : uint16_t {
		MAINCAMERA = 5,
		PLAYER = 6,
		TERRAIN = 20001,
		CORPSE = 20009, //Animals & Players
		MISC = 20006, //Trash cans, Sleeping Bags, Storage Box, etc
		ANIMAL = 20008,
		SKYDOME = 20011,
		RIVERMESH = 20014,
		MONUMENT = 20015 //Airport, Powerplant, etc
	};

	enum class CacheEntityType : char {
		VEHICLE,
		OTHER,
	};

	class Process {
	public:
		DWORD id = 0;
		HANDLE handle = NULL;

		HWND window_handle = NULL;
		RECT window_rect;
		D3DXVECTOR2 window_size = D3DXVECTOR2();

		uint64_t base_address = 0x7ff7f8f60000;
	};

	class Overlay {
	public:
		bool create();

		HWND handle = NULL;
		WNDCLASSEX wc;
	};


	class CacheEntity {
	public:
		uintptr_t class_object = NULL;
		uintptr_t driver_object;
		uintptr_t visual_state = NULL;

		string name = "player";
		DWORD color = NULL;
		float distance = 0;
		float health = 0.f;
		bool render = false;
		bool localplayer = false;

		CacheEntityType type = CacheEntityType::OTHER;

		D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR2 w2s_position = D3DXVECTOR2(0.f, 0.f);

		bool update_ent();
		int GetID();
		float get_health();
		D3DXVECTOR3 get_position();
	};

	class CachePlayer : public CacheEntity {
	public:
		uintptr_t base_player = NULL;
		float health = 0.f;

		bool localplayer = false;
		DWORD localTeamID = 0;
		bool team_mate = false;

		bool isDead = false;
		int TeamID = 0;
		int item_uid = 0;

		D3DXVECTOR3 head_position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR2 w2s_head = D3DXVECTOR2(0.f, 0.f);

		D3DXVECTOR3 body_position = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR2 w2s_body = D3DXVECTOR2(0.f, 0.f);

		D3DXVECTOR3 view_angles = D3DXVECTOR3(0.f, 0.f, 0.f);

		bool update();

		string get_name();
		D3DXVECTOR3 get_headposition();
		D3DXVECTOR3 get_bodyposition();
		bool getDead();
		int GetID();
		int GetItemUID();
	};

	class CacheLocalPlayer : public CachePlayer {
	public:

	};


	class Cache {
	public:
		bool enabled = true;
		
		CacheLocalPlayer* localplayer = new CacheLocalPlayer();

		vector<CachePlayer> players; // Players Only
		vector<CacheEntity> other; // Everything else (nodes, items, hemp etc.)

		void update();
		void collect();
		void clear();
	};

	struct AimbotSettings {
		bool enabled = true;
		bool aim = false;
		bool silent = false;
		bool head = true;
		CachePlayer frame_target;

		/* Targets */
		bool players = false;
		bool team = true;
		bool scientists = false;

		/* Other */
		bool prediction = false;
		bool while_shooting = false;
		bool rcs = false;
		int hotkey = 0x46;
		int normalHotkey = 0x46;
		int target_hotkey = 0x46;
		int frame_hotkey = 0x46;
		int smoothness = 0;
		int sleep = 20;

		bool fov = false;
		int ifov = 80;

		/* Debug */
		CachePlayer target;
		D3DXVECTOR3 position;
	};

	struct VisualSettings {
		/* Objects */
		bool players = true;
		bool teamVicColors = true;
		bool cars = true;
		bool helis = false;
		bool tanks = false;
		bool boats = false;
		bool planes = false;

		/* Filters */
		bool boxes = true;
		bool corner_boxes = true;
		bool filled_boxes = false;
		bool names = true;
		bool shoWDead = false;
		bool showTeam = true;
		bool distance = true;
		bool headDot = true;
		bool headCrosshair = false;
		bool health = true;
		bool sleepers = false;
		bool tracelines = false;

		/* Other */
		bool aim_line = false;
		bool fov_circle = true;
		bool crosshair = true;
	};

	struct MiscSettings {
		int selected_config = 0;

		bool kill_restrictions = false;

		/* Other */
		string session_username = "";

		CachePlayer magic_target;

		bool console_output = false;
		void console_log(string message) {
			if (console_output)
				cout << message << endl;
		}

		bool debug = false;
		bool debug_playerlist = false;
		bool debug_other = false;
		bool debug_projectiles = false;

		bool menu_open = false;
		bool cheat_running = true;
		bool in_game = false;

		CachePlayer closest_aim_target;
		bool valid_aim_target = false;

		bool no_clip = false;

		/* Entityloop */
		clock_t active_lastupdated;
		clock_t tagged_lastupdated;

		int active_currentidx = 0;
		int tagged_currentidx = 0;
		float active_updatetime = 0;
		float tagged_updatetime = 0;

		/* Performance */
		int render_rate = 3;
		int active_rate = 1;
		int tagged_rate = 1;

		/* Settings */
		int menu_key = VK_INSERT;
		bool hide_overlay = false;
		bool rgb_theme = false;
		bool key_focused = false;
		bool menu_active = false;
		bool draw_menu = false;
	};

	struct ColorSettings {
		float civilianPlayer[3] = { 1, 1, 1 };
		float independantPlayer[3] = { 1, 1, 1 };
		float blueforPlayer[3] = { 1, 1, 1 };
		float opforPlayer[3] = { 1, 1, 1 };
		float magic_target[3] = { 1, 1, 1 };
		float car[3] = { 1, 1, 1 };
		float helicopter[3] = { 1, 1, 1 };
		float tank[3] = { 1, 1, 1 };
		float boat[3] = { 1, 1, 1 };
		float airplane[3] = { 1, 1, 1 };
		float team[3] = { 0, 1, 0 };

		float filled_box[3] = { 0.2, 0.2, 0.2 };
		float menu_color[3] = { 0.235, 0.125, 0.933 };

		D3DXVECTOR3 menu_theme;
		float rgb_speed = 0.25f;
	};

	struct FontSettings {
		int size = 13;
		int selected_font = 0;
		bool italic = false;
	};

	struct HackSettings {
		AimbotSettings aimbot;
		VisualSettings visuals;
		MiscSettings misc;
		FontSettings font;
		ColorSettings colors;
	};

	extern Access* access;
	extern Process* process;
	extern Cache* cache;
	
	extern Renderer* renderer;
	extern Overlay* overlay;
	extern HackSettings* settings;
}