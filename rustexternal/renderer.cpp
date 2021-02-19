#include "aura.h"
#include "globals.h"
#include "game.h"
#include "renderer.h"
#include "memory.h"
#include "entityloop.h"
#include <iostream>
#include <random>
#include "config.h"

using namespace aura;

bool Renderer::create_device(HWND hWnd) {
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.BackBufferCount = 1;
	g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	g_d3dpp.BackBufferWidth = process->window_size.x;
	g_d3dpp.BackBufferHeight = process->window_size.y;
	g_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	g_d3dpp.hDeviceWindow = hWnd;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void Renderer::cleanup_device() {
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void Renderer::reset_device() {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void update_interaction(bool state, HWND hwnd) {
	long style = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (state){
		style &= ~WS_EX_LAYERED;
		SetWindowLong(hwnd, GWL_EXSTYLE, style);
	}
	else{
		style |= WS_EX_LAYERED;
		SetWindowLong(hwnd, GWL_EXSTYLE, style);
	}
}

void slider_float(const char* text, float* value, float min, float max, int id) {
	ImGui::Text(text);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75);
	ImGui::PushID(id);
	ImGui::SliderFloat(XorStr(""), value, min, max);
}

void slider(const char* text, int* value, int min, int max, int id) {
	ImGui::Text(text);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(75);
	ImGui::PushID(id);
	ImGui::SliderInt(XorStr(""), value, min, max);
}

void visual_option(const char* label, bool* option, float color[3], int id) {
	ImGui::PushID(id);
	ImGui::Checkbox(label, option);
	ImGui::SameLine();
	ImGui::ColorEdit3(XorStr(""), color, ImGuiColorEditFlags_NoInputs);
}

map<int, string> menu_tabs {
	{ 0, XorStr("Aimbot") },
	{ 1, XorStr("Visuals") },
	{ 2, XorStr("Misc") },
	{ 3, XorStr("Settings") },
};

map<int, string> setting_tabs {
	{ 0, XorStr("General") },
	{ 1, XorStr("Performance") },
	{ 2, XorStr("Debug")},
};

Config* config;
void load_color(const char* name, float color[3]) {
	const char* vects[3] = { "r", "g", "b" };
	int current_color = 0;

	for (int i = 0; i < 3; i++) {
		char config_name[64];
		sprintf(config_name, "%s.%s", name, vects[current_color]);

		config->SetupValue(config_name, color[current_color]);
		current_color++;
	}
}

void load_config() {
	config->SetupValue(XorStr("aimbot-enabled"), settings->aimbot.aim);
	config->SetupValue(XorStr("aimbot-normalkey"), settings->aimbot.normalHotkey);
	config->SetupValue(XorStr("aimbot-sleep"), settings->aimbot.sleep);
	config->SetupValue(XorStr("aimbot-silent"), settings->aimbot.silent);
	config->SetupValue(XorStr("aimbot-fov"), settings->aimbot.fov);
	config->SetupValue(XorStr("aimbot-team"), settings->aimbot.team);
	config->SetupValue(XorStr("aimbot-hotkey"), settings->aimbot.hotkey);
	config->SetupValue(XorStr("aimbot-targetkey"), settings->aimbot.target_hotkey);

	config->SetupValue(XorStr("visuals-players"), settings->visuals.players);

	config->SetupValue(XorStr("visuals-boxes"), settings->visuals.boxes);
	config->SetupValue(XorStr("visuals-corner_boxes"), settings->visuals.corner_boxes);
	config->SetupValue(XorStr("visuals-filled_boxes"), settings->visuals.filled_boxes);
	config->SetupValue(XorStr("visuals-names"), settings->visuals.names);
	config->SetupValue(XorStr("visuals-dead"), settings->visuals.shoWDead);
	config->SetupValue(XorStr("visuals-team"), settings->visuals.showTeam);
	config->SetupValue(XorStr("visuals-dot"), settings->visuals.headDot);
	config->SetupValue(XorStr("visuals-distance"), settings->visuals.distance);
	config->SetupValue(XorStr("visuals-health"), settings->visuals.health);
	config->SetupValue(XorStr("visuals-crosshair"), settings->visuals.crosshair);
	config->SetupValue(XorStr("visuals-cars"), settings->visuals.cars);
	config->SetupValue(XorStr("visuals-planes"), settings->visuals.planes);
	config->SetupValue(XorStr("visuals-helis"), settings->visuals.helis);
	config->SetupValue(XorStr("visuals-boats"), settings->visuals.boats);
	config->SetupValue(XorStr("visuals-fov_circle"), settings->visuals.fov_circle);
	config->SetupValue(XorStr("visuals-crosshair"), settings->visuals.crosshair);

	config->SetupValue(XorStr("misc-menu_key"), settings->misc.menu_key);
	config->SetupValue(XorStr("misc-hide_overlay"), settings->misc.hide_overlay);
	config->SetupValue(XorStr("misc-rgb_theme"), settings->misc.rgb_theme);
	config->SetupValue(XorStr("misc-key_focused"), settings->misc.key_focused);

	config->SetupValue(XorStr("font-size"), settings->font.size);
	config->SetupValue(XorStr("font-italic"), settings->font.italic);
	config->SetupValue(XorStr("font-selected_font"), settings->font.selected_font);

	config->SetupValue(XorStr("settings-render_rate"), settings->misc.render_rate);
	config->SetupValue(XorStr("settings-active_rate"), settings->misc.active_rate);
	config->SetupValue(XorStr("settings-tagged_rate"), settings->misc.tagged_rate);
	config->SetupValue(XorStr("settings-debug_enabled"), settings->misc.debug_other);
	config->SetupValue(XorStr("settings-debug_projectiles"), settings->misc.debug_projectiles);
	config->SetupValue(XorStr("settings-debug_playerlist"), settings->misc.debug_playerlist);

	load_color(XorStr("color-civilian"), settings->colors.civilianPlayer);
	load_color(XorStr("color-independant"), settings->colors.independantPlayer);
	load_color(XorStr("color-blufor"), settings->colors.blueforPlayer);
	load_color(XorStr("color-opfor"), settings->colors.opforPlayer);
	load_color(XorStr("color-team"), settings->colors.team);
	load_color(XorStr("color-cars"), settings->colors.car);
	load_color(XorStr("color-helicopter"), settings->colors.helicopter);
	load_color(XorStr("color-tank"), settings->colors.tank);
	load_color(XorStr("color-airplane"), settings->colors.airplane);
	load_color(XorStr("color-boat"), settings->colors.boat);
	load_color(XorStr("color-target"), settings->colors.magic_target);
	load_color(XorStr("color-filled_box"), settings->colors.filled_box);
	load_color(XorStr("color-menu_color"), settings->colors.menu_color);

}

int menu_index = 0;
int sub_menu_index = 0;
void render_tabs(map<int, string> tabs, bool sub_menu, float justifier) {
	for (const auto& tab : tabs) {
		ImGui::SameLine();

		auto is_active = sub_menu ? (sub_menu_index == tab.first) : (menu_index == tab.first);
		if (is_active)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		if (ImGui::Button(tab.second.c_str(), ImVec2(450.f / tabs.size() - justifier, 25))) {
			if (!sub_menu) {
				if (tab.first == 1 || tab.first == 2 || tab.first == 3)
					sub_menu_index = 0;

				menu_index = tab.first;
			} else {
				sub_menu_index = tab.first;
			}
			
		}

		if (is_active)
			ImGui::PopStyleColor(1);

	}
}

const char* fonts[] = { "Arial", "Impact", "Tahoma", "Trebuchet MS", "Webdings" };
const char* config_names[] = { "slot1", "slot2", "slot3" };
const char* config_file_names[] = { "Slot1.rosehacc", "Slot2.rosehacc", "Slot3.rosehacc" };
void render_menu() {
	if (settings->misc.draw_menu || (!settings->misc.key_focused && settings->misc.menu_open)) {
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(settings->colors.menu_theme.x / 255.f, settings->colors.menu_theme.y / 255.f, settings->colors.menu_theme.z / 255.f, 255));
		if (ImGui::Begin(XorStr("roseha.cc"), &settings->misc.menu_open, ImVec2(450.f, 300.f), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
			ImGui::PopStyleColor(1);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));
			render_tabs(menu_tabs, false, 5.f);

			auto child_height = 240.f;
			switch (menu_index) {
			case 3:
				ImGui::Text("");
				render_tabs(setting_tabs, true, 6.15f);
				child_height = 215.f;
				break;
			default:
				break;
			}

			ImGui::PopStyleVar(2);

			ImGui::BeginChild(XorStr("##"), ImVec2(433.f, child_height), 1.f);
			switch (menu_index) {
			case 0:
				ImGui::Checkbox(XorStr("Aimbot"), &settings->aimbot.aim);
				ImGui::SameLine();
				ImGui::Hotkey(XorStr(""), &settings->aimbot.normalHotkey, ImVec2(75, 20));
				ImGui::PushID(534);
				slider(XorStr("Aimbot Sleep"), &settings->aimbot.sleep, 1, 100, 54684);
				ImGui::PushID(43453);
				ImGui::Checkbox(XorStr("Magic Bullet"), &settings->aimbot.silent);
				ImGui::SameLine();
				ImGui::Hotkey(XorStr(""), &settings->aimbot.hotkey, ImVec2(75, 20));
				ImGui::PushID(498762);
				ImGui::Text(XorStr("Target Selection"));
				ImGui::SameLine();
				ImGui::Hotkey(XorStr(""), &settings->aimbot.target_hotkey, ImVec2(75, 20));
				ImGui::PushID(4984242);
				ImGui::Text(XorStr("Frame Selection"));
				ImGui::SameLine();
				ImGui::Hotkey(XorStr(""), &settings->aimbot.frame_hotkey, ImVec2(75, 20));
				ImGui::PushID(4414242);
				char buffer[32];
				sprintf(buffer, "%p", settings->aimbot.frame_target.class_object);
				ImGui::Text(buffer);
				ImGui::Checkbox(XorStr("Field of View"), &settings->aimbot.fov);
				ImGui::SameLine();
				slider(XorStr(""), &settings->aimbot.ifov, 1, 180, 54684);
				ImGui::Checkbox(XorStr("Target team"), &settings->aimbot.team);

				break;
			case 1:
				ImGui::Checkbox(XorStr("Players"), &settings->visuals.players);
				ImGui::ColorEdit3(XorStr("##"), settings->colors.civilianPlayer, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::Text("Civilian");
				ImGui::PushID(79);
				ImGui::ColorEdit3(XorStr("###"), settings->colors.independantPlayer, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::Text("Independant");
				ImGui::PushID(794);
				ImGui::ColorEdit3(XorStr("####"), settings->colors.blueforPlayer, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::Text("Blufor");
				ImGui::PushID(793);
				ImGui::ColorEdit3(XorStr("#####"), settings->colors.opforPlayer, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::Text("Opfor");
				ImGui::PushID(791);
				if(settings->visuals.players)
					visual_option(XorStr("Filled Boxes"), &settings->visuals.filled_boxes, settings->colors.filled_box, 1499);
				ImGui::Checkbox(XorStr("Team vehicle colors"), &settings->visuals.teamVicColors);

				ImGui::Checkbox(XorStr("######"), &settings->visuals.cars);
				ImGui::SameLine();
				ImGui::Text("Cars");
				ImGui::PushID(731);
				ImGui::Checkbox(XorStr("#######"), &settings->visuals.helis);
				ImGui::SameLine();
				ImGui::Text("Helicopters");
				ImGui::PushID(751);
				ImGui::Checkbox(XorStr("########"), &settings->visuals.planes);
				ImGui::SameLine();
				ImGui::Text("Planes");
				ImGui::PushID(711);
				ImGui::Checkbox(XorStr("#########"), &settings->visuals.tanks);
				ImGui::SameLine();
				ImGui::Text("Tanks");
				ImGui::PushID(771);
				ImGui::Checkbox(XorStr("##########"), &settings->visuals.boats);
				ImGui::SameLine();
				ImGui::Text("Boats");
				ImGui::PushID(191);

				if (!settings->visuals.teamVicColors) {
					ImGui::ColorEdit3(XorStr("###########"), settings->colors.car, ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::Text("Cars");
					ImGui::PushID(109);

					ImGui::ColorEdit3(XorStr("############"), settings->colors.helicopter, ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::Text("Helicopters");
					ImGui::PushID(132);

					ImGui::ColorEdit3(XorStr("#############"), settings->colors.airplane, ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::Text("Planes");
					ImGui::PushID(132);

					ImGui::ColorEdit3(XorStr("##############"), settings->colors.tank, ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::Text("Tanks");
					ImGui::PushID(852);
					ImGui::ColorEdit3(XorStr("###############"), settings->colors.boat, ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::Text("Boats");
					ImGui::PushID(324);
				}
				ImGui::PushID(31254);
				ImGui::Text(XorStr("Magic Target"));
				ImGui::SameLine();
				ImGui::ColorEdit3(XorStr(""), settings->colors.magic_target, ImGuiColorEditFlags_NoInputs);

				ImGui::NewLine();

				ImGui::Checkbox(XorStr("Name"), &settings->visuals.names);
				ImGui::Checkbox(XorStr("Show Dead"), &settings->visuals.shoWDead);
				ImGui::Checkbox(XorStr("Show Team"), &settings->visuals.showTeam);
				ImGui::Checkbox(XorStr("Head Dot"), &settings->visuals.headDot);
				ImGui::Checkbox(XorStr("Head Crosshair"), &settings->visuals.headCrosshair);
				ImGui::Checkbox(XorStr("Distance"), &settings->visuals.distance);
				ImGui::Checkbox(XorStr("Health"), &settings->visuals.health);
				ImGui::Checkbox(XorStr("2D Boxes"), &settings->visuals.boxes);
				ImGui::Checkbox(XorStr("Corner Box"), &settings->visuals.corner_boxes);

				break;
			case 2:
				if (ImGui::Button(XorStr("Inject Script"))) {
					ifstream ifs("script.sqf");
					string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

					char script[64000];
					sprintf(script, "%s", content.c_str());

					sdk::inject_script(script);
				}

				if (ImGui::Button(XorStr("Kill Restrictions")))
					sdk::kill_scriptrestrictions();

				ImGui::SameLine();

				if (ImGui::Button(XorStr("Kill Anti-Cheat")))
					sdk::kill_infistar();

				if (ImGui::Button(XorStr("Teleport All Players")))
					sdk::TeleportAllPlayers();

				if (ImGui::Button(XorStr("Dump Scripts")))
					sdk::DumpVariables();
				break;
			case 3:
				switch (sub_menu_index) {
				case 0:
					ImGui::Columns(2, NULL, true);
					ImGui::Text(XorStr("Font"));
					ImGui::SameLine();	
					ImGui::Combo("", &settings->font.selected_font, fonts, 5);
					slider(XorStr("Font Size"), &settings->font.size, 5, 20, 165);
					ImGui::Checkbox(XorStr("Italic"), &settings->font.italic);
					ImGui::SameLine();
					if (ImGui::Button(XorStr("Update")))
						renderer->d3d_font = renderer->create_font(fonts[settings->font.selected_font], settings->font.size, 1, settings->font.italic);

					ImGui::NewLine();

					ImGui::Checkbox(XorStr("Keyfocus Menu"), &settings->misc.key_focused);
					ImGui::Checkbox(XorStr("Hide Overlay"), &settings->misc.hide_overlay);
					visual_option(XorStr("Menu Theme"), &settings->misc.rgb_theme, settings->colors.menu_color, 1526);
					slider_float(XorStr("RGB Speed"), &settings->colors.rgb_speed, 0.15f, 7.5f, 7651);
					ImGui::Text(XorStr("Menu Key"));
					ImGui::SameLine();
					ImGui::Hotkey(XorStr("#####"), &settings->misc.menu_key, ImVec2(75.f, 20.f));

					ImGui::NextColumn();

					ImGui::Text(XorStr("Cache"));
					ImGui::Checkbox(XorStr("Enabled"), &cache->enabled);

					ImGui::NewLine();

					ImGui::Text(XorStr("Access"));
					if (ImGui::Button(XorStr("Clear")))
						cache->clear();

					ImGui::NewLine();
					ImGui::NewLine();

					ImGui::Text(XorStr("Config"));
					ImGui::SameLine();
					ImGui::Combo("##", &settings->misc.selected_config, config_names, 3);

					if (ImGui::Button(XorStr("Save"))) {
						config = new Config(config_file_names[settings->misc.selected_config]);
						load_config();
						config->Save();
					}

					ImGui::SameLine();
					if (ImGui::Button(XorStr("Load"))) {
						config = new Config(config_file_names[settings->misc.selected_config]);
						load_config();
						config->Load();
					}

					ImGui::Columns(1);
					break;
				case 1:
					slider(XorStr("Render Rate (ms)"), &settings->misc.render_rate, 3, 30, 4987);
					slider(XorStr("Active Updaterate (s)"), &settings->misc.active_rate, 1, 32, 1354);
					slider(XorStr("Tagged Updaterate (s)"), &settings->misc.tagged_rate, 1, 120, 9854);
					if (ImGui::Button(XorStr("Fast Preset"))) {
						settings->misc.render_rate = 8;
						settings->misc.active_rate = 5;
						settings->misc.tagged_rate = 3;
					}

					ImGui::SameLine();

					if (ImGui::Button(XorStr("Slow Preset"))) {
						settings->misc.render_rate = 22;
						settings->misc.active_rate = 10;
						settings->misc.tagged_rate = 5;
					}

					break;
				case 2:
					ImGui::Checkbox(XorStr("Player List"), &settings->misc.debug_playerlist);
					ImGui::Checkbox(XorStr("Debug Panel"), &settings->misc.debug_other);
					ImGui::Checkbox(XorStr("Projectiles"), &settings->misc.debug_projectiles);
					break;
				}
				break;
			default:
				ImGui::Text(XorStr("This tab is currently unavailable."));
				break;
			}
			ImGui::EndChild();
			ImGui::End();
		}
	}
}

void Renderer::draw_panel(int x, int y, int width, int height, const char* title, D3DCOLOR color, D3DCOLOR outline, D3DCOLOR text_outline) {
	draw_filled_box_outlined(x, y, width, height, 1.f, color, outline);

	char panel_title[64];
	sprintf(panel_title, XorStr("%s"), title);
	draw_string(x + (width / 2), y + 5.f, D3DCOLOR_RGBA(255, 255, 255, 255), text_outline, d3d_font, true, true, panel_title);
	draw_line(x, y + 20.f, x + width, y + 20.f, D3DCOLOR_RGBA(255, 255, 255, 255));
}

void Renderer::render() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(overlay->handle);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	SetLayeredWindowAttributes(overlay->handle, 0, 1.0f, LWA_ALPHA);

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(overlay->handle, &margin);

	ShowWindow(overlay->handle, SW_SHOWDEFAULT);
	SetWindowPos(overlay->handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	d3d_font = create_font(XorStr("Arial"), 13, 1, false);

	config = new Config(config_file_names[0]);
	load_config();

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT){
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)){
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		render_menu();
		ImGui::EndFrame();

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0) {
			HWND foreground_window = GetForegroundWindow();
			if (!settings->misc.hide_overlay || settings->misc.hide_overlay && (foreground_window == process->window_handle || foreground_window == overlay->handle)) {
				update_interaction(settings->misc.draw_menu || (!settings->misc.key_focused && settings->misc.menu_open), overlay->handle);

				entityloop::render();
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

				//char cheat_name[128];
				//sprintf(cheat_name, XorStr("roseha.cc - %d fps - %s - %s"), (int)ImGui::GetIO().Framerate, __DATE__, settings->misc.session_username.c_str());

				auto x_axis = process->window_size.x / 12;
				//auto string_width = get_string_width(d3d_font, cheat_name);
				//draw_filled_box_outlined(x_axis - (string_width / 2) - 5.f, 15.f, string_width + 10.f, 25.f, 1.f, D3DCOLOR_RGBA(25, 25, 25, 200), D3DCOLOR_RGBA((int)settings->colors.menu_theme.x, (int)settings->colors.menu_theme.y, (int)settings->colors.menu_theme.z, 255));
				//draw_string(x_axis, 20.f, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, d3d_font, false, true, cheat_name);

				if (settings->misc.debug_playerlist) {
					draw_string(x_axis, 50.f, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, d3d_font, true, true, XorStr("Players"));

					auto y_axis = 65;
					for (auto ply : cache->players) {
						char ply_info[64];
						if (ply.localplayer)
							sprintf(ply_info, "[Local] %s (%p) - %d %d %d", ply.name.c_str(), ply.base_player, (int)ply.position.x, (int)ply.position.y, (int)ply.position.z);
						else
							sprintf(ply_info, "%s (%p) - %d %d %d", ply.name.c_str(), ply.base_player, (int)ply.position.x, (int)ply.position.y, (int)ply.position.z);
						draw_string(x_axis, y_axis, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, d3d_font, true, true, ply_info);
						y_axis += 20;

					}
				}


				if (settings->misc.debug_other) {
					draw_panel(50.f, process->window_size.y / 4, 170.f, 180.f, XorStr("[ Cache ]"), D3DCOLOR_RGBA(25, 25, 25, 200), D3DCOLOR_RGBA(215, 215, 215, 255), D3DCOLOR_RGBA(150, 150, 150, 255));

					auto net_manager = access->read<uintptr_t>(process->base_address + 0x258C9D0);

					char buffer[256];
					sprintf(buffer, "[ information ]\n base address: 0x%p\n net manager: 0x", process->base_address, net_manager);
					draw_string(55.f, process->window_size.y / 4 + 25.f, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, d3d_font, true, false, buffer);

					sprintf(buffer, "%d %d %d | %s", (int)settings->aimbot.position.x, (int)settings->aimbot.position.y, (int)settings->aimbot.position.z, settings->aimbot.target.name.c_str());
					draw_string(process->window_size.x / 2, process->window_size.y / 2 + 25.f, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, d3d_font, true, true, buffer);
				}

				auto fov = D3DXVECTOR2(process->window_size.x / 2.f, process->window_size.y / 2.f);
				if (settings->visuals.fov_circle && settings->aimbot.enabled && settings->aimbot.fov)
					draw_circle(fov.x, fov.y, settings->aimbot.ifov * 2, settings->aimbot.ifov * 2, D3DCOLOR_RGBA(255, 255, 255, 255));

				if(settings->visuals.aim_line && settings->misc.valid_aim_target)
					draw_line(fov.x, fov.y, settings->misc.closest_aim_target.w2s_position.x, settings->misc.closest_aim_target.w2s_position.y, D3DCOLOR_RGBA(255, 255, 255, 255));

				if (settings->visuals.crosshair)
					draw_crosshair(fov.x, fov.y, D3DCOLOR_RGBA(255, 255, 255, 255));
			} else {
				update_interaction(false, overlay->handle);
			}
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(settings->misc.render_rate));
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


int Renderer::draw_health_bar(int x, int y, int width, int height, float value, float max_health, bool vertical, D3DCOLOR colour){
	if (vertical) {
		int healthbar_maxheight = height;
		float healthbar_volume = value / max_health;
		float healthbar_height = healthbar_maxheight * healthbar_volume;

		if (healthbar_volume >= 1)
			healthbar_height = healthbar_maxheight;

		draw_filled_box_outlined(x - 1, y - 1, width + 2, height + 2, 1, D3DCOLOR_RGBA(0, 0, 0, 255), D3DCOLOR_RGBA(0, 0, 0, 255));
		if (!colour) {
			float r, g;
			g = (healthbar_volume * 100.f) * 2.55f;
			r = 255 - g;
			draw_bordered_box(x, y, width, healthbar_maxheight, 1, D3DCOLOR_RGBA((int)r, (int)g, 0, 255));
		}
		else {
			draw_bordered_box(x, y, width, healthbar_maxheight, 1, colour);
		}

		draw_filled_box(x, y, width, healthbar_maxheight - healthbar_height, D3DCOLOR_RGBA(0, 0, 0, 255));
		return healthbar_height;
	} else {
		int healthbar_maxlength = width;
		float healthbar_volume = value / max_health;
		float healthbar_length = healthbar_maxlength * healthbar_volume;

		if (healthbar_volume >= 1)
			healthbar_length = healthbar_maxlength;

		draw_filled_box_outlined(x - 1, y - 1, width + 2, height + 2, 1, D3DCOLOR_RGBA(0, 0, 0, 255), D3DCOLOR_RGBA(0, 0, 0, 255));
		draw_bordered_box(x, y, healthbar_maxlength, height, 1, D3DCOLOR_RGBA(0, 0, 0, 255));

		draw_filled_box(x, y, healthbar_maxlength - healthbar_length, height, colour);
		return healthbar_length;
	}
}


void Renderer::draw_line(int x, int y, int x2, int y2, D3DCOLOR colour) {
	SD3DVertex pVertex[2] = { { x, y, 0.0f, 3.0f, colour }, { x2, y2, 0.0f, 3.0f, colour } };
	g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &pVertex, sizeof(SD3DVertex));
}

void Renderer::draw_filled_box(int x, int y, int w, int h, D3DCOLOR color) {
	SD3DVertex pVertex[4] = { { x, y + h, 0.0f, 1.0f, color }, { x, y, 0.0f, 1.0f, color }, { x + w, y + h, 0.0f, 1.0f, color }, { x + w, y, 0.0f, 1.0f, color } };
	g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
}

void Renderer::draw_bordered_box(int x, int y, int width, int height, int thickness, D3DCOLOR colour) {
	draw_filled_box(x, y, width, thickness, colour);
	draw_filled_box(x, y, thickness, height, colour);
	draw_filled_box(x + width - thickness, y, thickness, height, colour);
	draw_filled_box(x, y + height - thickness, width, thickness, colour);
}

void Renderer::draw_bordered_box_outlined(int x, int y, int width, int height, float thickness, D3DCOLOR colour, D3DCOLOR outlinecolor) {
	draw_bordered_box(x, y, width, height, thickness, outlinecolor);
	draw_bordered_box(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), thickness, colour);
	draw_bordered_box(x + (thickness * 2), y + (thickness * 2), width - (thickness * 4), height - (thickness * 4), thickness, outlinecolor);
}

void Renderer::draw_filled_box_outlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor) {
	draw_bordered_box(x, y, width, height, thickness, outlinecolor);
	draw_filled_box(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), colour);
}

void Renderer::draw_crosshair(int x, int y, D3DCOLOR color) {
	draw_filled_box(x, y - 5, 1, 11, color);
	draw_filled_box(x - 5, y, 11, 1, color);
}

ID3DXFont* Renderer::create_font(LPCSTR fontName, int size, int weight, bool italic){
	ID3DXFont* font;
	D3DXCreateFontA(g_pd3dDevice, size, NULL, weight, NULL, italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, NULL, DEFAULT_PITCH | FF_DONTCARE, fontName, &font);

	return font;
}

void Renderer::draw_circle(int x, int y, int radius, int points, D3DCOLOR colour) {
	SD3DVertex* pVertex = new SD3DVertex[points + 1];
	for (int i = 0; i <= points; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
	g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points, pVertex, sizeof(SD3DVertex));
	delete[] pVertex;
}

#pragma region  imgui drawing

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void Renderer::imgui_stroketext(int x, int y, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)), utf_8_2.c_str());
}


void Renderer::imgui_text(int x, int y, const char* str, D3DCOLOR color) {
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), color, utf_8_2.c_str());
}

