/**
 *  Header file for theGame.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

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
                        case SDL_SCANCODE_S:
                            p_action->down = true;
                            break;

                        case SDL_SCANCODE_D:
                            p_action->right = true;
                            break;

                        case SDL_SCANCODE_A:
                            p_action->left = true;
                            break;

                        case SDL_SCANCODE_SPACE:
                            p_action->p1Shooting = true;
                            break;

                        // second player
                        case SDL_SCANCODE_UP:
                            p_action->up2 = true;
                            break;

                        case SDL_SCANCODE_DOWN:
                            p_action->down2 = true;
                            break;

                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = true;
                            break;

                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = true;
                            break;

                        case SDL_SCANCODE_M:
                            p_action->p2Shooting = true;
                            break;
                        case SDL_SCANCODE_RETURN:
                            initNewGame(game);
                            break;
                        case SDL_SCANCODE_ESCAPE:
                            *running = false;
                            break;
                        case SDL_SCANCODE_U:

                            game->p_p2->AI = true;
                            break;
                        case SDL_SCANCODE_I:
                            if (!game->action->p2Shooting)
                                game->p_p2->AI = false;
                            break;

                    }
                    break;
                case SDL_KEYUP:
                    switch (event->key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            p_action->up = false;
                            break;

                        case SDL_SCANCODE_S:
                            p_action->down = false;
                            break;

                        case SDL_SCANCODE_D:
                            p_action->right = false;
                            break;

                        case SDL_SCANCODE_A:
                            p_action->left = false;
                            break;

                        case SDL_SCANCODE_SPACE:
                            p_action->p1Shooting = false;
                            break;

                        //second player

                        case SDL_SCANCODE_UP:
                            p_action->up2 = false;
                            break;

                        case SDL_SCANCODE_DOWN:
                            p_action->down2 = false;
                            break;

                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = false;
                            break;

                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = false;
                            break;

                        case SDL_SCANCODE_M:
                            p_action->p2Shooting = false;
                            break;

                    }
                    break;
            }


}
/**
 *
 *  This function does a lot of grapzic stuff.
 *
 */
void renderStuff(SDL_Renderer *renderer, gameState game)
{
    // setting the wall.
    /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);*/
    SDL_Rect backRect = { 0, 0, WIDTH, HEIGHT};
    SDL_RenderCopy(renderer, game.backTexture, NULL, &backRect);

    // bullets
    for(int i = 0; i < MAXBULLETS; i++) if (game.bullets[i].display)
    {
        SDL_Rect bRect = { game.bullets[i].x, game.bullets[i].y, 8, 8 };
        SDL_RenderCopy(renderer, game.bulletTexture, NULL, &bRect);
    }
    //hp
    char hp[4];
    sprintf(hp, "%d", game.p_p1->hp);
    drawText(renderer, hp, 2, 2, 50);
    sprintf(hp, "%d", game.p_p2->hp);
    drawText(renderer, hp, WIDTH - 2 , 2, 50);


    // player1
    if(game.p_p1->shooting == true)
        game.p_p1->currentSprite = 5;
    else
        game.p_p1->currentSprite = 4;

    SDL_Rect srcRect = { 40*(game.p_p1->currentSprite), 0, 40, 50};
    SDL_Rect rect = { game.p_p1->x, game.p_p1->y, 40, 50};
    SDL_RenderCopyEx(renderer, game.p_p1->sheetTexture, &srcRect, &rect,0 , NULL, game.p_p1->facingLeft);

    // player2
    if(game.p_p2->shooting == true)
        game.p_p2->currentSprite = 5;
    else
        game.p_p2->currentSprite = 4;

    SDL_Rect srcRect2 = { 40*(game.p_p2->currentSprite), 0, 40, 50};
    SDL_Rect rect2 = { game.p_p2->x, game.p_p2->y, 40, 50};
    SDL_RenderCopyEx(renderer, game.p_p2->sheetTexture, &srcRect2, &rect2, 0, NULL, game.p_p2->facingLeft);

    // game over
    if(game.gameIsOver)
    {
        drawText(renderer, "GAME OVER", WIDTH / 2 - 297 , 30, 100);
        if(game.p_p1->hp > game.p_p2->hp)
             drawText(renderer, "PLAYER 1 WON", WIDTH / 2 - 237 , HEIGHT / 2 - 200, 60);
        else
             drawText(renderer, "PLAYER 2 WON", WIDTH / 2 - 237 , HEIGHT / 2 - 200, 60);

        // time
        int gameTime = game.endTime - game.startTime;
        sprintf(hp, "%d", gameTime);
        char result[100];
        merge(result, "TIME  ", hp, " S");
        drawText(renderer, result, WIDTH / 2 - 198 , HEIGHT / 2 - 320, 60);
    }

}
/**
 *
 *  This function does a lot of logic stuff.
 *
 */

