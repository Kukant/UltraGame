/**
 *  Header file for theGame.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "theGame.h"

void eventsDetection(SDL_Event *event, Action *p_action, bool *running, gameState *game, SDL_Window *window)
{
    switch(event->type)
            {
                case SDL_QUIT:
                    *running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event->key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            p_action->up = true;
                            break;
                        /*case SDL_SCANCODE_S:
                            p_action->down = true;
                            break;*/

                        case SDL_SCANCODE_D:
                            p_action->right = true;
                            break;

                        case SDL_SCANCODE_A:
                            p_action->left = true;
                            break;

                        case SDL_SCANCODE_V:
                            p_action->p1Shooting = true;
                            break;

                        // second player
                        case SDL_SCANCODE_UP:
                            p_action->up2 = true;
                            break;

                        /*case SDL_SCANCODE_DOWN:
                            p_action->down2 = true;
                            break;*/

                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = true;
                            break;

                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = true;
                            break;

                        case SDL_SCANCODE_P:
                            p_action->p2Shooting = true;
                            break;

                        case SDL_SCANCODE_KP_ENTER:
                            p_action->p2Shooting = true;
                            break;

                        // the rest
                        case SDL_SCANCODE_RETURN:
                            initNewGame(game);
                            break;

                        case SDL_SCANCODE_ESCAPE:
                            *running = false;
                            break;

                        case SDL_SCANCODE_H:
                            game->help = true;
                            break;

                    }
                    break;
                case SDL_KEYUP:
                    switch (event->key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            p_action->up = false;
                            break;

                        /*case SDL_SCANCODE_S:
                            p_action->down = false;
                            break;*/

                        case SDL_SCANCODE_D:
                            p_action->right = false;
                            break;

                        case SDL_SCANCODE_A:
                            p_action->left = false;
                            break;

                        case SDL_SCANCODE_V:
                            p_action->p1Shooting = false;
                            break;

                        //second player

                        case SDL_SCANCODE_UP:
                            p_action->up2 = false;
                            break;

                        /*case SDL_SCANCODE_DOWN:
                            p_action->down2 = false;
                            break;*/

                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = false;
                            break;

                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = false;
                            break;

                        case SDL_SCANCODE_P:
                            p_action->p2Shooting = false;
                            break;

                        case SDL_SCANCODE_KP_ENTER:
                            p_action->p2Shooting = false;
                            break;
                        // rest
                        case SDL_SCANCODE_H:
                            game->help = false;
                            break;

                    }
                    break;
            }


}
/**
 *
 *  This function does a lot of graphic stuff.
 *
 */
void renderStuff(SDL_Renderer *renderer, gameState *game)
{
    // setting the wall.
    SDL_Rect backRect = { 0, 0, WIDTH, HEIGHT};
    SDL_RenderCopy(renderer, game->backTexture, NULL, &backRect);

    // ledges and bullets
    renderLedges(game, renderer);

    // Specials
    renderSpecials(game, renderer);

    //hp rectangles and ammo
    renderHP(game, renderer);

    // players
    renderPlayers(game, renderer);

    // game over
    if(game->gameIsOver)
    {
        renderGO(game, renderer);
    }
    if (game->help)
    {
            SDL_Rect rect = { 0, 0, WIDTH, HEIGHT};
            SDL_RenderCopy(renderer, game->helpTexture, NULL, &rect);
    }
}
/**
 *
 *  This function does a lot of logic stuff.
 *
 */

void logicStuff(gameState *game)
{
    // players movement
    playersMoving(game);

    // collision detection
    collDetect(game, game->p_p1);
    collDetect(game, game->p_p2);

    // specials coll. detect
    specialsCollDetect(game->specials, game->p_p1);
    specialsCollDetect(game->specials, game->p_p2);

    // set specials display
    setDisplaySpec(game);

    // gravity
    gravityMoving(game);

    // moving bullets
    movingBullets(game);

    if((game->p_p1->hp <= 0 || game->p_p2->hp <= 0) && !game->gameIsOver)
    {
        game->gameIsOver = true;
        game->endTime = time(NULL);
    }

}

