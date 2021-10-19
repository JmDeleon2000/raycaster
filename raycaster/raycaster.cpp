#include <SDL.h>
#include <stdio.h>
#include <iostream>


#define frameCapActive false
#define uvDebug false
const int frameCap = 100;
#define wallAmount 8
const int height = 512;
const int width = height * 2;
const int hwidth = width / 2;
const int hheight = height / 2;
const int mapWallW = hwidth / wallAmount;
const int mapWallH = mapWallW;
const int hmapWallW = mapWallW / 2;
const int hmapWallH = mapWallH / 2;
const double MAX_DIST = 100;

const int fov = 60;
const int RAY_AMOUNT = 100;
const int stepSize = 600;

SDL_Window* window = nullptr;
//SDL_Surface* screen = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* wall1;
SDL_Texture* wall2;
SDL_Texture* wall3;

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
	{1, 3, 1, 2, 3, 2, 1, 2} };

SDL_Texture* textureMap[wallAmount][wallAmount];

int wall_colors[4][3] = {
	{0, 0, 0},
	{0xFF, 0, 0},
	{0, 0XFF, 0},
	{0, 0, 0XFF} };


void init_text_ref()
{
	int i, j = 0;
	while (j < wallAmount)
	{
		i = 0;
		while (i < wallAmount)
		{
			switch (map[j][i])
			{
			case 1:
				textureMap[j][i] = wall1;
				break;
			case 2:
				textureMap[j][i] = wall2;
				break;
			case 3:
				textureMap[j][i] = wall3;
				break;
			default:
				textureMap[j][i] = nullptr;
				break;
			}
			i++;
		}
		j++;
	}

}

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
	SDL_Texture* mapVal;
	double uvx;
};

float frac(float a)
{
	return a - (int)a;
}
const double PI = 3.141592653;
bool raycast(double angle, hit* out)
{
	double dist = 0;
	const double stepsize = 1;
	int i, j;
	double x = playerX, y = playerY;
	double deltax, deltay;
	const double stepx = stepsize * cos(angle);
	const double stepy = stepsize * sin(angle);

	while (true)
	{
		x += stepx;
		y += stepy;

		i = (int)x / mapWallW;
		j = (int)y / mapWallH;

		if (j < wallAmount && i < wallAmount && map[j][i] > 0)
		{
			deltax = frac((float)x / (float)mapWallW);
			deltay = frac((float)y / (float)mapWallH);
			//new(out) hit();
			out->dist = dist;
			out->mapVal = textureMap[j][i];
			out->uvx = frac(deltax + deltay);


#if uvDebug
			std::cout << deltax << " " << deltay << " ";
#endif
			SDL_RenderDrawLine(renderer, playerX, playerY, (int)x, (int)y);
			return true;
		}

		dist += 1;
	}
	return false;
}


bool mainMenu()
{
	bool running = true;
	SDL_Event event;
	SDL_Surface* sbg = SDL_LoadBMP("menuBG.bmp");
	SDL_Texture* bg = SDL_CreateTextureFromSurface(renderer, sbg);
	SDL_FreeSurface(sbg);


	while (running)
	{
		//SDL_RenderCopy(renderer, bg, NULL, &all);
		//SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		//SDL_RenderFillRect(renderer, NULL);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
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

		SDL_Delay(100);
	}


	return true;
}

