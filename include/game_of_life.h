#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H


#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "settings.h"


#define DEAD 0
#define LIVE 1


typedef struct {
	char *grid;
	int width;
	int height;
	int rows;
	int cell_size;
	int cols;
	bool should_update;
} Game;


Game *Game_Init(int width, int height);
void Game_Randomize_Grid(Game* game);
int Game_Get_Grid_Index(Game* game, int row, int col);
void Game_Print_Grid(Game* game);
void Game_Draw_Grid(Game *game, SDL_Renderer *renderer);
void Game_Draw_Mesh(Game *game, SDL_Renderer *renderer);
void Game_Update_Grid(Game* game);
void Game_Set_Cells_DEAD(Game *game);
void Game_Reset(Game **game, int new_width, int new_height);
void Game_Update_Cell(Game *game, float x, float y);
void Game_Free(Game* game);


#endif
