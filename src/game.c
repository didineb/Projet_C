#include "game.h"

extern Texture2D gTileTextures[];
extern int gTileTextureCount;
Player gPlayer; // appel du joueur global

// ******************************************
//

static bool TileContains(const Tile *t, int texIndex)
{
    for (int i = 0; i < t->layerCount; i++)
    {
        if (t->layers[i] == texIndex)
            return true;
    }
    return false;
}

static void TileClear(Tile *t)
{
    t->layerCount = 0;
    for (int i = 0; i < MAX_LAYERS; i++)
    {
        t->layers[i] = -1;
    }
}

static bool TilePush(Tile *t, int texIndex)
{
    if (t->layerCount >= MAX_LAYERS)
        return false;
    t->layers[t->layerCount++] = texIndex;
    return true;
}

static int TilePop(Tile *t)
{
    if (t->layerCount <= 0)
        return -1;
    int tex = t->layers[--t->layerCount];
    t->layers[t->layerCount] = -1;
    return tex;
}

// ******************************************
// Gestion du board et des entrées

int maze[BOARD_ROWS][BOARD_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    };

void GameInit(Board *board)
{
    for (int y = 0; y < BOARD_ROWS; y++)
    {
        for (int x = 0; x < BOARD_COLS; x++)
        {

            
            Tile *t = &board->tiles[y][x];
            TileClear(t);

            if (maze[y][x] == 1)
            {
                TilePush(t, 1);
            }
            else if (maze[y][x] == 0)
            {
                TilePush(t, 0);
            }
        }
    }

    gPlayer.x = 1; // par exemple départ en (1,1)
    gPlayer.y = 1;
    gPlayer.textureIndex = 3; // correspond à la texture knight

}

void GameUpdate(Board *board, float dt)
{
    Vector2 m = GetMousePosition();
    
    // Gestion des entrées souris sur les tuiles
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        int tileX = (int)(m.x) / TILE_SIZE;
        int tileY = (int)(m.y) / TILE_SIZE;
        
        TraceLog(LOG_INFO,
            "MOUSE DOWN at x=%.1f y=%.1f corresponding tile (%d, %d)",
            m.x, m.y, tileX, tileY);
            
            Tile *t = &board->tiles[tileY][tileX];
            if (t->layerCount > 1)
            {
                TilePop(t);
            }
            else
            {
                int objectIndex = 2;
                TilePush(t, objectIndex);
            }
        }

    int nextX = gPlayer.x;
    int nextY = gPlayer.y;

    // déplacement proposé
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        nextX++;
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        nextX--;
    else if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        nextY--;
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        nextY++;

    // limites du board
    if (nextX < 0 || nextX >= BOARD_COLS || nextY < 0 || nextY >= BOARD_ROWS)
        return;

    // récupère la tile cible
    Tile *target = &board->tiles[nextY][nextX];

    // collision mur (mur = index 1)
    if (TileContains(target, 1))
    {
        // collision → on ne bouge pas
        return;
    }

    // pas de collision → on valide le déplacement
    gPlayer.x = nextX;
    gPlayer.y = nextY;
}

void GameDraw(const Board *board)
{
    for (int y = 0; y < BOARD_ROWS; y++)
    {
        for (int x = 0; x < BOARD_COLS; x++)
        {
            const Tile *t = &board->tiles[y][x];

            // dessine le joueur au-dessus de tout
            DrawTexture(
                gTileTextures[gPlayer.textureIndex], //récupère la texture du joueur
                gPlayer.x * TILE_SIZE, // position x en cases car multiplié par la taille d'une tuile
                gPlayer.y * TILE_SIZE,
                WHITE); //tint = filtre de couleur | WHITE signifie : dessiner l’image normalement.


            // fond “vide” au cas où
            DrawRectangle(
                x * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE,
                LIGHTGRAY);

            // dessine chaque couche dans l'ordre
            for (int i = 0; i < t->layerCount; i++)
            {
                int idx = t->layers[i];
                if (idx >= 0 && idx < gTileTextureCount)
                {
                    DrawTexture(
                        gTileTextures[idx],
                        x * TILE_SIZE,
                        y * TILE_SIZE,
                        WHITE);
                }
            }

            // contour de la tuile (debug)
//            DrawRectangleLines(
//                x * TILE_SIZE,
//                y * TILE_SIZE,
//                TILE_SIZE,
//                TILE_SIZE,
//                DARKGRAY);
        }
    }
}