bool dispMenu() 
{
	SDL_Surface* smenuBG = SDL_LoadBMP("menuBG.bmp");
	SDL_Texture* menuBG = SDL_CreateTextureFromSurface(renderer, smenuBG);
	SDL_FreeSurface(smenuBG);

	SDL_Surface* splaybtn = SDL_LoadBMP("playButton.bmp");
	SDL_Texture* playbtn = SDL_CreateTextureFromSurface(renderer, splaybtn);
	SDL_FreeSurface(splaybtn);

	SDL_Surface* squitbtn = SDL_LoadBMP("quitButton.bmp");
	SDL_Texture* quitbtn = SDL_CreateTextureFromSurface(renderer, squitbtn);
	SDL_FreeSurface(squitbtn);

	SDL_Rect pbda;
	pbda.x = hwidth - 64;
	pbda.y = hheight - 80;
	pbda.w = 128;
	pbda.h = 64;
	SDL_Rect qbda;
	qbda.x = hwidth - 64;
	qbda.y = hheight + 16;
	qbda.w = 128;
	qbda.h = 64;

	SDL_Event event;
	bool menu = true;
	bool play_btn_in = false;
	bool quit_btn_in = false;
	while (menu)
	{
		SDL_RenderCopy(renderer, menuBG, NULL, NULL);
		if (play_btn_in) SDL_SetTextureColorMod(playbtn, 0xAF, 0xAF, 0xAF);
		else SDL_SetTextureColorMod(playbtn, 0xFF, 0xFF, 0xFF);
		SDL_RenderCopy(renderer, playbtn, NULL, &pbda);
		if (quit_btn_in) SDL_SetTextureColorMod(quitbtn, 0xAF, 0xAF, 0xAF);
		else SDL_SetTextureColorMod(quitbtn, 0xFF, 0xFF, 0xFF);
		SDL_RenderCopy(renderer, quitbtn, NULL, &qbda);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEMOTION:
				SDL_MouseMotionEvent mme = event.motion;
				play_btn_in = mme.x > pbda.x &&
					mme.y > pbda.y &&
					mme.x < pbda.x + pbda.w &&
					mme.y < pbda.y + pbda.h;
				quit_btn_in = mme.x > qbda.x &&
					mme.y > qbda.y &&
					mme.x < qbda.x + qbda.w &&
					mme.y < qbda.y + qbda.h;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (play_btn_in)
					menu = false;
				if (quit_btn_in)
				{
					SDL_DestroyTexture(menuBG);
					SDL_DestroyTexture(playbtn);
					SDL_DestroyTexture(quitbtn);
					return false;
				}
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
					menu = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	SDL_DestroyTexture(menuBG);
	SDL_DestroyTexture(playbtn);
	SDL_DestroyTexture(quitbtn);
	return true;
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



	//wall textures
	SDL_Surface* swall1 = SDL_LoadBMP("wall1.bmp");
	wall1 = SDL_CreateTextureFromSurface(renderer, swall1);
	SDL_FreeSurface(swall1);
	SDL_Surface* swall2 = SDL_LoadBMP("wall2.bmp");
	wall2 = SDL_CreateTextureFromSurface(renderer, swall2);
	SDL_FreeSurface(swall2);
	SDL_Surface* swall3 = SDL_LoadBMP("wall3.bmp");
	wall3 = SDL_CreateTextureFromSurface(renderer, swall3);
	SDL_FreeSurface(swall3);
	init_text_ref();



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

	const double ray_width = double(hwidth / RAY_AMOUNT) + 100 / (double)RAY_AMOUNT;
	SDL_Rect wallDrawArea;
	wallDrawArea.w = ray_width;
	SDL_Rect textureCrop;
	textureCrop.h = 128;
	textureCrop.w = double((double)swall1->w / (double)RAY_AMOUNT) + 100 / (double)RAY_AMOUNT;
	textureCrop.y = 0;
	SDL_Event event;
	hit hitData;
	double angle = 0, fwd, right;
	double ray_angle;
	int unvalidated_X, unvalidated_Y, x_test, y_test;
	int colorMod;
	bool running = dispMenu();

	
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
					unvalidated_X -= stepSize * cos(right) * frame_time / 1000;
					unvalidated_Y -= stepSize * sin(right) * frame_time / 1000;
					break;
				case SDLK_d:
					unvalidated_X += stepSize * cos(right) * frame_time / 1000;
					unvalidated_Y += stepSize * sin(right) * frame_time / 1000;
					break;
				case SDLK_w:
					unvalidated_X += stepSize * cos(fwd) * frame_time / 1000;
					unvalidated_Y += stepSize * sin(fwd) * frame_time / 1000;
					break;
				case SDLK_s:
					unvalidated_X -= stepSize * cos(fwd) * frame_time / 1000;
					unvalidated_Y -= stepSize * sin(fwd) * frame_time / 1000;
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
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		while (i < RAY_AMOUNT)
		{
			ray_angle = angle + (double)((double)fov / (double)RAY_AMOUNT) * (double)(i + RAY_AMOUNT * -0.5);
			ray_angle = ray_angle / 180 * PI;
			if (raycast(ray_angle, &hitData))
			{
				wallDrawArea.h = (double)height / (hitData.dist * cos(ray_angle - fwd)) * 50.0f;
				wallDrawArea.y = hheight - wallDrawArea.h * 0.5;
				wallDrawArea.x = (int)((double)hwidth + ray_width * (double)i);
				textureCrop.x = (int)((float)hitData.uvx * (float)128);
				colorMod = 0xFF / (hitData.dist / MAX_DIST);
				colorMod = colorMod < 0xFF ? colorMod : 0xFF;
				SDL_SetTextureColorMod(hitData.mapVal, colorMod, colorMod, colorMod);
#if uvDebug
				std::cout << hitData.uvx << " " << textureCrop.x << " ";
#endif
				SDL_RenderCopy(renderer, hitData.mapVal, &textureCrop, &wallDrawArea);
			}
			i++;
		}

		drawPlayer();

		SDL_RenderPresent(renderer);

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

	SDL_DestroyTexture(wall1);
	SDL_DestroyTexture(wall2);
	SDL_DestroyTexture(wall3);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}