void playersMoving(gameState *game)
{
    // player1 movement

    int manVelX = 0;

    if (game->action->up && !game->action->down && game->p_p1->onLedge)
    {
        game->p_p1->dy = -8;
        game->p_p1->onLedge = false;
    }

    if (game->action->left && !game->action->right)
    {
        manVelX = -SPEED;
        game->p_p1->facingLeft = true;
        game->p_p1->walking = true;
    }
    else
        game->p_p1->walking = false;

    if (game->action->right && !game->action->left)
    {
        manVelX = SPEED;
        game->p_p1->facingLeft = false;
        game->p_p1->walking = true;
    }
    else if (!game->p_p2->facingLeft)
        game->p_p1->walking = false;

    game->p_p1->x += (int)manVelX;

    // player2 movement
    manVelX = 0;

    if (game->action->up2 && !game->action->down2  &&  game->p_p2->onLedge)
    {
        game->p_p2->dy = -8;
        game->p_p2->onLedge = false;
    }
    if (game->action->left2 && !game->action->right2 )
    {
        manVelX = -SPEED;
        game->p_p2->facingLeft = true;
        game->p_p2->walking = true;
    }
    else
        game->p_p2->walking = false;

    if (game->action->right2 && !game->action->left2)
    {
        manVelX = SPEED;
        game->p_p2->facingLeft = false;
        game->p_p2->walking = true;
    }
    else if (!game->p_p2->facingLeft)
        game->p_p2->walking = false;

    game->p_p2->x += (int)manVelX;
}


void gravityMoving(gameState* game)
{
    //gravity 2
    if (game->p_p2->onLedge == true)
        game->p_p2->dy = 0.00;

    game->p_p2->y += game->p_p2->dy;

    if (!game->p_p2->onLedge)
        game->p_p2->dy += GRAVITY;

    //gravity1

    if (game->p_p1->onLedge == true)
        game->p_p1->dy = 0.00;

    game->p_p1->y += game->p_p1->dy;

    if (!game->p_p1->onLedge)
        game->p_p1->dy += GRAVITY;
}

int isInWindow(int x, int y) // bullet
{
    return (x > 0 && x < WIDTH && y > 0 && y < HEIGHT)? 1 : 0;
}

void initNewGame(gameState *game)
{
    time_t startTime = time(NULL);
    game->gameIsOver = false;
    game->action->p2Shooting = false;
    game->action->p1Shooting = false;
    game->startTime = startTime;
    game->lastHit = game->frames;


    for(int i = 0; i < MAXBULLETS; i++)
        game->bullets[i].display = false;

    for(int i = 0; i < MAXSPECIALS; i++)
        game->specials[i].display = false;

    game->p_p1->ammo = 100;
    game->p_p1->hp = 50;
    game->p_p1->x = 52;
    game->p_p1->y = HEIGHT - 100;
    game->p_p1->dy = 0;
    game->p_p1->facingLeft = false;
    game->p_p1->alive = 2;
    game->p_p1->currentSprite = 4;

    game->p_p2->ammo = 100;
    game->p_p2->hp = 50;
    game->p_p2->x = WIDTH - 102;
    game->p_p2->y = HEIGHT - 100;
    game->p_p2->dy = 0;
    game->p_p2->facingLeft = true;
    game->p_p2->alive = 2;
    game->p_p2->currentSprite = 4;

}

