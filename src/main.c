#include "raylib.h"
#include "game.h"


// Gestionnaire de texture
Texture2D gTileTextures[6];
int gTileTextureCount = 0;


int main(void)
{
    const int screenWidth = BOARD_COLS * TILE_SIZE;
    const int screenHeight = BOARD_ROWS * TILE_SIZE;

    InitWindow(screenWidth, screenHeight, "Knight Maze - PADARMA Edition");
    SetTargetFPS(60);

    // Chargement des textures
    gTileTextures[0] = LoadTexture("assets/sol.png");
    gTileTextures[1] = LoadTexture("assets/wall.png");
    gTileTextures[2] = LoadTexture("assets/knight.png");
    gTileTextures[3] = LoadTexture("assets/skeleton.png");
    gTileTextures[4] = LoadTexture("assets/attack.png");
    gTileTextureCount = 6;
    
    Board board = {0}; 
    GameInit(&board);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime(); 

        GameUpdate(&board, dt); 

        BeginDrawing();
        ClearBackground(RAYWHITE);

        GameDraw(&board);
        DrawFPS(10, 10);

        double time = GetTime();
        DrawText(TextFormat("Time : %.2f", time), 170, 10, 20, GREEN);

        DrawText(TextFormat("PV : %d", gPlayer.pv), 10, 40, 20, RED);

        EndDrawing();
    }

    // Libération mémoire
    for (int i = 0; i < gTileTextureCount; i++)
    {
        UnloadTexture(gTileTextures[i]);
    }

    CloseWindow();
    return 0;
}
