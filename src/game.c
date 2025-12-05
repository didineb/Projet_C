#include "game.h"
#include "stdlib.h"
#include "math.h"

extern Texture2D gTileTextures[];
extern int gTileTextureCount;
Player gPlayer; // appel du joueur global
Enemy gEnemy; // appel de l'ennemi global

// ******************************************
// ******************************************

int visionRadius = 5; // rayon de vision du joueur

static bool TileContains(const Tile *t, int texIndex) //fonction bool pour vérifier si une texture précise est présente dans une tuile
{
    for (int i = 0; i < t->layerCount; i++) //parcours toute les couches de la tuile
    {
        if (t->layers[i] == texIndex)  //si l'index de la couche correspond à l'index recherché la fonction renvoie true 
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
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,0,1,0,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1},
    {1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,0},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};


void GameInit(Board *board)
{
    for (int y = 0; y < BOARD_ROWS; y++)
    {
        for (int x = 0; x < BOARD_COLS; x++)
        {
            Tile *t = &board->tiles[y][x];
            TileClear(t);

            if (maze[y][x] == 1) // si la case est un mur
            {
                TilePush(t, 1);
            }
            else if (maze[y][x] == 0) //si la case est un sol
            {
                TilePush(t, 0);
            }
        }
    }

    gPlayer.x = 1; // départ en (1,1)
    gPlayer.y = 1;
    gPlayer.pv = 3;
    gPlayer.textureIndex = 2; // correspond à la texture knight

    gEnemy.x = 28;
    gEnemy.y = 28;
    gEnemy.textureIndex = 3;

    TilePush(&board->tiles[gEnemy.y][gEnemy.x], 3);

}

void GameUpdate(Board *board, float dt)
{

    // Durée minimale entre deux mouvements (en secondes)
    float moveDelay = 0.15f;
    static float lastMoveTime = 0.0f;

    float now = GetTime();   // temps en secondes depuis le lancement
    
    int nextX = gPlayer.x;  //va récupérer la position actuelle du joueur en x
    int nextY = gPlayer.y;


   // On agit seulement si le délai est écoulé
    if (now - lastMoveTime >= moveDelay)
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            nextX++;
            lastMoveTime = now;
        }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            nextX--;
            lastMoveTime = now;
        }
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            nextY--;
            lastMoveTime = now;
        }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            nextY++;
            lastMoveTime = now;
        }
    }


    // limites du board pour ne pas sortir de l'écran
    if (nextX < 0 || nextX >= BOARD_COLS || nextY < 0 || nextY >= BOARD_ROWS) 
        return;

    // récupère la tile cible
    Tile *target = &board->tiles[nextY][nextX];

    // appel de la fonction de vérification des tuiles et si la tuile cible est égale à 1 c'est alors un mur
    if (TileContains(target, 1))
    {
        // collision → on ne bouge pas
        return;
    }

    if (TileContains(target, 3))
    {
        gPlayer.pv -= 1; // le joueur perd 1 point de vie
        return;
    }

    // pas de collision → on valide le déplacement
    gPlayer.x = nextX;
    gPlayer.y = nextY;
}

void GameDraw(const Board *board)
{
    for (int y = 0; y < BOARD_ROWS; y++) //parcours toutes les lignes
    {
        for (int x = 0; x < BOARD_COLS; x++) //parcours toutes les colonnes
        {
            const Tile *t = &board->tiles[y][x]; //récupère la tuile actuelle

            // fond “vide” au cas où
            DrawRectangle(
                x * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE,
                LIGHTGRAY);

            // dessine chaque couche dans l'ordre
            for (int i = 0; i < t->layerCount; i++) //parcours toutes les couches de la tuile
            {
                int idx = t->layers[i]; //récupère l'index de la couche actuelle
                if (idx >= 0 && idx < gTileTextureCount) //vérifie que l'index est valide
                {
                    DrawTexture(   
                        gTileTextures[idx],
                        x * TILE_SIZE,
                        y * TILE_SIZE,
                        WHITE);
                }
            }
        }
    }

    // dessine le joueur au-dessus de tout
    DrawTexture(
        gTileTextures[gPlayer.textureIndex], //récupère la texture du joueur
        gPlayer.x * TILE_SIZE, // position x en cases car multiplié par la taille d'une tuile
        gPlayer.y * TILE_SIZE,
        WHITE); //tint = filtre de couleur | WHITE signifie : dessiner l’image normalement.

}