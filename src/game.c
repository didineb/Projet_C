#include "game.h"
#include "stdlib.h"


extern Texture2D gTileTextures[];
extern int gTileTextureCount;
Player gPlayer; // appel du joueur global
Enemy gEnemy; // appel de l'ennemi global
Trophe gTrophe;
Sound gDeathSound; // son de mort
Sound gHitSound; // son de dommage

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

    gEnemy.x = 9;
    gEnemy.y = 1;
    gEnemy.textureIndex = 3;

    gTrophe.x = 32;
    gTrophe.y = 21;
    gTrophe.victoire = 0;
    gTrophe.textureIndex = 5;

    TilePush(&board->tiles[gTrophe.y][gTrophe.x], 5);
    TilePush(&board->tiles[gEnemy.y][gEnemy.x], 3);

    gEnemy.lastMoveTime = 0;
    gEnemy.moveDelay = 0.4; // ennemi bouge toutes les 0.4 secondes (400 ms)

}


// IA ennemi basique : se rapproche du joueur en ligne droite
void UpdateEnemy(Board *board, Enemy *e, const Player *p)
{
    int dx = p->x - e->x;
    int dy = p->y - e->y;

    int oldX = e->x;   // <-- Sauvegarde de l’ancienne position
    int oldY = e->y;

    int tryX = e->x + (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int tryY = e->y + (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    bool moved = false;

    if (abs(dx) >= abs(dy))
    {
        if (!TileContains(&board->tiles[e->y][tryX], 1))
        {
            e->x = tryX;
            moved = true;
        }
        else if (!TileContains(&board->tiles[tryY][e->x], 1))
        {
            e->y = tryY;
            moved = true;
        }
    }
    else
    {
        if (!TileContains(&board->tiles[tryY][e->x], 1))
        {
            e->y = tryY;
            moved = true;
        }
        else if (!TileContains(&board->tiles[e->y][tryX], 1))
        {
            e->x = tryX;
            moved = true;
        }
    }

    if (moved)
    {
        // ✔️ Effacer L’ANCIEN emplacement
        Tile *oldTile = &board->tiles[oldY][oldX];
        for (int i = 0; i < oldTile->layerCount; i++)
        {
            if (oldTile->layers[i] == e->textureIndex)
                oldTile->layers[i] = -1;
        }

        // ✔️ Ajouter la NOUVELLE position
        TilePush(&board->tiles[e->y][e->x], e->textureIndex);
    }
}


void GameUpdate(Board *board, float dt)
{
    float moveDelay = 0.15f;
    static float lastMoveTime = 0.0f;

    static bool gameOver = false;
    static float gameOverTime = 0.0f;

    static float HitTime = 0.0f;

    if (gameOver)
    {
        if (GetTime() - gameOverTime >= 2.5f)
        {
            GameInit(board);
            gPlayer.pv = 3;
            gameOver = false;
        }
        return;
    }

    static bool Victory = false;
    static float VictoryTime = 0.0f;

    if (Victory)
    {
        // Après 2.5s, réinitialiser le jeu
        if (GetTime() - VictoryTime >= 2.5f) 
        {
            GameInit(board);
            Victory = false;
        }
        return; // pendant le Game Over, pas de déplacement
    }

    static bool Victory = false;
    static float VictoryTime = 0.0f;

    if (Victory)
    {
        // Après 2.5s, réinitialiser le jeu
        if (GetTime() - VictoryTime >= 2.5f) 
        {
            GameInit(board);
            Victory = false;
        }
        return; // pendant le Game Over, pas de déplacement
    }







    double now = GetTime();

    // --- ✔ ENNEMI SE DÉPLACE TOUJOURS ---
    if (now - gEnemy.lastMoveTime >= gEnemy.moveDelay)
    {
        UpdateEnemy(board, &gEnemy, &gPlayer);
        gEnemy.lastMoveTime = now;
    }
    // -------------------------------------


    // --- LOGIQUE DU JOUEUR ---
    int nextX = gPlayer.x;
    int nextY = gPlayer.y;

    if (now - lastMoveTime >= moveDelay)
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { nextX++; lastMoveTime = now; }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { nextX--; lastMoveTime = now; }
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { nextY--; lastMoveTime = now; }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { nextY++; lastMoveTime = now; }
    }

    // collisions joueur
    Tile *target = &board->tiles[nextY][nextX];

    if (TileContains(target, 1))
        return;

    if (TileContains(target, 3))
    {
        if (GetTime() - HitTime >= 2.5f)
        {
            gPlayer.pv--;
            PlaySound(gHitSound);
            HitTime = GetTime();
        }
        if (gPlayer.pv <= 0)
        {
            PlaySound(gDeathSound);
            gameOver = true;
            gameOverTime = GetTime();
        }
        return;
    }

    if (TileContains(target, 5))
    {
        gTrophe.victoire += 1;
        gPlayer.x = 1;
        gPlayer.y = 1;
        gPlayer.x = 1;
        Victory = true;
        VictoryTime = GetTime();
        return;
    }

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
        DrawText("GAME OVER", 400, 350, 80, RED);
    }
    static bool Victory = false; // même flag que dans GameUpdate
    if (gTrophe.victoire==1 || Victory)
    {
        DrawText("VICTOIRE", 300, 300, 80, YELLOW);
    }
    static bool Victory = false; // même flag que dans GameUpdate
    if (gTrophe.victoire==1 || Victory)
    {
        DrawText("VICTOIRE", 300, 300, 80, YELLOW);
    }
}
