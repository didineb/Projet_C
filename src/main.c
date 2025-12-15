#include "raylib.h"
#include "game.h"


// Gestionnaire de texture
Texture2D gTileTextures[11];
int gTileTextureCount = 0;


int main(void)
{
    const int screenWidth = BOARD_COLS * TILE_SIZE;
    const int screenHeight = BOARD_ROWS * TILE_SIZE;

    InitWindow(screenWidth, screenHeight, "Knight Maze - PADARMA Edition");

    InitAudioDevice();

    SetTargetFPS(60);

    // Chargement des textures
    gTileTextures[0] = LoadTexture("assets/sol.png");
    gTileTextures[1] = LoadTexture("assets/wall.png");
    gTileTextures[2] = LoadTexture("assets/knight.png");
    gTileTextures[3] = LoadTexture("assets/skeleton.png");
    gTileTextures[5] = LoadTexture("assets/trophy.png");
    gTileTextures[6] = LoadTexture("assets/hermes.png");
    gTileTextures[7] = LoadTexture("assets/lunettes.png");
    gTileTextures[8] = LoadTexture("assets/coeur.png");
    gTileTextures[9] = LoadTexture("assets/knight_hurt1.png");
    gTileTextures[10] = LoadTexture("assets/knight_left.png");

    gTileTextureCount = 11;

    

    
    Board board = {0}; 
    GameInit(&board);

    Music music = LoadMusicStream("assets/Panjabi.ogg");
    gDeathSound = LoadSound("assets/gameover.mp3");
    gHitSound = LoadSound("assets/hurt_sound.mp3");
    gVictoryMusic = LoadSound("assets/Panjabi2m.ogg");
    gFlash = LoadSound("assets/kachow.mp3");
    gHeart = LoadSound("assets/heart.ogg");

    gVision = LoadSound("assets/vision.mp3");
    SetSoundVolume(gVision, 0.2f); // son vision trop fort donc
    
    //---Gestion du son pour l'ennemi---//
    gEnemyMusic = LoadSound("assets/thecroods.mp3");
    SetSoundVolume(gEnemyMusic, 0.3f);
    PlaySound(gEnemyMusic);
    //-------//


    PlayMusicStream(music);
    SetMusicVolume(music, 0.0f);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        float dt = GetFrameTime(); 

        GameUpdate(&board, dt);


        BeginDrawing();
        ClearBackground(RAYWHITE);

        GameDraw(&board);
        DrawFPS(10, 10);

        DrawText(TextFormat("PV : %d", gPlayer.pv), 95, 10, 20, RED);

        EndDrawing();
    }

    // Libération mémoire
    for (int i = 0; i < gTileTextureCount; i++)
    {
        UnloadTexture(gTileTextures[i]);
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
