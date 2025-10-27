#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "../include/game_of_life.h"


int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	int width = 0;
	int height = 0;
	Game *game = NULL;
	bool is_running = true;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_CreateWindowAndRenderer("Game Of Life", 1200, 900, 0, &window, &renderer);
	SDL_GetWindowSize(window, &width, &height);

	game = Game_Init(width, height);

	while (is_running)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
					is_running = false;
					break;
				case SDL_EVENT_KEY_UP:
					switch (event.key.key)
					{
						case SDLK_ESCAPE:
							is_running = false;
							break;
						case SDLK_Q:
							is_running = false;
							break;
						case SDLK_R:
							Game_Randomize_Grid(game);
							break;
						case SDLK_P:
							Game_Print_Grid(game);
							break;
						case SDLK_U:
							game->should_update = !game->should_update;
							break;
					}
					break;
				case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
					SDL_GetWindowSize(window, &width, &height);
					Game_Reset(&game, width, height);
					break;
				case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
					SDL_GetWindowSize(window, &width, &height);
					Game_Reset(&game, width, height);
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(renderer);
		Game_Draw_Grid(renderer, game);
		SDL_RenderPresent(renderer);
		SDL_Delay(25);
		
		Game_Update_Grid(game);
	}

	Game_Free(game);
    SDL_DestroyWindow(window);
	SDL_Quit();
}


