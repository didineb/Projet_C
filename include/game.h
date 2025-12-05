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
    int x, y; // position sur le board
    int pv;           
    int textureIndex;   // index de la texture dans gTileTextures
} Player;

typedef struct{
    int x, y;
    int textureIndex;
} Enemy;

typedef struct{
    int x, y;
    int textureIndex;
    int victoire;
} Trophe;

extern Player gPlayer;  // joueur global
extern Enemy gEnemy;    // ennemi global
extern Trophe gTrophe;

void GameInit(Board *board); // initialise le board
void GameUpdate(Board *board, float dt); // mise à jour du jeu
void GameDraw(const Board *board); // affichage du jeu

#endif