void movingBullets(gameState *game)
{
    for(int i = 0; i < MAXBULLETS; i++) if(game->bullets[i].display)
    {
        game->bullets[i].x = game->bullets[i].x + (game->bullets[i].goingRight? BULLETSPEED/60 : -BULLETSPEED/60);

        // p1 bullet from the left side
        if( game->p_p1->x + 14 > game->bullets[i].x && game->p_p1->x - 3 < game->bullets[i].x && game->bullets[i].y > game->p_p1->y && game->bullets[i].y < game->p_p1->y + 50 && game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            {
                game->p_p1->hp--;
                game->lastHit = game->frames;
            }
        }
        // p1 bullet from the right side
        else if( game->p_p1->x - 3 < game->bullets[i].x && game->p_p1->x + 14 > game->bullets[i].x && game->bullets[i].y > game->p_p1->y && game->bullets[i].y < game->p_p1->y + 50 && !game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            {
                game->p_p1->hp--;
                game->lastHit = game->frames;
            }
        }
        // p2 bullet from the left side
        else if( game->p_p2->x + 14 > game->bullets[i].x && game->p_p2->x - 3< game->bullets[i].x && game->bullets[i].y > game->p_p2->y && game->bullets[i].y < game->p_p2->y + 50 && game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            {
                game->p_p2->hp--;
                game->lastHit = game->frames;
            }
        }
        // p2 bullet from the right side
        else if( game->p_p2->x - 3 < game->bullets[i].x && game->p_p2->x + 14 > game->bullets[i].x && game->bullets[i].y > game->p_p2->y && game->bullets[i].y < game->p_p2->y + 50 && !game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            {
                game->p_p2->hp--;
                game->lastHit = game->frames;
            }
        }

    }

    // deleting bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game->bullets[i].display)
    {
        if (!isInWindow(game->bullets[i].x,game->bullets[i].y) || ledgeBullDetect(game->bullets[i], game))
        {
            game->bullets[i].display = false;
        }

    }

    // generating bullets
    if (game->action->p1Shooting && (game->frames % 5) == 0 && game->p_p1->alive && game->p_p1->ammo > 0)
    {
        game->p_p1->ammo--;
        Mix_PlayChannel(-1, game->ak47, 0); // ak47 sound
        int shootingHeight;
        if (game->p_p1->walking)
            shootingHeight = 14;
        else
           shootingHeight = 19;
        for(int i = 0; i < MAXBULLETS; i++) if(!game->bullets[i].display)
        {
            game->bullets[i].display = true;
            game->bullets[i].goingRight = !game->p_p1->facingLeft;
            game->bullets[i].x = game->p_p1->facingLeft ? game->p_p1->x : (game->p_p1->x + 40);
            game->bullets[i].y = game->p_p1->y + shootingHeight;
            game->bullets[i].w = 8;
            game->bullets[i].h = 8;

            break;
        }

        game->p_p1->shooting = true;
    }
    else
        game->p_p1->shooting = false;

    if (game->action->p2Shooting && (game->frames % 5) == 0 && game->p_p2->alive && game->p_p2->ammo > 0)
    {
        game->p_p2->ammo--;
        Mix_PlayChannel(-1, game->ak47, 0);
        int shootingHeight;
        if (game->p_p1->walking)
            shootingHeight = 14;
        else
           shootingHeight = 19;
        for(int i = 0; i < MAXBULLETS; i++) if(!game->bullets[i].display)
        {
            game->bullets[i].display = true; // ak47 sound
            game->bullets[i].goingRight = !game->p_p2->facingLeft;
            game->bullets[i].x = game->p_p2->facingLeft ? game->p_p2->x : (game->p_p2->x + 40);
            game->bullets[i].y = game->p_p2->y + shootingHeight;
            game->bullets[i].w = 8;
            game->bullets[i].h = 8;

            break;
        }

        game->p_p2->shooting = true;
    }
    else
        game->p_p2->shooting = false;
}

void merge(char *result, char *one, char *two, char *three)
{
    int i = 0;
    for(int j = 0; j < strlen(one); j++, i++)
        result[i] = one[j];
    for(int j = 0; j < strlen(two); j++, i++)
        result[i] = two[j];
    for(int j = 0; j < strlen(three) + 1; j++, i++) // aby se pripsala nula
        result[i] = three[j];
}

