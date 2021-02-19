#include "aura.h"
#include "globals.h"
#include "memory.h"
#include "offsets.h"
#include "renderer.h"
#include "entityloop.h"
#include "game.h"

using namespace aura;

bool is_vehicle(const char* name) {
	if (strstr(name, XorStr("transporthelipilot.prefab")) || strstr(name, XorStr("miniheliseat.prefab")) || strstr(name, XorStr("smallboatdriver.prefab")))
		return true;

	return false;
}

void entityloop::render() {
	char buffer[10];
	if (settings->aimbot.head)
		sprintf(buffer, "HEAD");
	else if (!settings->aimbot.head)
		sprintf(buffer, "BODY");
	renderer->draw_string(20, 10, D3DCOLOR_RGBA(255, 255, 255, 255), NULL, renderer->d3d_font, true, true, buffer);

	for (auto entity : cache->other) {
		if (!sdk::world_to_screen(entity.position, &entity.w2s_position))
			continue;
	
		char buffer[256];
		sprintf(buffer, "%s [ %i m ]", entity.driver_object, (int)round(entity.distance));
	
		if (!settings->visuals.cars && strstr(entity.name.c_str(), XorStr("car")))
			continue;

		if (!settings->visuals.helis && strstr(entity.name.c_str(), XorStr("helicopt")))
			continue;

		if (!settings->visuals.tanks && strstr(entity.name.c_str(), XorStr("tank")))
			continue;

		if (!settings->visuals.boats && strstr(entity.name.c_str(), XorStr("boat")))
			continue;

		if (!settings->visuals.planes && strstr(entity.name.c_str(), XorStr("airplane")))
			continue;

		if (settings->visuals.names)
			renderer->draw_string(entity.w2s_position.x, entity.w2s_position.y + 7.5f, entity.color, NULL, renderer->d3d_font, true, true, buffer);
	}
	
	if (!settings->visuals.players)
		return;

	for (auto player : cache->players) {
		if (!settings->visuals.shoWDead && player.isDead)
			continue;

		if (!settings->visuals.showTeam && player.TeamID == cache->localplayer->TeamID)
			continue;

		if (!sdk::world_to_screen(player.position, &player.w2s_position) || !sdk::world_to_screen(player.head_position, &player.w2s_head) || !sdk::world_to_screen(player.body_position, &player.w2s_body))
			continue;

		char buffer[256] = { 0 };
		char bottom[256] = { 0 };
		auto box_height = abs(player.w2s_position.y - player.w2s_head.y), box_width = box_height / 2.f;

		if (settings->visuals.names && player.name.length() < 32) {
			sprintf(bottom, "%s", player.name.c_str());
		}

		if (settings->visuals.distance) {
			sprintf(buffer, " [ %i m ]", (int)round(player.distance));
			strcat(bottom, buffer);
		}

		if (settings->visuals.headDot) {
			char headBuffer[256];
			sprintf(headBuffer, "•");
			renderer->draw_string(player.w2s_head.x, player.w2s_head.y, player.color, NULL, renderer->d3d_font, true, true, headBuffer);
		}

		if (settings->visuals.headCrosshair)
			renderer->draw_crosshair(player.w2s_head.x, player.w2s_head.y, player.color);

		renderer->draw_string(player.w2s_position.x, player.w2s_position.y, player.color, NULL, renderer->d3d_font, true, true, bottom);
		if (settings->visuals.boxes) {
			if (settings->visuals.filled_boxes)
				renderer->draw_filled_box_outlined(player.w2s_position.x - (box_width / 2.f), player.w2s_head.y, box_width, box_height, 1, D3DCOLOR_RGBA((int)round(settings->colors.filled_box[0] * 255.f), (int)round(settings->colors.filled_box[1] * 255.f), (int)round(settings->colors.filled_box[2] * 255.f), 60), D3DCOLOR_RGBA(0, 0, 0, 0));

			if (settings->visuals.corner_boxes) {
				auto half_width = box_width / 2.f;

				renderer->draw_line(player.w2s_position.x - (box_width / 2), player.w2s_position.y, player.w2s_position.x - half_width / 2.f, player.w2s_position.y, player.color);
				renderer->draw_line(player.w2s_position.x + (box_width / 2), player.w2s_position.y, player.w2s_position.x + half_width / 2.f, player.w2s_position.y, player.color);

				renderer->draw_line(player.w2s_position.x - (box_width / 2), player.w2s_head.y, player.w2s_position.x - half_width / 2.f, player.w2s_head.y, player.color);
				renderer->draw_line(player.w2s_position.x + (box_width / 2), player.w2s_head.y, player.w2s_position.x + half_width / 2.f, player.w2s_head.y, player.color);

				renderer->draw_line(player.w2s_position.x - (box_width / 2), player.w2s_head.y, player.w2s_position.x - (box_width / 2), player.w2s_head.y + half_width / 2.f, player.color);
				renderer->draw_line(player.w2s_position.x - (box_width / 2), player.w2s_position.y - half_width / 2.f, player.w2s_position.x - (box_width / 2), player.w2s_position.y, player.color);

				renderer->draw_line(player.w2s_position.x + (box_width / 2), player.w2s_head.y, player.w2s_position.x + (box_width / 2), player.w2s_head.y + half_width / 2.f, player.color);
				renderer->draw_line(player.w2s_position.x + (box_width / 2), player.w2s_position.y - half_width / 2.f, player.w2s_position.x + (box_width / 2), player.w2s_position.y, player.color);
			}
			else {
				renderer->draw_bordered_box_outlined(player.w2s_position.x - (box_width / 2), player.w2s_head.y, box_width, box_height, 1, player.color, D3DCOLOR_RGBA(0, 0, 0, 255));
			}
		}

		if (settings->visuals.tracelines)
			renderer->draw_line(process->window_size.x / 2.f, process->window_size.x / 2.f, player.w2s_position.x, player.w2s_position.y, player.color);

		if (settings->visuals.health) {
			sprintf(buffer, "%d", (int)round(player.health));

			auto hp_bar = player.w2s_position.y - renderer->draw_health_bar((player.w2s_position.x - (box_width / 2)) - 3, player.w2s_head.y + 1, 1, player.w2s_position.y - player.w2s_head.y - 2, player.health, 100, true, NULL);
			renderer->draw_string((player.w2s_position.x - (box_width / 2)) - renderer->get_string_width(renderer->d3d_font, buffer) + 2, hp_bar - 3, player.color, NULL, renderer->d3d_font, true, true, buffer);
		}
	}
}
