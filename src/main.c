#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "../include/game_of_life.h"


int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	Game *game = NULL;
	bool is_running = true;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_CreateWindowAndRenderer("Game Of Life", 1200, 900, 0, &window, &renderer);

	game = Game_Init();

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
						case SDLK_V:
							Game_Print_Grid(game);
							break;
						case SDLK_P:
							game->should_update = !game->should_update;
							break;
						case SDLK_S:
							Game_Update_Grid(game);
							break;
						case SDLK_C:
							Game_Set_Cells_DEAD(game);
							break;
					}
					break;
				case SDL_EVENT_MOUSE_MOTION:
					if (game->mode == NAVIGATION_MODE)
						Game_Set_XY_Cur_Offset(game, event.button.x, event.button.y);
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					Game_Set_Mode(game, NAVIGATION_MODE);
					Game_Set_XY(game, event.button.x, event.button.y);
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if (game->nav_state->cur_x_offset == 0 && game->nav_state->cur_y_offset == 0)
						Game_Update_Cell(game, event.motion.x, event.motion.y);
					Game_Set_Mode(game, DEFAULT_MODE);
					Game_Update_XY_Offset(game);
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					float mouse_x = 0;
					float mouse_y = 0;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					Game_Zoom(game, mouse_x, mouse_y, event.wheel.y > 0);
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, BG_COLOR);
		SDL_RenderClear(renderer);
		Game_Draw_Grid(game, renderer);
		Game_Draw_Mesh(game, renderer);
		SDL_RenderPresent(renderer);
		// SDL_Delay(25);
		
		if (game->should_update)
			Game_Update_Grid(game);
	}

	Game_Free(game);
    SDL_DestroyWindow(window);
	SDL_Quit();
}


