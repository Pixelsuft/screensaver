#define _CRT_SECURE_NO_WARNINGS
#define WIN_32_LEAN_AND_MEAN
#define ERROR_EXIT_CODE 1
#define MIN_SPEED 0.5f
#define MAX_SPEED 5.0f


#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <chrono>
#include <random>
#include <fstream>
#include <SDL.h>
#include <SDL_syswm.h>


using namespace std;
using namespace std::chrono;


void raise(const char* error_name) {
	printf("%s: %s\n", error_name, SDL_GetError());
	SDL_Quit();
	exit(ERROR_EXIT_CODE);
}


uint64_t get_current_tick() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


float random_float(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}


int random_int(int a, int b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return (int)(a + r);
}


void calc_speed(int* a, int* b, float* c, float d) {
	for (int i = 0; i < 3; i++) {
		c[i] = ((float)b[i] - (float)a[i]) / d;
	}
}


int main(int argc, char* argv[]) {
	setlocale(0, "");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		raise("Error initializing SDL2");
	}
	RECT screen_rect;
	GetClientRect(GetDesktopWindow(), &screen_rect);
	SDL_Window* window = SDL_CreateWindow(
		"ScreenSaver",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		screen_rect.right,
		screen_rect.bottom,
		SDL_WINDOW_BORDERLESS
	);
	if (!window) {
		raise("Error creating window");
	}

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;
	if (hwnd) {
		SetForegroundWindow(hwnd);
	}
	else {
		raise("Could not found window handle");
	}
	SDL_Surface* screen = SDL_GetWindowSurface(window);
	SDL_Event e;
	bool running = true;

	SDL_SetWindowGrab(window, SDL_bool(true));
	SDL_ShowCursor(false);

	srand(time(0));

	int from_color[3] = { 0, 0, 0 };
	int to_color[3] = { random_int(0, 255), random_int(0, 255), random_int(0, 255) };
	float current_color[3] = { 0.0f, 0.0f, 0.0f };
	float speed = random_float(MIN_SPEED, MAX_SPEED);
	float color_speed[3] = { 0.0f, 0.0f, 0.0f };
	float current_timer = 0.0f;
	calc_speed(from_color, to_color, color_speed, speed);

	uint64_t last_tick = get_current_tick();

	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (
				e.type == SDL_QUIT ||
				e.type == SDL_KEYDOWN ||
				e.type == SDL_MOUSEBUTTONDOWN ||
				e.type == SDL_KEYUP ||
				e.type == SDL_MOUSEBUTTONUP
				) {
				running = false;
			}
		}

		uint64_t now = get_current_tick();
		float delta = (float(now - last_tick)) * 0.001f;
		last_tick = now;

		current_timer += delta;
		current_color[0] += delta * color_speed[0];
		current_color[1] += delta * color_speed[1];
		current_color[2] += delta * color_speed[2];

		if (current_timer >= speed) {
			current_timer = 0.0f;
			for (int i = 0; i < 3; i++) {
				current_color[i] = float(to_color[i]);
				from_color[i] = to_color[i];
				to_color[i] = random_int(0, 255);
			}
			speed = random_float(MIN_SPEED, MAX_SPEED);
			calc_speed(from_color, to_color, color_speed, speed);
		}

		SDL_FillRect(
			screen,
			NULL,
			SDL_MapRGB(
				screen->format,
				(int)round(current_color[0]),
				(int)round(current_color[1]),
				(int)round(current_color[2])
			)
		);

		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
