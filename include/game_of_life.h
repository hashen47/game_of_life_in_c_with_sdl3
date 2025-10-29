#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H


#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include "settings.h"


#define DEAD 0
#define LIVE 1


enum Game_Mode
{
	DEFAULT_MODE,
	NAVIGATION_MODE
};


typedef struct {
	int x;
	int y;
	int x_offset;
	int y_offset;
	int cur_x_offset;
	int cur_y_offset;
	int zoom_x_offset;
	int zoom_y_offset;
} Navigation_State;


typedef struct {
	char *grid;
	int rows;
	int cols;
	int cell_size;
	enum Game_Mode mode;
	Navigation_State *nav_state;
	bool should_update;
	int cell_size_multiplier;
} Game;


Game *Game_Init(void);
void Game_Randomize_Grid(Game* game);
int Game_Get_Grid_Index(Game* game, int row, int col);
int Game_Get_Grid_Index_By_Mouse_Pos(Game* game, float mouse_x, float mouse_y, int *row, int *col);
void Game_Print_Grid(Game* game);
void Game_Draw_Grid(Game *game, SDL_Renderer *renderer);
void Game_Draw_Mesh(Game *game, SDL_Renderer *renderer);
void Game_Update_Grid(Game* game);
void Game_Set_Cells_DEAD(Game *game);
void Game_Set_XY(Game *game, float mouse_x, float mouse_y);
void Game_Set_XY_Cur_Offset(Game *game, float mouse_x, float mouse_y);
void Game_Update_XY_Offset(Game *game);
void Game_Zoom(Game *game, float mouse_x, float mouse_y, bool is_zoom_in);
void Game_Reset(Game **game);
void Game_Update_Cell(Game *game, float x, float y);
void Game_Set_Mode(Game *game, enum Game_Mode mode);
void Game_Free(Game* game);


#endif
