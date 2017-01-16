/**
 *  Header file for theGame.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

                        case SDL_SCANCODE_P:
                            p_action->p2Shooting = true;
                            break;
                        case SDL_SCANCODE_RETURN:
                            initNewGame(*game);
                            break;
                        case SDL_SCANCODE_Q:
                            *running = false;
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
    SDL_Rect srcRect2 = { 40*(game.p_p2->currentSprite), 0, 40, 50};
    SDL_Rect rect2 = { game.p_p2->x, game.p_p2->y, 40, 50};
    SDL_RenderCopyEx(renderer, game.p_p2->sheetTexture, &srcRect2, &rect2, 0, NULL, game.p_p2->facingLeft);

    // game over
    if(game.gameIsOver)
    {
        drawText(renderer, "GAME OVER", WIDTH / 2 - 297 , HEIGHT / 2 - 50, 100);
        if(game.p_p1->hp > game.p_p2->hp)
             drawText(renderer, "PLAYER 1 WON", WIDTH / 2 - 237 , HEIGHT / 2 - 200, 60);
        else
             drawText(renderer, "PLAYER 2 WON", WIDTH / 2 - 237 , HEIGHT / 2 - 200, 60);
    }

}

void logicStuff(gameState game, gameState *p_game)
{
    // player1
    int manVelX = 0;
    int manVelY = 0;

    if (game.action->up && !game.action->down) manVelY = -SPEED;
    if (!game.action->up && game.action->down) manVelY = SPEED;
    if (game.action->left && !game.action->right)
    {
        manVelX = -SPEED;
        game.p_p1->facingLeft = true;
    }
    if (game.action->right && !game.action->left)
    {
        manVelX = SPEED;
        game.p_p1->facingLeft = false;
    }

    game.p_p1->x += (int)manVelX/60;
    game.p_p1->y += (int)manVelY/60;

    // player2
    manVelX = 0;
    manVelY = 0;

    if (game.action->up2 && !game.action->down2) manVelY = -SPEED;
    if (!game.action->up2 && game.action->down2) manVelY = SPEED;
    if (game.action->left2 && !game.action->right2)
    {
        manVelX = -SPEED;
        game.p_p2->facingLeft = true;
    }
    if (game.action->right2 && !game.action->left2)
    {
        manVelX = SPEED;
        game.p_p2->facingLeft = false;
    }

    game.p_p2->x += (int)manVelX/60;
    game.p_p2->y += (int)manVelY/60;

    // moving bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game.bullets[i].display)
    {
        game.bullets[i].x = game.bullets[i].x + (game.bullets[i].goingRight? BULLETSPEED/60 : -BULLETSPEED/60);

        // p1 bullet from the left side
        if( game.p_p1->x + 14 > game.bullets[i].x && game.p_p1->x - 3< game.bullets[i].x && game.bullets[i].y > game.p_p1->y && game.bullets[i].y < game.p_p1->y + 50 && game.bullets[i].goingRight)
        {
            game.bullets[i].display = false;
            if(!game.gameIsOver)
            game.p_p1->hp--;
        }
        // p1 bullet from the right side
        else if( game.p_p1->x - 3 < game.bullets[i].x && game.p_p1->x + 14 > game.bullets[i].x && game.bullets[i].y > game.p_p1->y && game.bullets[i].y < game.p_p1->y + 50 && !game.bullets[i].goingRight)
        {
            game.bullets[i].display = false;
            if(!game.gameIsOver)
            game.p_p1->hp--;
        }
        // p2 bullet from the left side
        else if( game.p_p2->x + 14 > game.bullets[i].x && game.p_p2->x - 3< game.bullets[i].x && game.bullets[i].y > game.p_p2->y && game.bullets[i].y < game.p_p2->y + 50 && game.bullets[i].goingRight)
        {
            game.bullets[i].display = false;
            if(!game.gameIsOver)
                game.p_p2->hp--;
        }
        // p2 bullet from the right side
        else if( game.p_p2->x - 3 < game.bullets[i].x && game.p_p2->x + 14 > game.bullets[i].x && game.bullets[i].y > game.p_p2->y && game.bullets[i].y < game.p_p2->y + 50 && !game.bullets[i].goingRight)
        {
            game.bullets[i].display = false;
            if(!game.gameIsOver)
                game.p_p2->hp--;
        }

    }

    // deleting bullets
    for(int i = 0; i < MAXBULLETS; i++) if(game.bullets[i].display)
    {
        if (!isInWindow(game.bullets[i].x,game.bullets[i].y))
        {
            game.bullets[i].display = false;
        }

    }

    // generating bullets
    if (game.action->p1Shooting && (game.frames % 3) == 0)
    {
        for(int i = 0; i < MAXBULLETS; i++) if(!game.bullets[i].display)
        {
            fflush(stdout);
            game.bullets[i].display = true;
            game.bullets[i].goingRight = !game.p_p1->facingLeft;
            game.bullets[i].x = game.p_p1->facingLeft ? game.p_p1->x : (game.p_p1->x + 40);
            game.bullets[i].y = game.p_p1->y + 20;

            break;
        }

        game.p_p1->shooting = true;
    }
    else
        game.p_p1->shooting = false;

    if (game.action->p2Shooting && (game.frames % 3) == 0)
    {

        for(int i = 0; i < MAXBULLETS; i++) if(!game.bullets[i].display)
        {
            fflush(stdout);
            game.bullets[i].display = true;
            game.bullets[i].goingRight = !game.p_p2->facingLeft;
            game.bullets[i].x = game.p_p2->facingLeft ? game.p_p2->x : (game.p_p2->x + 40);
            game.bullets[i].y = game.p_p2->y + 20;

            break;
        }

        game.p_p2->shooting = true;
    }
    else
        game.p_p2->shooting = false;

    if((game.p_p1->hp <= 0 || game.p_p2->hp <= 0) && !game.gameIsOver)
        p_game->gameIsOver = true;

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

void initNewGame(gameState game)
{
    game.gameIsOver = false;

    game.p_p1->hp = 50;
    game.p_p1->x = 50;
    game.p_p1->y = HEIGHT - 100;
    game.p_p1->facingLeft = false;
    game.p_p1->alive = true;
    game.p_p1->currentSprite = 4;

    game.p_p2->hp = 50;
    game.p_p2->x = WIDTH - 100;
    game.p_p2->y = HEIGHT - 100;
    game.p_p2->facingLeft = true;
    game.p_p2->alive = true;
    game.p_p2->currentSprite = 4;
}






