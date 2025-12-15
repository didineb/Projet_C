#include "game.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>


extern Texture2D gTileTextures[];
extern int gTileTextureCount;
Player gPlayer; // appel du joueur global
Enemy gEnemy; // appel de l'ennemi global
Trophe gTrophe;
Piège gPiège; //appel piège
Sound gDeathSound; // son de mort
Sound gHitSound; // son de dommage
Sound gEnemyMusic; // son sur l'ennemi
Sound gVictoryMusic; // musique de victoire
Sound gFlash; // son power up vitesse
Sound gVision; // son power up vision
Sound gHeart; // son power up coeur
PowerUp gPowerUp; // powerup global
int visionRadius = 1; // rayon de vision du joueur
float moveDelay = 0.15f;
float scoreBoard[MAX_RECORDS] = {9999, 9999, 9999, 9999, 9999}; // tableau des 5 meilleurs scores
static float startTime = 0; // temps de début du jeu
float gTimer = 0; // temps écoulé depuis le début du jeu
int nbVictoires = 0; // nombre de victoires

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

float DistancePlayerEnemy() { // fonction qui calcule la distance entre le player et l'ennemi
    float dx = (float)gPlayer.x - (float)gEnemy.x;
    float dy = (float)gPlayer.y - (float)gEnemy.y;
    return sqrtf(dx*dx + dy*dy);
}


// ******************************************
// Gestion du board et des entrées

int maze[BOARD_ROWS][BOARD_COLS] = {
    {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,1,1,0,1,1,0,1,1,1,0,1,0,0,0,0},
    {1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1},
    {0,0,1,1,1,1,0,1,1,1,1,0,1,0,1,1,0,0,0,1,0,0,1,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,1,1,0,1,0,0,1,0,1,0,1,1,1,0,0},
    {1,1,1,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1,0,0,1,0,1,1,0,0,0,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,0,1,1,0,1,1,1,0,1,0,0,1,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,1,0,1,1,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
    {1,1,1,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,1,0,1,0,1,0,0},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1},
    {0,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,0,1,0,1},
    {1,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,1},
    {1,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1},
    {1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,0,1},
};

void UpdateRecords(float currentTime)
{
    for (int i = 0; i < MAX_RECORDS; i++)
    {
        if (currentTime < scoreBoard[i])    // si le temps actuel est meilleur qu'un des records
        {
            // va décaler les scores plus bas
            for (int j = MAX_RECORDS - 1; j > i; j--)
            {
                scoreBoard[j] = scoreBoard[j - 1];
            }
            scoreBoard[i] = currentTime; // insère le nouveau record
            break;
        }
    }
}

