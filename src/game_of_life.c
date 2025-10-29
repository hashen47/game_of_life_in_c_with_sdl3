#include <stdlib.h>
#include <time.h>
#include "../include/game_of_life.h"


Game *Game_Init(void)
{
	Game* game = (Game*)malloc(sizeof(Game));
	if (game == NULL)
		return NULL;

	game->nav_state = (Navigation_State*)malloc(sizeof(Navigation_State));
	if (game->nav_state == NULL)
		return NULL;

	game->cell_size            = CELL_SIZE;
	game->rows                 = ROWS;
	game->cols                 = COLS;
	game->grid                 = (char*)malloc(ROWS * COLS * sizeof(char));
	game->should_update        = true;
	game->mode                 = DEFAULT_MODE;

	// NOTE: this initial value should not be changed
	game->cell_size_multiplier = 1;

	game->nav_state->x             = 0;
	game->nav_state->y             = 0;
	game->nav_state->x_offset      = 0;
	game->nav_state->y_offset      = 0;
	game->nav_state->cur_x_offset  = 0;
	game->nav_state->cur_y_offset  = 0;
	game->nav_state->zoom_x_offset = 0;
	game->nav_state->zoom_y_offset = 0;

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


int Game_Get_Grid_Index_By_Mouse_Pos(Game* game, float mouse_x, float mouse_y, int *row, int *col)
{
	int x = (int)mouse_x;
	int y = (int)mouse_y;

	if (game->nav_state->y_offset < 0)
		y += -1 * game->nav_state->y_offset;
	else
		y -= game->nav_state->y_offset;

	if (game->nav_state->x_offset < 0)
		x += -1 * game->nav_state->x_offset;
	else
		x -= game->nav_state->x_offset;

	int _row = y/(game->cell_size * game->cell_size_multiplier);
	int _col = x/(game->cell_size * game->cell_size_multiplier);

	if (row != NULL)
		*row = _row;
	if (col != NULL)
		*col = _col;

	int index = Game_Get_Grid_Index(game, _row, _col);
	return index;
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

	SDL_LogInfo(0, "\nrows: %d, cols: %d\n\n", game->rows, game->cols);
	SDL_LogInfo(0, "%s\n", text);

	free(text);
}


void Game_Draw_Grid(Game *game, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, CELL_COLOR);

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			int index = Game_Get_Grid_Index(game, row, col);
			if (game->grid[index] == LIVE)
			{
				int x = game->nav_state->x_offset + (col * game->cell_size * game->cell_size_multiplier);
				int y = game->nav_state->y_offset + (row * game->cell_size * game->cell_size_multiplier);
				int w = game->cell_size * game->cell_size_multiplier;
				int h = game->cell_size * game->cell_size_multiplier;

				if (game->mode == NAVIGATION_MODE)
				{
					x += game->nav_state->cur_x_offset;
					y += game->nav_state->cur_y_offset;
				}

				SDL_RenderFillRect(renderer, &(SDL_FRect){x, y, w, h});
			}
		}
	}
}


void Game_Draw_Mesh(Game *game, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, MESH_COLOR);

	for (int row = 0; row <= game->rows; row++)
	{
		int start_x = game->nav_state->x_offset + 0;
		int start_y = game->nav_state->y_offset + (row * game->cell_size * game->cell_size_multiplier);
		int end_x   = game->nav_state->x_offset + (game->cols * game->cell_size * game->cell_size_multiplier);
		int end_y   = game->nav_state->y_offset + (row * game->cell_size * game->cell_size_multiplier);

		if (game->mode == NAVIGATION_MODE)
		{
			start_x += game->nav_state->cur_x_offset;
			start_y += game->nav_state->cur_y_offset;
			end_x   += game->nav_state->cur_x_offset;
			end_y   += game->nav_state->cur_y_offset;
		}

		SDL_RenderLine(renderer, start_x, start_y, end_x, end_y);
	}

	for (int col = 0; col <= game->cols; col++)
	{
		int start_x = game->nav_state->x_offset + (col * game->cell_size * game->cell_size_multiplier);
		int start_y = game->nav_state->y_offset + 0;
		int end_x   = game->nav_state->x_offset + (col * game->cell_size * game->cell_size_multiplier);
		int end_y   = game->nav_state->y_offset + (game->rows * game->cell_size * game->cell_size_multiplier);

		if (game->mode == NAVIGATION_MODE)
		{
			start_x += game->nav_state->cur_x_offset;
			start_y += game->nav_state->cur_y_offset;
			end_x   += game->nav_state->cur_x_offset;
			end_y   += game->nav_state->cur_y_offset;
		}

		SDL_RenderLine(renderer, start_x, start_y, end_x, end_y);
	}
}