void logicStuff(gameState game, gameState *p_game)
{
    // player1
    int manVelX = 0;
    int manVelY = 0;

    if (game.action->up && !game.action->down && !colDetected(1, game.p_p1)) manVelY = -SPEED;
    if (!game.action->up && game.action->down && !colDetected(2, game.p_p1)) manVelY = SPEED;
    if (game.action->left && !game.action->right && !colDetected(4, game.p_p1))
    {
        manVelX = -SPEED;
        game.p_p1->facingLeft = true;
    }
    if (game.action->right && !game.action->left && !colDetected(3, game.p_p1))
    {
        manVelX = SPEED;
        game.p_p1->facingLeft = false;
    }

    game.p_p1->x += (int)manVelX/60;
    game.p_p1->y += (int)manVelY/60;

    // player2
    manVelX = 0;
    manVelY = 0;
    if(game.p_p2->AI == false)
    {
        if (game.action->up2 && !game.action->down2 && !colDetected(1, game.p_p2)) manVelY = -SPEED;
        if (!game.action->up2 && game.action->down2 && !colDetected(2, game.p_p2)) manVelY = SPEED;
        if (game.action->left2 && !game.action->right2 && !colDetected(4, game.p_p2))
        {
            manVelX = -SPEED;
            game.p_p2->facingLeft = true;
        }
        if (game.action->right2 && !game.action->left2 && !colDetected(3, game.p_p2))
        {
            manVelX = SPEED;
            game.p_p2->facingLeft = false;
        }
    }
    // AI p2 moving
    else
    {
        if ((game.frames % 60) == 0)
        {
            time_t seconds = time(NULL);
            if ((int)(seconds % 3) == 0 || (int)(seconds % 7) == 0 || (int)(seconds % 16) == 0) // zmena stavu jestli ma AI chodit
            {
                if (p_game->walkAI)
                   p_game->walkAI = false;
                else
                    p_game->walkAI = true;
            }
        }

        AI(&manVelX, &manVelY, game);
    }

    game.p_p2->x += (int)manVelX/60;
    game.p_p2->y += (int)manVelY/60;

    // AI shooting
    if (game.p_p1->x < game.p_p2->x && game.p_p1->y > game.p_p2->y - 25 && game.p_p1->y < game.p_p2->y + 25 && game.p_p2->AI)
    {
        game.p_p2->shooting = true;
        game.action->p2Shooting = true;

    }
    else if (game.p_p2->AI)
    {
       game.p_p2->shooting = false;
        game.action->p2Shooting = false;
    }

    // moving bullets
    movingBullets(&game);


    if((game.p_p1->hp <= 0 || game.p_p2->hp <= 0) && !game.gameIsOver)
    {
        p_game->gameIsOver = true;
        p_game->endTime = time(NULL);
    }

}

int isInWindow(int x, int y)
{
    return (x > 0 && x < WIDTH && y > 0 && y < HEIGHT)? 1 : 0;
}

void drawText(SDL_Renderer *renderer, char *text, int x, int y, int size)
{
    TTF_Init();

    TTF_Font *Blox2 = TTF_OpenFont("Blox2.ttf", size);
    SDL_Color Red = {255, 0, 0};
    SDL_Surface *textSurface = TTF_RenderText_Solid(Blox2, text, Red);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int lenght = strlen(text);
    int tmpX = 20*lenght + x > WIDTH ? x - size*lenght*0.66 : x;
    SDL_Rect textRect = {.x = tmpX, .y = y, .w = size*lenght*0.66, .h = size};
    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    TTF_CloseFont(Blox2);
    SDL_DestroyTexture(textTexture);
    TTF_Quit();
}

void initNewGame(gameState *game)
{
    time_t startTime = time(NULL);
    game->gameIsOver = false;
    game->action->p2Shooting = false;
    game->action->p1Shooting = false;
    game->startTime = startTime;

    for(int i = 0; i < MAXBULLETS; i++)
        game->bullets[i].display = false;

    game->p_p1->hp = 50;
    game->p_p1->x = 50;
    game->p_p1->y = HEIGHT - 100;
    game->p_p1->facingLeft = false;
    game->p_p1->alive = true;
    game->p_p1->currentSprite = 4;
    game->p_p1->AI = false;

    game->p_p2->hp = 50;
    game->p_p2->x = WIDTH - 100;
    game->p_p2->y = HEIGHT - 100;
    game->p_p2->facingLeft = true;
    game->p_p2->alive = true;
    game->p_p2->currentSprite = 4;
    game->p_p2->AI = false;

}

