#pragma once
#include "aura.h"

class Renderer {
public:
	struct SD3DVertex {
		float x, y, z, rhw;
		DWORD colour;
	};

	bool create_device(HWND hWnd);
	void cleanup_device();
	void reset_device();

	LPDIRECT3D9 g_pD3D;
	D3DPRESENT_PARAMETERS g_d3dpp;
	LPDIRECT3DDEVICE9 g_pd3dDevice;
	ID3DXFont* d3d_font = NULL;

	void render();

	void draw_filled_box(int x, int y, int width, int height, D3DCOLOR colour);
	void draw_bordered_box(int x, int y, int width, int height, int thickness, D3DCOLOR colour);
	void draw_panel(int x, int y, int width, int height, const char* title, D3DCOLOR color, D3DCOLOR outline, D3DCOLOR text_outline);
	void draw_bordered_box_outlined(int x, int y, int width, int height, float thickness, D3DCOLOR colour, D3DCOLOR outlinecolor);
	void draw_filled_box_outlined(int x, int y, int width, int height, int thickness, D3DCOLOR colour, D3DCOLOR outlinecolor);
	void draw_string(int x, int y, D3DCOLOR colour, D3DCOLOR outline_colour, ID3DXFont* font, bool outlined, bool centered, char* string, ...);
	int draw_health_bar(int x, int y, int width, int height, float value, float max_health, bool vertical, D3DCOLOR colour);
	void draw_circle(int x, int y, int radius, int points, D3DCOLOR colour);
	void draw_line(int x, int y, int x2, int y2, D3DCOLOR colour);
	void draw_crosshair(int x, int y, D3DCOLOR color);

	void imgui_stroketext(int x, int y, const char* str);
	void imgui_text(int x, int y, const char* str, D3DCOLOR color);
	void imgui_box(float X, float Y, float W, float H, D3DCOLOR color);
	void imgui_line(int x1, int y1, int x2, int y2, D3DCOLOR color, int thickness);
	void imgui_circle(int x, int y, int radius, D3DCOLOR color, int segments);

	int get_string_width(ID3DXFont* font, char* string);
	ID3DXFont* create_font(LPCSTR fontName, int size, int weight, bool italic);
};