void initTexts(gameState game)
{
    TTF_Init();
    TTF_Font *Blox2 = TTF_OpenFont("Blox2.ttf", 100);
    SDL_Color color = {255, 255, 255};

    SDL_Surface *textSurface = TTF_RenderText_Solid(Blox2, "GAME OVER", color);
    game.p_texts->gameOver = SDL_CreateTextureFromSurface(game.renderer, textSurface);
    int lenght = 9;
    game.p_texts->gORect.x = WIDTH / 2 - 297;
    game.p_texts->gORect.y = 110;
    game.p_texts->gORect.w = 100*lenght*0.66;
    game.p_texts->gORect.h = 100;

    TTF_CloseFont(Blox2);
    Blox2 = TTF_OpenFont("Blox2.ttf", 60);

    textSurface = TTF_RenderText_Solid(Blox2, "PLAYER 1 WON", color);
    game.p_texts->p1Won = SDL_CreateTextureFromSurface(game.renderer, textSurface);
    lenght = 12;
    game.p_texts->wRect.x = WIDTH / 2 - 237;
    game.p_texts->wRect.y = HEIGHT / 2 - 200;
    game.p_texts->wRect.w = 60*lenght*0.66;
    game.p_texts->wRect.h = 60;

    textSurface = TTF_RenderText_Solid(Blox2, "PLAYER 2 WON", color);
    game.p_texts->p2Won = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    game.p_texts->tRect.x = WIDTH / 2 - 198;
    game.p_texts->tRect.y = HEIGHT / 2 - 320 + 15;
    game.p_texts->tRect.w = 60*9*0.66;
    game.p_texts->tRect.h = 60;

    TTF_CloseFont(Blox2);
    SDL_FreeSurface(textSurface);
}

void loadImages(gameState *game)
{
    SDL_Surface *sheet;
    if((sheet = IMG_Load("sheet.png")) == NULL)
            printf("sheet.png not found\n");
    game->p_p1->sheetTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("badman_sheet.png")) == NULL)
            printf("badman_sheet.png not found\n");
    game->p_p2->sheetTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("banana.png")) == NULL)
            printf("banana.png not found\n");
    game->bulletTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("background.png")) == NULL)
            printf("background.png not found\n");
    game->backTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("wall.png")) == NULL)
            printf("wall.png not found\n");
    game->ledgeTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("wallYX.png")) == NULL)
            printf("wallYX.png not found\n");
    game->ledgeTextureYX = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("help.png")) == NULL)
            printf("help.png not found\n");
    game->helpTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("heart.png")) == NULL)
            printf("heart.png not found\n");
    game->heartTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    if((sheet = IMG_Load("ammo.png")) == NULL)
            printf("ammo.png not found\n");
    game->ammoTexture = SDL_CreateTextureFromSurface(game->renderer, sheet);
    SDL_FreeSurface(sheet);
}

void collDetect(gameState *game, Man *man)
{
    float mX = man->x;
    float mY = man->y;
    float mDY = man->dy;
    man->onLedge = false;

    Ledge *ledges = game->ledges;

    for (int i = 0; i < MAXLEDGES; i++)
    {
        if (mX + 20 > ledges[i].x && mX + 20 < ledges[i].x + ledges[i].w )
        {
            // top, bumping our head
            if (mY < ledges[i].y + ledges[i].h && mY > ledges[i].y && mDY < 0)
            {
                /*printf("up pl: .x %g .y %g\n", mX, mY);
                printf("up %d .x %g .y %g .w %g .h %g\n\n",i,ledges[i].x,ledges[i].y,ledges[i].w,ledges[i].h );*/
                man->dy = 0.00;
                man->y = ledges[i].y + ledges[i].h;
                mY = ledges[i].y + ledges[i].h;
            }

            // bottom standing on sth?
            if (mY + 50 > ledges[i].y && mY + 50 < ledges[i].y + ledges[i].h && mDY > 0)
            {
                /*printf("xxxxpl: .x %g .y %g .dy %.5g\n", mX, mY,mDY);
                printf("xxxxx %d .x %g .y %g .w %g .h %g\n\n",i,ledges[i].x,ledges[i].y,ledges[i].w,ledges[i].h );
                */man->y = ledges[i].y - 50;
                mY = ledges[i].y - 50;
                man->dy = 0.00;
                mDY = 0;
                man->onLedge = true;
            }
        }

        if (mY + 25 > ledges[i].y && mY + 25 < ledges[i].y + ledges[i].h )
        {
            // come from right side
            if ( mX < ledges[i].x + ledges[i].w && mX > ledges[i].x)
            {
                /*printf("left pl: .x %g .y %g .dy %.5g\n", mX, mY,mDY);
                printf("left %d .x %g .y %g .w %g .h %g\n\n",i,ledges[i].x,ledges[i].y,ledges[i].w,ledges[i].h );
                */man->x = ledges[i].x + ledges[i].w;
                mX = ledges[i].x + ledges[i].w;
                man->walking = false;
            }

            // come from left side
            if ( mX + 40 > ledges[i].x && mX + 40 < ledges[i].x + ledges[i].w)
            {
                /*printf("right pl: .x %g .y %g .dy %.5g.....................n", mX, mY,mDY);
                printf("right %d .x %g .y %g .w %g .h %g\n\n",i,ledges[i].x,ledges[i].y,ledges[i].w,ledges[i].h );
                */man->x = ledges[i].x - 40;
                mX = ledges[i].x - 40;
                man->walking = false;
            }
        }
    }
}


