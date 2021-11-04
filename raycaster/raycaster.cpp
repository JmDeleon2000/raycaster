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
const float MAX_DIST = 1000;

const int fov = 60;
const int RAY_AMOUNT = 100;
const int stepSize = 600;

SDL_Window* window = nullptr;
//SDL_Surface* screen = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* wall1;
SDL_Texture* wall2;
SDL_Texture* wall3;
SDL_Texture* ceilingText;
SDL_Texture* floorText;
SDL_Texture* powerUpIcon;
SDL_Texture* cacodemon;

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

struct enemy
{
	int x = 0;
	int y = 0;
	SDL_Texture* text = nullptr;
};

enemy enemies[3];


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
			case-1:
				textureMap[j][i] = powerUpIcon;
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
	float dist;
	SDL_Texture* mapVal;
	float uvx;
};

float frac(float a)
{
	return a - (int)a;
}
const float PI = 3.141592653;
bool raycast(float angle, hit* out)
{
	float dist = 0;
	const float stepsize = 1;
	int i, j;
	float x = playerX, y = playerY;
	float deltax, deltay;
	const float stepx = stepsize * cosf(angle);
	const float stepy = stepsize * sinf(angle);

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

bool raycast(int xstep, int ystep, float dist, hit*out)
{
	float d = 0;
	float x, y;
	x = playerX;
	y = playerY;
	while (d < dist)
	{
		x += xstep;
		y += ystep;
		d += 1;
	}
	return false;
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
	{
		SDL_Surface* swall1 = SDL_LoadBMP("wall1.bmp");
		wall1 = SDL_CreateTextureFromSurface(renderer, swall1);
		SDL_FreeSurface(swall1);
		SDL_Surface* swall2 = SDL_LoadBMP("wall2.bmp");
		wall2 = SDL_CreateTextureFromSurface(renderer, swall2);
		SDL_FreeSurface(swall2);
		SDL_Surface* swall3 = SDL_LoadBMP("wall3.bmp");
		wall3 = SDL_CreateTextureFromSurface(renderer, swall3);
		SDL_FreeSurface(swall3);
		SDL_Surface* sceiling = SDL_LoadBMP("ceiling.bmp");
		ceilingText = SDL_CreateTextureFromSurface(renderer, sceiling);
		SDL_FreeSurface(sceiling);
		SDL_Surface* sfloor = SDL_LoadBMP("floor.bmp");
		floorText = SDL_CreateTextureFromSurface(renderer, sfloor);
		SDL_FreeSurface(sfloor);
		SDL_Surface* spowerUpIcon = SDL_LoadBMP("powerUP.bmp");
		SDL_SetColorKey(spowerUpIcon, SDL_TRUE, SDL_MapRGB(spowerUpIcon->format, 255, 0, 255));
		powerUpIcon = SDL_CreateTextureFromSurface(renderer, spowerUpIcon);
		SDL_FreeSurface(spowerUpIcon);
		SDL_Surface* scacodemon = SDL_LoadBMP("cacodemon.bmp");
		SDL_SetColorKey(scacodemon, SDL_TRUE, SDL_MapRGB(scacodemon->format, 255, 0, 255));
		cacodemon = SDL_CreateTextureFromSurface(renderer, scacodemon);
		SDL_FreeSurface(scacodemon);
	}
	init_text_ref();
	

	enemies[0].x = playerX * 2;
	enemies[0].y = playerY;
	enemies[0].text = powerUpIcon;
	enemies[1].x = playerX;
	enemies[1].y = 500;
	enemies[1].text = cacodemon;
	enemies[2].x = playerX * 5;
	enemies[2].y = playerY*10;
	enemies[2].text = powerUpIcon;
	
	Uint32 frame_begin, frame_end, frame_time = 0, frame_rate;
	SDL_Rect ceilingRect, floorRect;
	ceilingRect.x = hwidth;
	ceilingRect.y = 0;
	ceilingRect.w = hwidth;
	ceilingRect.h = hheight;
	floorRect.x = hwidth;
	floorRect.y = hheight;
	floorRect.w = hwidth;
	floorRect.h = hheight;

	const float ray_width = float(hwidth / RAY_AMOUNT) + 100 / (float)RAY_AMOUNT;
	SDL_Rect wallDrawArea;
	wallDrawArea.w = ray_width;
	SDL_Rect textureCrop;
	textureCrop.h = 128;
	textureCrop.w = float((float)128 / (float)RAY_AMOUNT) + 100 / (float)RAY_AMOUNT;
	textureCrop.y = 0;
	SDL_Event event;
	SDL_Point rotationSource;
	rotationSource.x = hwidth / 2;
	rotationSource.y = hheight;
	hit hitData;
	float angle = 0, fwd, right;
	float ray_angle;
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
					unvalidated_X -= stepSize * cosf(right) * frame_time / 1000;
					unvalidated_Y -= stepSize * sinf(right) * frame_time / 1000;
					break;
				case SDLK_d:
					unvalidated_X += stepSize * cosf(right) * frame_time / 1000;
					unvalidated_Y += stepSize * sinf(right) * frame_time / 1000;
					break;
				case SDLK_w:
					unvalidated_X += stepSize * cosf(fwd) * frame_time / 1000;
					unvalidated_Y += stepSize * sinf(fwd) * frame_time / 1000;
					break;
				case SDLK_s:
					unvalidated_X -= stepSize * cosf(fwd) * frame_time / 1000;
					unvalidated_Y -= stepSize * sinf(fwd) * frame_time / 1000;
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
		//https://wiki.libsdl.org/SDL_RenderCopyEx
		// altura = altura*sen(x) + ancho * cos(x)
		//SDL_RenderCopyEx(renderer, floorText, NULL, &floorRect, -angle, NULL, SDL_FLIP_NONE);
		//SDL_RenderCopyEx(renderer, ceilingText, NULL, &ceilingRect, -angle, NULL, SDL_FLIP_NONE);
		SDL_RenderCopy(renderer, floorText, NULL, &floorRect);
		SDL_RenderCopy(renderer, ceilingText, NULL, &ceilingRect);
		drawMap();
		int i = 0;
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		while (i < RAY_AMOUNT)
		{
			ray_angle = angle + (float)((float)fov / (float)RAY_AMOUNT) * (float)(i + RAY_AMOUNT * -0.5);
			ray_angle = ray_angle / 180 * PI;
			if (raycast(ray_angle, &hitData))
			{
				wallDrawArea.h = (float)height / (hitData.dist * cosf(ray_angle - fwd)) * 50.0f;
				wallDrawArea.y = hheight - wallDrawArea.h * 0.5;
				wallDrawArea.x = (int)((float)hwidth + ray_width * (float)i);
				textureCrop.x = (int)((float)hitData.uvx * (float)128);
				colorMod = 0xFF / (hitData.dist / MAX_DIST);
				colorMod = colorMod < 0xFF ? colorMod : 0xFF;
#if uvDebug
				std::cout << hitData.uvx << " " << textureCrop.x << " ";
#endif
				SDL_SetTextureColorMod(hitData.mapVal, colorMod, colorMod, colorMod);
				SDL_RenderCopy(renderer, hitData.mapVal, &textureCrop, &wallDrawArea);
			}
			i++;
		}
		i = 0;
		//powerUp/enemies
		while (i < 2)
		{	
			float ABx, ABy;
			ABx = enemies[i].x - playerX;
			ABy = enemies[i].y - playerY;
			float dot = (float)playerX * (float)ABx + (float)playerY * (float)ABy;
			dot /= sqrtf(playerX * playerX + playerY * playerY);
			dot /= sqrtf(ABx * ABx + ABy * ABy);
			float spriteAngle = acosf(dot) * 180 / PI;
			float angleDiff = (int)(angle + 30) % 360 - spriteAngle;
			std::cout << " detected " << angleDiff;
			if (angleDiff >= -30 && angleDiff <= 30) 
			{
				SDL_RenderCopy(renderer, enemies[i].text, NULL, NULL);

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