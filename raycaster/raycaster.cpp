#include <SDL.h>
#include <stdio.h>
#include <iostream>


#define frameCapActive false
const int frameCap = 1000 / 60;
#define wallAmount 8
const int height = 512;
const int width = height * 2;
const int hwidth = width / 2;
const int hheight = height / 2;
const int mapWallW = hwidth / wallAmount;
const int mapWallH = mapWallW;
const int hmapWallW = mapWallW / 2;
const int hmapWallH = mapWallH / 2;

const int fov = 60;
const int RAY_AMOUNT = 100;
const int stepSize = 600;

SDL_Window* window = nullptr;
SDL_Surface* screen = nullptr;
SDL_Renderer* renderer = nullptr;

const int playerWH = 10;
int playerX = 80;
int playerY = 80;

int map[wallAmount][wallAmount] = {
	{1, 1, 2, 3, 2, 1, 3, 1},
	{1, 0, 0, 0, 0, 2, 0, 1}, 
	{2, 0, 0, 3, 2, 1, 0, 1}, 
	{2, 3, 0, 1, 0, 0, 0, 2}, 
	{1, 0, 0, 1, 2, 3, 0, 2},
	{1, 0, 2, 2, 0, 0, 0, 3},
	{3, 0, 0, 0, 0, 0, 0, 3},
	{1, 3, 1, 2, 3, 2, 1, 2}};

int wall_colors[4][3] = {
	{0, 0, 0},
	{0xFF, 0, 0},
	{0, 0XFF, 0},
	{0, 0, 0XFF}};

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
				SDL_SetRenderDrawColor(renderer, 0x6F, 0x6F, 0x6F, SDL_ALPHA_OPAQUE);
				break;
			case 1:
				SDL_SetRenderDrawColor(renderer, 0XFF, 0, 0, SDL_ALPHA_OPAQUE);
				break;
			case 2:
				SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, SDL_ALPHA_OPAQUE);
				break;
			case 3:
				SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, SDL_ALPHA_OPAQUE);
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
	rect.x = playerX - playerWH / 2;
	rect.y = playerY - playerWH / 2;
	rect.w = rect.h = playerWH;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &rect);
}

struct hit
{
	double dist;
	int mapVal;
};

const double PI = 3.141592653;
hit* raycast(double angle)
{
	double dist = 0;
	int i, j, x, y;
	hit* result = nullptr;
	while (true)
	{
		x = (int)(playerX + dist * cos(angle));
		y = (int)(playerY + dist * sin(angle));

		i = x / mapWallW;
		j = y / mapWallH;

		if (j < wallAmount && i < wallAmount && map[j][i] > 0)
		{
			result = new hit();
			result->dist = dist;
			result->mapVal = map[j][i];
			break;
		}

		if (x < hwidth)
		{
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x, y);
		}

		dist += 1;
	}
	return result;
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
	Uint32 frame_begin, frame_end, frame_time = 0, frame_rate;
	SDL_Rect ceiling, floor;
	ceiling.x = hwidth;
	ceiling.y = 0;
	ceiling.w = hwidth;
	ceiling.h = hheight;
	floor.x = hwidth;
	floor.y = hheight;
	floor.w = hwidth;
	floor.h = hheight;

	const double ray_width = double(hwidth / RAY_AMOUNT) + 1;
	SDL_Rect wallDrawArea;
	wallDrawArea.w = ray_width;
	SDL_Event event;
	hit* hitData;
	double angle = 0, fwd, right;
	//const double fov_rads = fov / 180 * PI /  RAY_AMOUNT;
	double ray_angle;
	int unvalidated_X, unvalidated_Y, x_test, y_test;
	bool running = true;
	while (running)
	{
		fwd = angle * PI / 180;
		right = fwd + PI / 2;
		//can change during input
		unvalidated_X = playerX;
		unvalidated_Y = playerY;
		frame_begin = SDL_GetTicks();
		
		//input
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_q:
					angle -= 360 * frame_time / 1000;
					break;
				case SDLK_e:
					angle += 360 * frame_time / 1000;
					break;
				case SDLK_a:
					unvalidated_X -= stepSize * cos(right)*frame_time/1000;
					unvalidated_Y -= stepSize * sin(right)*frame_time/1000;
					break;
				case SDLK_d:
					unvalidated_X += stepSize * cos(right)*frame_time/1000;
					unvalidated_Y += stepSize * sin(right)*frame_time/1000;
					break;
				case SDLK_w:
					unvalidated_X += stepSize * cos(fwd)*frame_time/1000;
					unvalidated_Y += stepSize * sin(fwd)*frame_time/1000;
					break;
				case SDLK_s:
					unvalidated_X -= stepSize * cos(fwd)*frame_time/1000;
					unvalidated_Y -= stepSize * sin(fwd)*frame_time/1000;
					break;
				case SDLK_ESCAPE:
					running = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		x_test = unvalidated_X / mapWallW;
		y_test = unvalidated_Y / mapWallH;
		if (map[y_test][x_test] == 0)
		{
			playerX = unvalidated_X;
			playerY = unvalidated_Y;
		}

		//rendering
		SDL_SetRenderDrawColor(renderer, 191, 57, 57, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &ceiling);
		SDL_SetRenderDrawColor(renderer, 97, 98, 99, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &floor);
		drawMap();
		int i = 0;
		while (i < RAY_AMOUNT)
		{
			ray_angle = angle + (double)((float)fov / (float)RAY_AMOUNT) * (float)(i + RAY_AMOUNT * -0.5);
			ray_angle = ray_angle / 180 * PI;
			hitData = raycast(ray_angle);
			if (hitData)
			{
				wallDrawArea.h = (double)height / (hitData->dist * cos(ray_angle - fwd)) * 50.0f;
				wallDrawArea.y = hheight - wallDrawArea.h * 0.5;
				wallDrawArea.x = (int)((double)hwidth + ray_width * (double)i);
				SDL_SetRenderDrawColor(renderer, wall_colors[hitData->mapVal][0],
												wall_colors[hitData->mapVal][1],
												wall_colors[hitData->mapVal][2], SDL_ALPHA_OPAQUE);
				SDL_RenderFillRect(renderer, &wallDrawArea);
				delete hitData;
			}
			i++;
		}
		
		drawPlayer();
		
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
#if frameCapActive
		frame_rate = frame_time > 0 ? 1000 / (frameCap + frame_time) : 60;
#else
		frame_rate = frame_time > 0 ? 1000 / frame_time : 1000;
#endif
		std::cout << "Frame time: " << frame_time << " Frame rate: " << frame_rate << std::endl;
#if frameCapActive
		//framerate cap
		SDL_Delay(frameCap - frame_time);
#endif
	}
	

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}