bool ledgeBullDetect(Bullet bullet, gameState *game)
{
    bool result = false;

    if (bullet.goingRight)
    {
        for(int i = 0; i < MAXLEDGES && !result; i++) if (game->ledges[i].drawn)
        {
            if (game->ledges[i].x < bullet.x + bullet.w && game->ledges[i].x + game->ledges[i].w > bullet.x && bullet.y < game->ledges[i].y + game->ledges[i].h && bullet.y + bullet.h > game->ledges[i].y)
                result = true;
        }
    }
    else
    {
        for(int i = 0; i < MAXLEDGES && !result; i++) if (game->ledges[i].drawn)
        {
            if (game->ledges[i].x + game->ledges[i].w > bullet.x && game->ledges[i].x < bullet.x && bullet.y < game->ledges[i].y + game->ledges[i].h && bullet.y + bullet.h > game->ledges[i].y)
            {
                /*printf("bullet: .x %g .y %g .w %g .h %g\n",bullet.x, bullet.y, bullet.w, bullet.h);
                printf("ledge: .x %g .y %g .w %g .h %g\n\n",game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h);
                */result = true;
            }
        }
    }

    return result;

}

void setLedges(Ledge *ledges)
{
    int i = 0, j;
    for(j = 0; j <= WIDTH / 300; j++) // bottom
    {
        ledges[j].x = j * 300;
        ledges[j].y = HEIGHT - 50;
        ledges[j].w = 300;
        ledges[j].h = 50;
        ledges[j].vertical = false;
        ledges[j].drawn = true;
    }
    i += j + 1;

    for(j = 0; j <= WIDTH / 300; j++) // top
    {
        ledges[j + i].x = j * 300;
        ledges[j + i].y = 53;
        ledges[j + i].w = 300;
        ledges[j + i].h = 50;
        ledges[j + i].vertical = false;
        ledges[j + i].drawn = true;
    }
    i += j + 1;

    for(j = 0; j <= HEIGHT / 300; j++) // left side
    {
        ledges[j + i].x = 0;
        ledges[j + i].y = j * 300 + 53;
        ledges[j + i].w = 50;
        ledges[j + i].h = 300;
        ledges[j + i].vertical = true;
        ledges[j + i].drawn = true;
    }
    i += j + 1;

    for(j = 0; j <= HEIGHT / 300; j++) // right side
    {
        ledges[j + i].x = WIDTH - 50;
        ledges[j + i].y = j * 300 + 53;
        ledges[j + i].w = 50;
        ledges[j + i].h = 300;
        ledges[j + i].vertical = true;
        ledges[j + i].drawn = true;
    }
    i += j + 1;

    // bottom -------------------------------------
    ledges[i].x = WIDTH/2 - 25;
    ledges[i].y = HEIGHT - 50 - 300;
    ledges[i].w = 50;
    ledges[i].h = 300;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 - 25 - 300;
    ledges[i].y = HEIGHT - 50 - 150;
    ledges[i].w = 300;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 + 25;
    ledges[i].y = HEIGHT - 50 - 150;
    ledges[i].w = 300;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 + 170; // right middle
    ledges[i].y = HEIGHT - 50 - 320;
    ledges[i].w = 300;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 + 170; // right middle vertical
    ledges[i].y = HEIGHT - 50 - 370;
    ledges[i].w = 50;
    ledges[i].h = 100;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 - 470; // left middle
    ledges[i].y = HEIGHT - 50 - 320;
    ledges[i].w = 300;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 - 220; // left middle vertical
    ledges[i].y = HEIGHT - 50 - 370;
    ledges[i].w = 50;
    ledges[i].h = 100;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 50; // left short
    ledges[i].y = HEIGHT - 50 - 350;
    ledges[i].w = 150;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH - 200; // right short
    ledges[i].y = HEIGHT - 50 - 350;
    ledges[i].w = 150;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 50; // left shorter
    ledges[i].y = HEIGHT - 50 - 600;
    ledges[i].w = 50;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH - 100; // right shorter
    ledges[i].y = HEIGHT - 50 - 600;
    ledges[i].w = 50;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 50; // left  player
    ledges[i].y = HEIGHT - 50 - 120;
    ledges[i].w = 200;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH - 250; // right player
    ledges[i].y = HEIGHT - 50 - 120;
    ledges[i].w = 200;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    // top --------------------------

    ledges[i].x = 170; // left start
    ledges[i].y = 203;
    ledges[i].w = 100;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 270; // left side vertical
    ledges[i].y = 203;
    ledges[i].w = 50;
    ledges[i].h = 200;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 270; // left
    ledges[i].y = 403;
    ledges[i].w = 350;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = 620; // left vertical short
    ledges[i].y = 353;
    ledges[i].w = 50;
    ledges[i].h = 100;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH - 100 - 170; // right start
    ledges[i].y = 203;
    ledges[i].w = 100;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH -100 - 170 - 50; // right side vertical
    ledges[i].y = 203;
    ledges[i].w = 50;
    ledges[i].h = 200;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH -100 - 170 - 350; // right
    ledges[i].y = 403;
    ledges[i].w = 350;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH -100 - 170 - 350 - 50; // right vertical short
    ledges[i].y = 353;
    ledges[i].w = 50;
    ledges[i].h = 100;
    ledges[i].vertical = true;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 - 200; // middle one
    ledges[i].y = 200;
    ledges[i].w = 200;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    ledges[i].x = WIDTH/2 ; // middle second
    ledges[i].y = 200;
    ledges[i].w = 200;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;

    // center ----------------------------

    ledges[i].x = WIDTH/2  - 100; // middle
    ledges[i].y = 500;
    ledges[i].w = 200;
    ledges[i].h = 50;
    ledges[i].vertical = false;
    ledges[i].drawn = true;
    i++;
}