void AI(int *manVelX, int *manVelY, gameState game)
{
    if(!AI_help(game))//kdyz v obdelniku pred nim nejsou kulky
    {
         if (game.walkAI)
         {
            if (game.p_p1->y > game.p_p2->y)
                *manVelY = SPEED/2;
            else if (game.p_p1->y < game.p_p2->y)
                *manVelY = -SPEED/2;
            else
                *manVelY = 0;
         }
         else
         {
            if (game.p_p1->y > game.p_p2->y)
                *manVelY = SPEED;
            else if (game.p_p1->y < game.p_p2->y)
                *manVelY = -SPEED;
            else
                *manVelY = 0;
         }
    }
    else
    {
        if (game.p_p2->x < HEIGHT/2)
            *manVelY = SPEED;
        else
            *manVelY = -SPEED;
    }
}

/**
 *  Funkce vraci nulu pokud hracem nejsou kulky. Jinak vraci true.
 *
 */
int AI_help(gameState game)
{
    int x = game.p_p2->x;
    int y = game.p_p2->y;

    int returnValue = 0;
    for (int i = 0; i < MAXBULLETS && returnValue == 0; i++)
    {
        if(game.bullets[i].y > y && game.bullets[i].y < y + 50 && game.bullets[i].x < x && game.bullets[i].x > x - 500 && game.bullets[i].goingRight && game.bullets[i].display)
            returnValue++;
    }

    return returnValue;
}

void movingBullets(gameState *game)
{
    for(int i = 0; i < MAXBULLETS; i++) if(game->bullets[i].display)
    {
        game->bullets[i].x = game->bullets[i].x + (game->bullets[i].goingRight? BULLETSPEED/60 : -BULLETSPEED/60);

        // p1 bullet from the left side
        if( game->p_p1->x + 14 > game->bullets[i].x && game->p_p1->x - 3< game->bullets[i].x && game->bullets[i].y > game->p_p1->y && game->bullets[i].y < game->p_p1->y + 50 && game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            game->p_p1->hp--;
        }
        // p1 bullet from the right side
        else if( game->p_p1->x - 3 < game->bullets[i].x && game->p_p1->x + 14 > game->bullets[i].x && game->bullets[i].y > game->p_p1->y && game->bullets[i].y < game->p_p1->y + 50 && !game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
            game->p_p1->hp--;
        }
        // p2 bullet from the left side
        else if( game->p_p2->x + 14 > game->bullets[i].x && game->p_p2->x - 3< game->bullets[i].x && game->bullets[i].y > game->p_p2->y && game->bullets[i].y < game->p_p2->y + 50 && game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
                game->p_p2->hp--;
        }
        // p2 bullet from the right side
        else if( game->p_p2->x - 3 < game->bullets[i].x && game->p_p2->x + 14 > game->bullets[i].x && game->bullets[i].y > game->p_p2->y && game->bullets[i].y < game->p_p2->y + 50 && !game->bullets[i].goingRight)
        {
            game->bullets[i].display = false;
            if(!game->gameIsOver)
                game->p_p2->hp--;
        }

    }

    // deleting bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game->bullets[i].display)
    {
        if (!isInWindow(game->bullets[i].x,game->bullets[i].y))
        {
            game->bullets[i].display = false;
        }

    }

    // generating bullets
    if (game->action->p1Shooting && (game->frames % 5) == 0)
    {
        Mix_PlayChannel(-1, game->ak47, 0); // ak47 sound
        for(int i = 0; i < MAXBULLETS; i++) if(!game->bullets[i].display)
        {
            game->bullets[i].display = true;
            game->bullets[i].goingRight = !game->p_p1->facingLeft;
            game->bullets[i].x = game->p_p1->facingLeft ? game->p_p1->x : (game->p_p1->x + 40);
            game->bullets[i].y = game->p_p1->y + 20;

            break;
        }

        game->p_p1->shooting = true;
    }
    else
        game->p_p1->shooting = false;

    if (game->action->p2Shooting && (game->frames % 5) == 0)
    {
        Mix_PlayChannel(-1, game->ak47, 0);

        for(int i = 0; i < MAXBULLETS; i++) if(!game->bullets[i].display)
        {
            game->bullets[i].display = true; // ak47 sound
            game->bullets[i].goingRight = !game->p_p2->facingLeft;
            game->bullets[i].x = game->p_p2->facingLeft ? game->p_p2->x : (game->p_p2->x + 40);
            game->bullets[i].y = game->p_p2->y + 20;

            break;
        }

        game->p_p2->shooting = true;
    }
    else
        game->p_p2->shooting = false;
}

bool colDetected(int type, Man *man)
{
    switch(type)
    {
        case 1: //up
            if(man->y < 0)
            {
                man->y = 0;
                return true;
            }
            else
                return 0;
        case 2: //down
            if(man->y + 50 > HEIGHT)
            {
                man->y = HEIGHT - 50;
                return true;
            }
            else
                return 0;
        case 3: //right
            if(man->x + 40 > WIDTH)
            {
                man->x = WIDTH - 40;
                return true;
            }
            else
                return 0;
        case 4: //left
            if(man->x < 0)
            {
                man->x = 0;
                return true;
            }
            else
                return 0;
    }
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





