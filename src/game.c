#include "game.h"
#include "stdlib.h"


extern Texture2D gTileTextures[];
extern int gTileTextureCount;
Player gPlayer; // appel du joueur global
Enemy gEnemy; // appel de l'ennemi global
Trophe gTrophe;
// ******************************************
// ******************************************

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
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,0,1,0,1,1,0,1,1,1,0,1,0,0,0,1},
    {1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,0,0,1,0,1,0,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,0,1,0,1,1,0,1,0,1,0,0,1,1,1,1,0,1,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,1,0,0,1},
    {1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1},
    {1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
};


void GameInit(Board *board)
{
    for (int y = 0; y < BOARD_ROWS; y++)
    {
        for (int x = 0; x < BOARD_COLS; x++)
        {
            Tile *t = &board->tiles[y][x]; //récupère la tuile actuelle
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

    gEnemy.x = 3;
    gEnemy.y = 1;
    gEnemy.x = 3;
    gEnemy.y = 1;
    gEnemy.textureIndex = 3;

    gTrophe.x = 29;
    gTrophe.y = 16;
    gTrophe.victoire = 0;
    gTrophe.textureIndex = 5;

    TilePush(&board->tiles[gTrophe.y][gTrophe.x], 5);
    TilePush(&board->tiles[gEnemy.y][gEnemy.x], 3);

}

void GameUpdate(Board *board, float dt)
{
    // Durée minimale entre deux mouvements (en secondes)
    float moveDelay = 0.15f;
    static float lastMoveTime = 0.0f;

    // Gestion du Game Over
    static bool gameOver = false;
    static float gameOverTime = 0.0f;

    if (gameOver)
    {
        // Après 0.9s, réinitialiser le jeu
        if (GetTime() - gameOverTime >= 2.5f) 
        {
            GameInit(board);
            gPlayer.pv = 3;
            gameOver = false;
        }
        return; // pendant le Game Over, pas de déplacement
    }

    float now = GetTime();   // temps actuel
    int nextX = gPlayer.x;
    int nextY = gPlayer.y;

    // Déplacement avec délai
    if (now - lastMoveTime >= moveDelay)
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { nextX++; lastMoveTime = now; }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { nextX--; lastMoveTime = now; }
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { nextY--; lastMoveTime = now; }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { nextY++; lastMoveTime = now; }
    }

    // Récupère la tuile cible
    Tile *target = &board->tiles[nextY][nextX];

    // Limites du board
    if (nextX < 0 || nextX >= BOARD_COLS || nextY < 0 || nextY >= BOARD_ROWS) return;

    // appel de la fonction de vérification des tuiles et si la tuile cible est égale à 1 c'est alors un mur
    if (TileContains(target, 1))
    {
        // collision → on ne bouge pas
        return;
    }

    if (TileContains(target, 3))
    {
        gPlayer.pv--;
        if (gPlayer.pv == 0)
        {
            gameOver = true;
            gameOverTime = GetTime(); // démarre le délai avant réinitialisation
        }
        return;
    }
    
    if (TileContains(target, 5)){
        gTrophe.victoire += 1;
        gPlayer.y = 1;
        gPlayer.x = 1;
        return;
    }

    // Déplacement validé
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

            DrawRectangle(
                x * TILE_SIZE,
                y * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE,
                LIGHTGRAY);

            for (int i = 0; i < t->layerCount; i++)
            {
                int idx = t->layers[i];
                if (idx >= 0 && idx < gTileTextureCount)
                {
                    DrawTexture(gTileTextures[idx], x * TILE_SIZE, y * TILE_SIZE, WHITE);
                }
            }

            // On calcule la distance simple (en cases)
            int dx = abs(x - gPlayer.x);    // Combien la case est à droite ou à gauche du joueur (axe x)
            int dy = abs(y - gPlayer.y);    // Combien la case est au-dessus ou en-dessous du joueur (axe y)

            // Si dx ou dy dépasse le rayon de vision, on dessine un rectangle noir par-dessus
            if (dx > visionRadius || dy > visionRadius)
            {
                DrawRectangle(
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE,
                    BLACK);   // totalement opaque
            }
        }
    }

    // Dessine le joueur
    DrawTexture(gTileTextures[gPlayer.textureIndex], gPlayer.x * TILE_SIZE, gPlayer.y * TILE_SIZE, WHITE);

    // Affiche Game Over si nécessaire
    static bool gameOver = false; // même flag que dans GameUpdate
    if (gPlayer.pv == 0 || gameOver)
    {
        DrawText("GAME OVER", 250, 410, 80, RED);
    }
}