long long current_timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

    return milliseconds;
}
/// gane over texts
void renderGO(gameState *game, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, game->p_texts->gameOver, NULL, &game->p_texts->gORect);

    // won
    if(game->p_p1->hp > game->p_p2->hp)
        SDL_RenderCopy(renderer, game->p_texts->p1Won, NULL, &game->p_texts->wRect);
    else
        SDL_RenderCopy(renderer, game->p_texts->p2Won, NULL, &game->p_texts->wRect);

    // time

    if(time(NULL) == game->endTime && game->frames == game->lastHit)
    {
        int gameTime = game->endTime - game->startTime;
        char hp[4];
        sprintf(hp, "%d", gameTime);
        char result[100];
        merge(result, "TIME ", hp, " S");

        TTF_Init();
        TTF_Font *Blox2 = TTF_OpenFont("Blox2.ttf", 100);
        SDL_Color color = {255, 255, 255};

        game->p_texts->tRect.w = 0.66*60*strlen(result);

        SDL_Surface *textSurface = TTF_RenderText_Solid(Blox2, result, color);
        game->p_texts->time = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
        // texture freed in main
        TTF_CloseFont(Blox2);
        TTF_Quit();
    }
    SDL_RenderCopy(renderer, game->p_texts->time, NULL, &game->p_texts->tRect);
}

