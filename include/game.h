#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#define TILE_SIZE 32
#define BOARD_COLS 34
#define BOARD_ROWS 22
#define MAX_LAYERS 6
#define visionRadius 1  // rayon de vision du joueur

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
    int x, y; // position sur le board
    int pv;           
    int textureIndex;   // index de la texture dans gTileTextures
} Player;

typedef struct{
    int x, y;
    int textureIndex;
} Enemy;


extern Player gPlayer;  // joueur global
extern Enemy gEnemy;    // ennemi global

void GameInit(Board *board); // initialise le board
void GameUpdate(Board *board, float dt); // mise à jour du jeu
void GameDraw(const Board *board); // affichage du jeu

#endif
