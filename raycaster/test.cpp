#include <SDL.h>
#include <stdio.h>
#include <iostream>


#define frameCap true
const int height = 512;
const int width = height * 2;
const int hwidth = width / 2;
const int mapWallW = hwidth / 8;
const int mapWallH = mapWallW;
const int fov = 60;
const int RAY_AMOUNT = 20;
SDL_Window* window = nullptr;
SDL_Surface* screen = nullptr;
SDL_Renderer* renderer = nullptr;

const int playerWH = 10;
int playerX = 70;
int playerY = 70;

int map[8][8] = {
	{1, 1, 2, 3, 2, 1, 3, 1},
	{1, 0, 0, 0, 0, 2, 0, 1}, 
	{2, 0, 0, 3, 2, 1, 0, 1}, 
	{2, 3, 0, 1, 0, 0, 0, 2}, 
	{1, 0, 0, 1, 2, 3, 0, 2},
	{1, 0, 2, 2, 0, 0, 0, 3},
	{3, 0, 0, 0, 0, 0, 0, 3},
	{1, 3, 1, 2, 3, 2, 1, 2}};


//https://wiki.libsdl.org/SDL_RenderDrawLine
void drawMap() 
{
	
	SDL_Rect rect;
	int i = 0, j;
	rect.w = mapWallW;
	rect.h = mapWallH;
	while (i < 8)
	{
		j = 0;
		while (j < 8)
		{
			rect.x = mapWallW * j;
			rect.y = mapWallH * i;
			switch (map[i][j])
			{
			case 0:
				SDL_SetRenderDrawColor(renderer, 0x6F, 0x6F, 0x6F, 1);
				break;
			case 1:
				SDL_SetRenderDrawColor(renderer, 0XFF, 0, 0, 1);
				break;
			case 2:
				SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 1);
				break;
			case 3:
				SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 1);
				break;
			default:
				break;
			}

			SDL_RenderFillRect(renderer, &rect);
			j++;
		}
		i++;
	}
}

void drawPlayer() 
{
	SDL_Rect rect;
	rect.x = playerX;
	rect.y = playerY;
	rect.w = rect.h = playerWH;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;

	window = SDL_CreateWindow("Doom de la Mega", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr)
		return -1;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
		return -1;


	
	screen = SDL_GetWindowSurface(window);
	int r = 0xFF, g = 0xFF, b = 0xFF;
	int i = 0;
	Uint32 frame_begin, frame_end, frame_time = 0, frame_rate;
	SDL_Rect rect;
	rect.x = 0;
	bool running = true;
	while (running)
	{
		frame_begin = SDL_GetTicks();
		

		SDL_SetRenderDrawColor(renderer, r, g, b, 1);
		SDL_RenderClear(renderer);
		drawMap();
		drawPlayer();
		i++;
		r -= 1;
		g -= 1;
		b -= 1;
		SDL_RenderPresent(renderer);
		/*SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}*/

		//performance tracking
		frame_end = SDL_GetTicks();
		frame_time = frame_end - frame_begin;
#if frameCap
		frame_rate = frame_time > 0 ? 6000 / (100 + frame_time) : 60;
#else
		frame_rate = frame_time > 0 ? 6000 / frame_time : 6000;
#endif
		std::cout << "Frame time: " << frame_time << " Frame rate: " << frame_rate << std::endl;
		//framerate cap
#if frameCap
		SDL_Delay(100 - frame_time);
#endif
		running = i < 150;
	}
	

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}