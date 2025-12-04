#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#define TILE_SIZE 32
#define BOARD_COLS 30
#define BOARD_ROWS 30
#define MAX_LAYERS 4

typedef struct
{
    int layers[MAX_LAYERS];     // indices dans gTileTextures
    int layerCount;             // nombre de couches utilisées
} Tile;

typedef struct
{
    Tile tiles[BOARD_ROWS][BOARD_COLS];
} Board;


typedef struct {
    int x, y;           // position sur le board
    int textureIndex;   // index de la texture dans gTileTextures
} Player;

extern Player gPlayer;


void GameInit(Board *board);
void GameUpdate(Board *board, float dt);
void GameDraw(const Board *board);

#endif