void GameInit(Board *board)
{
    
    srand(time(NULL));
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
    gPlayer.pv = 1;
    gPlayer.textureIndex = 2; // correspond à la texture knight

    gEnemy.textureIndex = 3;
    gEnemy.lastMoveTime = 0;
    gEnemy.moveDelay = 0.3;

    gPiège.x = 3;
    gPiège.y= 3;
    gPiège.textureIndex= 7; 


    // spawn aléatoire de l'ennemi
    int xrand, yrand;

    do {
        xrand = rand() % BOARD_COLS; // colonne aléatoire
        yrand = rand() % BOARD_ROWS; // ligne aléatoire
    } while (
        TileContains(&board->tiles[yrand][xrand], 1) || // mur
        TileContains(&board->tiles[yrand][xrand], 2) || // joueur
        TileContains(&board->tiles[yrand][xrand], 6) || // power-up
        TileContains(&board->tiles[yrand][xrand], 7) || // power-up
        TileContains(&board->tiles[yrand][xrand], 8) || // power-up
        TileContains(&board->tiles[yrand][xrand], 5) || // trophée
        xrand == 1 && yrand == 1                        // position de départ du joueur
    );
    
    gEnemy.x = xrand;
    gEnemy.y = yrand;
    TilePush(&board->tiles[gEnemy.y][gEnemy.x], 3); // place l'ennemi sur le board

    //spawn aléatoire de power-ups  
    for (int i = 0; i < 6; i++)
    {
        do {
            xrand = rand() % BOARD_COLS; // colonne aléatoire
            yrand = rand() % BOARD_ROWS; // ligne aléatoire
        } while (
            TileContains(&board->tiles[yrand][xrand], 1) || // mur
            TileContains(&board->tiles[yrand][xrand], 2) || // joueur
            TileContains(&board->tiles[yrand][xrand], 3) || // ennemi
            TileContains(&board->tiles[yrand][xrand], 5) || // trophée
            xrand == 1 && yrand == 1                        // position de départ du joueur
        );
        int texture_power = rand() % 3 + 6;  //random 6-8
        gPowerUp.textureIndex = texture_power; // power-up
        
        TilePush(&board->tiles[yrand][xrand], texture_power); // ajoute le power-up
        
    }


    for (int i = 0; i < 6; i++)
    {
        do {
            xrand = rand() % BOARD_COLS; // colonne aléatoire
            yrand = rand() % BOARD_ROWS; // ligne aléatoire
        } while (
            TileContains(&board->tiles[yrand][xrand], 1) || // mur
            TileContains(&board->tiles[yrand][xrand], 2) || // joueur
            TileContains(&board->tiles[yrand][xrand], 3) || // ennemi
            TileContains(&board->tiles[yrand][xrand], 5) || // trophée
            
            TileContains(&board->tiles[yrand][xrand], 6) || 
            TileContains(&board->tiles[yrand][xrand], 7) ||  
            TileContains(&board->tiles[yrand][xrand], 8) //pour pas mettre le piège là ou ya un power up 
        );
        gPiège.textureIndex = rand() % 2 + 11;  //random 9 à 10
        
        TilePush(&board->tiles[yrand][xrand], gPiège.textureIndex); // ajoute le piège
        
    }


    // Spawn trophée dans les bordures uniquement sur les cases = 0
    int tx, ty;

    do {
        // Choisir aléatoirement une bordure
        int side = rand() % 4;

        switch (side) {
            case 0:  // bord haut
                ty = 0;
                tx = rand() % BOARD_COLS;   // colonne aléatoire de 0 à BOARD_COLS-1
                break;

            case 1:  // bord bas
                ty = BOARD_ROWS - 1;    // dernière ligne
                tx = rand() % BOARD_COLS;   // colonne aléatoire de 0 à BOARD_COLS-1
                break;

            case 2:  // bord gauche
                tx = 0;
                ty = rand() % BOARD_ROWS;   // ligne aléatoire de 0 à BOARD_ROWS-1
                break;

            case 3:  // bord droite
                tx = BOARD_COLS - 1;    // dernière colonne
                ty = rand() % BOARD_ROWS;   // ligne aléatoire de 0 à BOARD_ROWS-1
                break;
        }
    } while (maze[ty][tx] != 0);  // continue jusqu’à trouver une case sol

    // Place le trophée
    gTrophe.x = tx;
    gTrophe.y = ty;
    gTrophe.victoire = 0;
    gTrophe.textureIndex = 5;
    TilePush(&board->tiles[ty][tx], 5);

    startTime = GetTime();

    gTrophe.victoire = 0;

}