void renderPlayers(gameState *game, SDL_Renderer *renderer)
{
    // player1
    if (game->p_p1->hp > 0)
    {
        if(game->p_p1->walking == true && game->frames % 6 == 0)
        {
            game->p_p1->currentSprite = (game->p_p1->currentSprite + 1) % 4;
        }
        else if (game->p_p1->shooting == true && game->p_p1->walking == false)
            game->p_p1->currentSprite = 5;
        else if (game->p_p1->walking == false)
            game->p_p1->currentSprite = 4;
    }

    else if (game->p_p1->alive && game->frames % 6 == 0) // hp == 0
    {
        game->p_p1->currentSprite = 6;
        game->p_p1->alive--;
        if (game->p_p1->alive == 0)
        {
            game->p_p1->currentSprite = 7;
            Mix_PlayChannel(-1, game->deathSound, 0);
        }

    }

    if (game->p_p1->alive)
    {
        SDL_Texture *p1Texture = game->p_p1->hp > 0 ? game->p_p1->sheetTexture : game->p_p2->sheetTexture; // if p1 is dead, than change texture
        SDL_Rect srcRect = { 40*(game->p_p1->currentSprite), 0, 40, 50};
        SDL_Rect rect = { game->p_p1->x, game->p_p1->y, 40, 50};
        SDL_RenderCopyEx(renderer, p1Texture, &srcRect, &rect,0 , NULL, game->p_p1->facingLeft);
    }

    // player2
    if (game->p_p2->hp > 0)
    {
        if(game->p_p2->walking == true && game->frames % 6 == 0)
        {
            game->p_p2->currentSprite = (game->p_p2->currentSprite + 1) % 4;
        }
        else if (game->p_p2->shooting == true && game->p_p2->walking == false)
            game->p_p2->currentSprite = 5;
        else if (game->p_p2->walking == false)
            game->p_p2->currentSprite = 4;
    }
    else if (game->p_p2->alive && game->frames % 6 == 0) // hp == 0
    {
        game->p_p2->currentSprite = 6;
        game->p_p2->alive--;
        if (game->p_p2->alive == 0)
        {
            Mix_PlayChannel(-1, game->deathSound, 0);
            game->p_p2->currentSprite = 7;
        }
    }

    if (game->p_p2->alive)
    {
        SDL_Rect srcRect2 = { 40*(game->p_p2->currentSprite), 0, 40, 50};
        SDL_Rect rect2 = { game->p_p2->x, game->p_p2->y, 40, 50};
        SDL_RenderCopyEx(renderer, game->p_p2->sheetTexture, &srcRect2, &rect2, 0, NULL, game->p_p2->facingLeft);
    }
}

void renderHP(gameState *game, SDL_Renderer *renderer)
{
    //hp
    if (game->p_p1->hp > 33)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //green
    else if (game->p_p1->hp > 16)
        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255); //orange
    else if (game->p_p1->hp >= 0)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //red

    SDL_Rect fillHp1 = {0, 0, (WIDTH /2 - 10) / 50 * game->p_p1->hp, 53};
    SDL_RenderFillRect(renderer, &fillHp1);

    if (game->p_p2->hp > 33)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //green
    else if (game->p_p2->hp > 16)
        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255); //orange
    else if (game->p_p2->hp >= 0)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //red

    SDL_Rect fillHp2 = {WIDTH  - (WIDTH /2 - 10) / 50 * game->p_p2->hp, 0, (WIDTH /2 - 10) / 50 * game->p_p2->hp, 53};
    SDL_RenderFillRect(renderer, &fillHp2);


    // ammo
    SDL_SetRenderDrawColor(renderer, 204, 204, 0, 255); // yellow

    SDL_Rect fillAmmo1 = { 0, HEIGHT - 12, (WIDTH /2 - 10) / 100 * game->p_p1->ammo, 12};
    SDL_Rect fillAmmo2 = { WIDTH - (WIDTH /2 - 10) / 100 * game->p_p2->ammo, HEIGHT - 12, (WIDTH /2 - 10) / 100 * game->p_p2->ammo, 12};
    SDL_RenderFillRect(renderer, &fillAmmo1);
    SDL_RenderFillRect(renderer, &fillAmmo2);
}

