#include <stdlib.h>
#include <time.h>
#include "../include/game_of_life.h"


Game *Game_Init(int width, int height)
{
	Game* game = (Game*)malloc(sizeof(Game));
	if (game == NULL)
		return NULL;

	game->width     = width;
	game->height    = height;
	game->cell_size = 2;
	game->rows      = height/game->cell_size;
	game->cols      = width/game->cell_size;
	game->grid      = (char*)malloc(game->rows * game->cols * sizeof(char));

	Game_Randomize_Grid(game);

	return game;
}


void Game_Randomize_Grid(Game* game)
{
	game->should_update = false;

	srand(time(NULL));

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			int index = Game_Get_Grid_Index(game, row, col);
			(game->grid)[index] = rand() % 2 == 1 ? LIVE : DEAD;
		}
	}
}


int Game_Get_Grid_Index(Game* game, int row, int col)
{
	return row*game->cols + col;
}


void Game_Print_Grid(Game* game)
{
	int index = 0;
	char* text = (char*)calloc(game->rows * (game->cols + 1), sizeof(char));

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			index = Game_Get_Grid_Index(game, row, col) + row;
			text[index] = game->grid[index] == LIVE ? 'x' : '.';
		}

		index++;
		text[index] = '\n';
	}

	SDL_LogInfo(0, "\nwidth: %d, height: %d, rows: %d, cols: %d\n\n", game->width, game->height, game->rows, game->cols);
	SDL_LogInfo(0, "%s\n", text);

	free(text);
}


void Game_Draw_Grid(SDL_Renderer *renderer, Game *game)
{
	SDL_SetRenderDrawColor(renderer, 0x22, 0x8c, 0x2c, 0);

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			int index = Game_Get_Grid_Index(game, row, col);
			if (game->grid[index] == LIVE)
				SDL_RenderFillRect(renderer, &(SDL_FRect){col * game->cell_size, row * game->cell_size, game->cell_size, game->cell_size});
		}
	}
}


void Game_Update_Grid(Game* game)
{
	if (! game->should_update)
		return;

	int index = 0;
	char *new_grid = (char*)malloc(game->rows * game->cols * sizeof(char)); 
	memcpy(new_grid, game->grid, game->rows * game->cols * sizeof(char));

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			int neighbour_count = 0;
			for (int row_offset = -1; row_offset < 2; row_offset++)
			{
				for (int col_offset = -1; col_offset < 2; col_offset++)
				{
					if (row_offset == 0 && col_offset == 0)
						continue;
					index = Game_Get_Grid_Index(game, (row + game->rows + row_offset) % game->rows, (col + game->cols + col_offset) % game->cols);
					if (game->grid[index] == LIVE)
						neighbour_count++;
				}
			}

			index = Game_Get_Grid_Index(game, row, col);
			if (game->grid[index] == LIVE)
			{
				if (neighbour_count < 2 || neighbour_count > 3)
					new_grid[index] = DEAD;
			}
			else
			{
				if (neighbour_count == 3)
					new_grid[index] = LIVE;
			}
		}
	}

	free(game->grid);
	game->grid = new_grid;
}


void Game_Reset(Game **game, int new_width, int new_height)
{
	Game_Free(*game);
	*game = Game_Init(new_width, new_height);
}


void Game_Free(Game *game)
{
	if (game == NULL)
		return;

	if (game->grid != NULL)
		free(game->grid);

	free(game);
}