// IA ennemi basique : se rapproche du joueur en ligne droite
void UpdateEnemy(Board *board, Enemy *e, const Player *p)
{
    int dx = p->x - e->x;   //combien de cases le joueur est à droite ou à gauche de l'ennemi -> flèche = pointeur d'adresse
    int dy = p->y - e->y;   //combien de cases le joueur est est au-dessus ou en-dessous de l'ennemi

    int oldX = e->x;   // <-- Sauvegarde de l’ancienne position
    int oldY = e->y;

    // prochaine case tentée par l'ennemi
    int tryX = e->x + (dx > 0 ? 1 : (dx < 0 ? -1 : 0)); // ? = if très court == si dx=>0 alors +1 sinon si dx<0 alors -1 sinon 0
    int tryY = e->y + (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    bool moved = false; // indique si l’ennemi a bougé -> utilisé pour mettre à jour l'affichage de l'ennemi sur le board

    // priorité axe X 
    // si la distance horizontale est plus grande que la distance verticale alors on essaye de bouger sur X en premier
    if (abs(dx) >= abs(dy)) 
    {
        if (!TileContains(&board->tiles[e->y][tryX], 1))    // si la tuile n'est pas un mur
        {
            e->x = tryX;    // on bouge l'ennemi
            moved = true;   // on indique qu'il a bougé
        }
        else if (!TileContains(&board->tiles[tryY][e->x], 1))
        {
            e->y = tryY;
            moved = true;
        }
    }
    else    // priorité axe Y
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

    if (moved)  // si l’ennemi a bougé, on met à jour le board en effaçant l’ancienne tuile ennemi et en ajoutant la nouvelle position
    {
        // efface l'ancien emplacement (méthode de réécriture pour enlever un élément)
        Tile *oldTile = &board->tiles[oldY][oldX];
        int new_layer_count = 0;
        for (int i = 0; i < oldTile->layerCount; i++)
        {
            if (oldTile->layers[i] != e->textureIndex) // On ne copie que ce qui n'est PAS l'ennemi
            {
                oldTile->layers[new_layer_count++] = oldTile->layers[i];    // réécrit la couche à la nouvelle position
            }
        }

        // mise à jour du nombre de couches = nouvelles couches après suppression de l'ennemi
        // comme dire "j'ai 3 couches, j'en ai enlevé une, j'en ai plus que 2 maintenant"
        oldTile->layerCount = new_layer_count; 

        // ajoute la nouvelle position
        TilePush(&board->tiles[e->y][e->x], e->textureIndex);
    }

}


void GameUpdate(Board *board, float dt)
{

    gTimer = GetTime() - startTime; // met à jour le timer
    
    if (!IsSoundPlaying(gEnemyMusic)) { // si la musique ne se joue pas
        PlaySound(gEnemyMusic); // Redémarre dès qu’il s’arrête pour faire une boucle
    }

    // ajuster volume selon distance :
    float dist = DistancePlayerEnemy(); //fonction qui renvoie la distance en tuile joueur/ennemi
    float maxDist = 15.0f; // distance max

    float volume = 1.0f - (dist / maxDist);
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;

    SetSoundVolume(gEnemyMusic, volume);


    static float lastMoveTime = 0.0f;

    static bool gameOver = false; //var pour delai mort
    static float gameOverTime = 0.0f;

    static float HitTime = 0.0f;

    if (gameOver)
    {
        if (GetTime() - gameOverTime >= 2.5f)
        {
            GameInit(board);
            gPlayer.pv = 1;
            gameOver = false;
            visionRadius = 1;
            moveDelay = 0.15f;
            nbVictoires = 0;

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

    double now = GetTime();

    // déplacement de l'ennemi
    if (now - gEnemy.lastMoveTime >= gEnemy.moveDelay)
    {
        UpdateEnemy(board, &gEnemy, &gPlayer);
        gEnemy.lastMoveTime = now;
    }

    // déplacement du joueur
    int nextX = gPlayer.x;
    int nextY = gPlayer.y;

    if (now - lastMoveTime >= moveDelay)
    {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { nextX++; lastMoveTime = now; gPlayer.textureIndex = 2; }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { nextX--; lastMoveTime = now; gPlayer.textureIndex = 10; }
        else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { nextY--; lastMoveTime = now; gPlayer.textureIndex = 2; }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { nextY++; lastMoveTime = now; gPlayer.textureIndex = 2; }
    }

    // Moyen d'augmenter/réduire le rayon de vision pour le débuggage
    if (IsKeyPressed(KEY_F))
    {
        visionRadius += 40;
    }
    
    if (IsKeyPressed(KEY_G))
    {
        visionRadius -= 40;
    }    

    // Empêche de sortir du board
    if (nextX < 0 || nextX >= BOARD_COLS || nextY < 0 || nextY >= BOARD_ROWS)
    {
        return; // on refuse le mouvement
    }   

    // collisions joueur
    Tile *target = &board->tiles[nextY][nextX];

    if (TileContains(target, 1))
        return;

    if (TileContains(target, 3)) // si joueur et ennemi sur la même case -> le joueur perd un PV
    {
        if (GetTime() - HitTime >= 2.5f) //delai pour un cooldown sur la perte de PV
        {
            gPlayer.pv--;
            PlaySound(gHitSound);
            HitTime = GetTime();
            gPlayer.textureIndex = 9;
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
        PlaySound(gVictoryMusic);
        UpdateRecords(gTimer); // met à jour le tableau des records
        gPlayer.x = 1;
        gPlayer.y = 1;
        nbVictoires += 1;
        Victory = true;
        VictoryTime = GetTime();
        return;
    }
    if (TileContains(target,6))
    {
        moveDelay *= 0.95f;
        PlaySound(gFlash);
        TilePop(target);
    }
    if (TileContains(target,7))
    {
        visionRadius += 1;
        PlaySound(gVision);
        TilePop(target);
    }
    if (TileContains(target,8))
    {
        gPlayer.pv+=1;
        PlaySound(gHeart);
        TilePop(target);
    }
    if (TileContains(target,11))
    {
        gPlayer.pv-= 1;
        TilePop(target);
        gameOver = true;
    }
    if (TileContains(target,12))
    {
        gPlayer.x=1;
        gPlayer.y=1;
        TilePop(target);
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
    if (gPlayer.pv <= 0)
    {
        DrawText("GAME OVER", 300, 300, 80, RED);
    }

    // Affiche Victoire si nécessaire
    if (gTrophe.victoire==1)
    {
        DrawText("VICTOIRE", 300, 300, 80, YELLOW);
    }

    DrawText(TextFormat("Time : %.2f", gTimer), 170, 10, 20, GREEN);
    
    DrawText(TextFormat("Victoires : %d", nbVictoires), 295, 10, 20, YELLOW);

    for (int i = 0; i < MAX_RECORDS; i++)
    {
        DrawText(TextFormat("%d. %.2f s", i + 1, scoreBoard[i]), 950, 10 + i * 30, 20, WHITE);
    }
    


}