void Game_Update_Grid(Game* game)
{
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


void Game_Set_Cells_DEAD(Game *game)
{
	game->should_update = false;

	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			int index = Game_Get_Grid_Index(game, row, col);
			game->grid[index] = DEAD;
		}
	}
}


void Game_Set_XY(Game *game, float mouse_x, float mouse_y)
{
	game->nav_state->x = (int)mouse_x;
	game->nav_state->y = (int)mouse_y;
}


void Game_Set_XY_Cur_Offset(Game *game, float mouse_x, float mouse_y)
{
	game->nav_state->cur_x_offset = (int)mouse_x - game->nav_state->x;
	game->nav_state->cur_y_offset = (int)mouse_y - game->nav_state->y;
}


void Game_Update_XY_Offset(Game *game)
{
	game->nav_state->x_offset += game->nav_state->cur_x_offset;
	game->nav_state->y_offset += game->nav_state->cur_y_offset;
	game->nav_state->cur_x_offset = 0;
	game->nav_state->cur_y_offset = 0;
}


void Game_Reset(Game **game)
{
	Game_Free(*game);
	*game = Game_Init();
}


void Game_Update_Cell(Game *game, float mouse_x, float mouse_y)
{
	int index = Game_Get_Grid_Index_By_Mouse_Pos(game, mouse_x, mouse_y, NULL, NULL);

	if (game->grid[index] == LIVE)
		game->grid[index] = DEAD;
	else
		game->grid[index] = LIVE;
}


void Game_Set_Mode(Game *game, enum Game_Mode mode)
{
	game->mode = mode;
}


void Game_Zoom(Game *game, float mouse_x, float mouse_y, bool is_zoom_in)
{
	int row = 0;
	int col = 0;
	(void)Game_Get_Grid_Index_By_Mouse_Pos(game, mouse_x, mouse_y, &row, &col);


	if (is_zoom_in)
	{
		game->cell_size_multiplier++;
		if (row < 0 || col < 0 || row >= game->rows || col >= game->cols)
			return;

		int new_zoom_x_offset  = col * (game->cell_size * (game->cell_size_multiplier-1));
		int prev_zoom_x_offset = col * (game->cell_size * (game->cell_size_multiplier-2));
		game->nav_state->x_offset += prev_zoom_x_offset;
		game->nav_state->x_offset -= new_zoom_x_offset;
		game->nav_state->zoom_x_offset = new_zoom_x_offset;

		int new_zoom_y_offset =  row * (game->cell_size * (game->cell_size_multiplier-1));
		int prev_zoom_y_offset = row * (game->cell_size * (game->cell_size_multiplier-2));
		game->nav_state->y_offset += prev_zoom_y_offset;
		game->nav_state->y_offset -= new_zoom_y_offset;
		game->nav_state->zoom_y_offset = new_zoom_y_offset;
	}
	else
	{
		if (game->cell_size_multiplier > 1)
		{
			game->cell_size_multiplier--;

			if (row < 0 || col < 0 || row >= game->rows || col >= game->cols)
				return;

			int new_zoom_x_offset  = col * (game->cell_size * (game->cell_size_multiplier-1));
			int prev_zoom_x_offset = col * (game->cell_size * (game->cell_size_multiplier-2));
			game->nav_state->x_offset -= prev_zoom_x_offset;
			game->nav_state->x_offset += new_zoom_x_offset;
			game->nav_state->zoom_x_offset = new_zoom_x_offset;

			int new_zoom_y_offset =  row * (game->cell_size * (game->cell_size_multiplier-1));
			int prev_zoom_y_offset = row * (game->cell_size * (game->cell_size_multiplier-2));
			game->nav_state->y_offset -= prev_zoom_y_offset;
			game->nav_state->y_offset += new_zoom_y_offset;
			game->nav_state->zoom_y_offset = new_zoom_y_offset;
		}
	}
}


void Game_Free(Game *game)
{
	if (game == NULL)
		return;

	if (game->grid != NULL)
		free(game->grid);

	free(game);
}