void Renderer::imgui_box(float X, float Y, float W, float H, D3DCOLOR color)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), color);
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), color);
}

void Renderer::imgui_line(int x1, int y1, int x2, int y2, D3DCOLOR color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
}

void Renderer::imgui_circle(int x, int y, int radius, D3DCOLOR color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, color, segments);
}
#pragma endregion

void Renderer::draw_string(int x, int y, D3DCOLOR colour, D3DCOLOR outline_colour, ID3DXFont* font, bool outlined, bool centered, char* string, ...){
	va_list args;
	char cBuffer[256];

	va_start(args, string);
	vsprintf_s(cBuffer, string, args);
	va_end(args);

	if (centered)
		x -= get_string_width(font, string) / 2;

	RECT pRect;
	if (outlined) {
		auto outline_color = D3DCOLOR_RGBA(0, 0, 0, 255);
		if (outline_colour)
			outline_color = outline_colour;

		pRect.left = x - 1;
		pRect.top = y;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, outline_color);
		pRect.left = x + 1;
		pRect.top = y;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, outline_color);
		pRect.left = x;
		pRect.top = y - 1;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, outline_color);
		pRect.left = x;
		pRect.top = y + 1;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, outline_color);
	}
	pRect.left = x;
	pRect.top = y;
	font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, colour);
}

int Renderer::get_string_width(ID3DXFont* font, char* string) {
	RECT pRect = RECT();
	font->DrawTextA(NULL, string, strlen(string), &pRect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
	return pRect.right - pRect.left;
}