void renderLedges(gameState *game, SDL_Renderer *renderer)
{
    for(int i = 0; i < MAXLEDGES; i++) if (game->ledges[i].vertical == true) // firstly vertical ones
    {
        SDL_Rect lRect = {game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h};
        SDL_RenderCopy(renderer, game->ledgeTextureYX, NULL, &lRect);
    }

    for(int i = 0; i < MAXLEDGES; i++) if (game->ledges[i].vertical == false) //then horizontal ones
    {
        SDL_Rect lRect = {game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h};
        SDL_RenderCopy(renderer, game->ledgeTexture, NULL, &lRect);
    }

    // bullets
    for(int i = 0; i < MAXBULLETS; i++) if (game->bullets[i].display)
    {
        SDL_Rect bRect = { game->bullets[i].x, game->bullets[i].y, game->bullets[i].w, game->bullets[i].h };
        SDL_RenderCopy(renderer, game->bulletTexture, NULL, &bRect);
    }
}

void initSpecials(Special *specials)
{
    int i = 0;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = WIDTH/2 - 15;
    specials[i].y = HEIGHT - 383;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = WIDTH/4;
    specials[i].y = HEIGHT - 83;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = (WIDTH/4) * 3;
    specials[i].y = HEIGHT - 83;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = WIDTH - 270 - 180;
    specials[i].y = 370;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = 450;
    specials[i].y = 370;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = 140;
    specials[i].y = HEIGHT - 433;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 1;
    specials[i].x = WIDTH - 170;
    specials[i].y = HEIGHT - 433;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    // ammo
    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = WIDTH/2 - 15;
    specials[i].y = HEIGHT - 614;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = WIDTH/4 - 50;
    specials[i].y = HEIGHT - 83;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = (WIDTH/4) * 3 + 50;
    specials[i].y = HEIGHT - 83;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = WIDTH - 270 - 180 - 50;
    specials[i].y = 370;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = 450 + 50;
    specials[i].y = 370;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = 140 - 50;
    specials[i].y = HEIGHT - 433;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

    specials[i].display = false;
    specials[i].type = 2;
    specials[i].x = WIDTH - 170 + 50;
    specials[i].y = HEIGHT - 433;
    specials[i].w = 30;
    specials[i].h = 30;
    i++;

}
void renderSpecials(gameState *game, SDL_Renderer *renderer)
{
    SDL_Texture *printTexture;

    for(int i = 0; i < MAXSPECIALS; i++) if (game->specials[i].display)
    {
        switch (game->specials[i].type)
        {
            case 1:
                printTexture = game->heartTexture;
                break;
            case 2:
                printTexture = game->ammoTexture;
                break;

        }
        SDL_Rect hRect = { game->specials[i].x, game->specials[i].y, game->specials[i].w, game->specials[i].h };
        SDL_RenderCopy(renderer, printTexture, NULL, &hRect);
    }
}

void specialsCollDetect(Special *specials, Man *man)
{
    float mX = man->x;
    float mY = man->y;

    for (int i = 0; i < MAXSPECIALS; i++) if (specials[i].display)
    {
        if (mX + 40 > specials[i].x && mX < specials[i].x + specials[i].w && mY + 50 > specials[i].y && mY < specials[i].y + specials[i].h)
        {
            if (man->hp < 50 && specials[i].type == 1)
            {
                specials[i].display = false;
                man->hp = man->hp + 20 > 50 ? 50 : man->hp + 20;
            }
            else if (specials[i].type == 2 && man->ammo < 100 )
            {
                specials[i].display = false;
                man->ammo = man->ammo + 30 > 100 ? 100 : man->ammo + 30;
            }
        }
    }
}

void setDisplaySpec(gameState *game)
{
    srand(time(NULL));

    if (game->frames % 200 == 0) // hearts
    {
        game->specials[rand() % 14].display = true;
    }


}



