/**
 *  Header file for theGame.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "theGame.h"

void eventsDetection(SDL_Event *event, Action *p_action, bool *running)
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
                        case SDL_SCANCODE_UP:
                            p_action->up2 = true;
                            break;
                        case SDL_SCANCODE_S:
                            p_action->down = true;
                            break;
                        case SDL_SCANCODE_DOWN:
                            p_action->down2 = true;
                            break;
                        case SDL_SCANCODE_D:
                            p_action->right = true;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = true;
                            break;
                        case SDL_SCANCODE_A:
                            p_action->left = true;
                            break;
                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = true;
                            break;
                        case SDL_SCANCODE_SPACE:
                            p_action->p1Shooting = true;
                            break;
                        case SDL_SCANCODE_P:
                            p_action->p2Shooting = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event->key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            p_action->up = false;
                            break;
                        case SDL_SCANCODE_UP:
                            p_action->up2 = false;
                            break;
                        case SDL_SCANCODE_S:
                            p_action->down = false;
                            break;
                        case SDL_SCANCODE_DOWN:
                            p_action->down2 = false;
                            break;
                        case SDL_SCANCODE_D:
                            p_action->right = false;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            p_action->right2 = false;
                            break;
                        case SDL_SCANCODE_A:
                            p_action->left = false;
                            break;
                        case SDL_SCANCODE_LEFT:
                            p_action->left2 = false;
                            break;
                        case SDL_SCANCODE_SPACE:
                            p_action->p1Shooting = false;
                            break;
                        case SDL_SCANCODE_P:
                            p_action->p2Shooting = false;
                            break;
                    }
                    break;
            }
}

void renderStuff(SDL_Renderer *renderer, gameState game)
{
    // setting the wall.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // bullets
    for(int i = 0; i < MAXBULLETS; i++) if (game.bullets[i].display)
    {
        SDL_Rect bRect = { game.bullets[i].x, game.bullets[i].y, 8, 8 };
        SDL_RenderCopy(renderer, game.bulletTexture, NULL, &bRect);
        //printf("rendered bullet %d [%d][%d]",i,game.bullets[i].x, game.bullets[i].y);
        //fflush(stdout);
    }


    // player1
    SDL_Rect srcRect = { 40*(game.p_p1->currentSprite), 0, 40, 50};
    SDL_Rect rect = { game.p_p1->x, game.p_p1->y, 40, 50};
    SDL_RenderCopyEx(renderer, game.p_p1->sheetTexture, &srcRect, &rect,0 , NULL, game.p_p1->facingLeft);

    // player2
    SDL_Rect srcRect2 = { 40*(game.p_p2->currentSprite), 0, 40, 50};
    SDL_Rect rect2 = { game.p_p2->x, game.p_p2->y, 40, 50};
    SDL_RenderCopyEx(renderer, game.p_p2->sheetTexture, &srcRect2, &rect2, 0, NULL, game.p_p2->facingLeft);

}

void logicStuff(gameState game)
{
    // player1
    int manVelX = 0;
    int manVelY = 0;

    if (game.action->up && !game.action->down) manVelY = -SPEED;
    if (!game.action->up && game.action->down) manVelY = SPEED;
    if (game.action->left && !game.action->right) manVelX = -SPEED;
    if (game.action->right && !game.action->left) manVelX = SPEED;

    game.p_p1->x += (int)manVelX/60;
    game.p_p1->y += (int)manVelY/60;

    // player2
    manVelX = 0;
    manVelY = 0;

    if (game.action->up2 && !game.action->down2) manVelY = -SPEED;
    if (!game.action->up2 && game.action->down2) manVelY = SPEED;
    if (game.action->left2 && !game.action->right2) manVelX = -SPEED;
    if (game.action->right2 && !game.action->left2) manVelX = SPEED;

    game.p_p2->x += (int)manVelX/60;
    game.p_p2->y += (int)manVelY/60;

    // moving bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game.bullets[i].display)
            game.bullets[i].x = game.bullets[i].x + (game.bullets[i].goingRight? BULLETSPEED/60 : -BULLETSPEED/60);

    // deleting bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game.bullets[i].display)
    {
        if (!isInWindow(game.bullets[i].x,game.bullets[i].y))
        {
            game.bullets[i].display = false;
            printf("deleted bullet number %d ",i);
        }

    }

    // generating bullets
    if (game.action->p1Shooting)
        for(int i = 0; i < MAXBULLETS; i++) if(!game.bullets[i].display)
        {
            printf("shot");
            fflush(stdout);
            game.bullets[i].display = true;
            game.bullets[i].goingRight = !game.p_p1->facingLeft;
            game.bullets[i].x = game.p_p1->facingLeft ? game.p_p1->x : (game.p_p1->x + 40);
            game.bullets[i].y = game.p_p1->y + 20;

            break;
        }
    if (game.action->p2Shooting)
        for(int i = 0; i < MAXBULLETS; i++) if(!game.bullets[i].display)
        {
            printf("shot");
            fflush(stdout);
            game.bullets[i].display = true;
            game.bullets[i].goingRight = !game.p_p2->facingLeft;
            game.bullets[i].x = game.p_p2->facingLeft ? game.p_p2->x : (game.p_p2->x + 40);
            game.bullets[i].y = game.p_p2->y + 20;

            break;
        }

}

int isInWindow(int x, int y)
{
    return (x > 0 && x < WIDTH && y > 0 && y < HEIGHT)? 1 : 0;
}










