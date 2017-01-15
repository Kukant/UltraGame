/**
 *  theGame
 *  Author: Tomas K.
 *
 *
 *  linker options: -std=c99 -lSDL2 -lSDL2_image -lSDL2_ttf
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "theGame.h"
//#include "functions.c"

#define HEIGHT 540
#define WIDTH 960
#define SPEED 300
#define BULLETSPEED 700
#define MAXBULLETS 1000

//************************************MAIN******************
int main()
{
    // initializing
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    // create a SDL window
    SDL_Window *window = SDL_CreateWindow(
                                          "The game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH,
                                          HEIGHT,
                                          SDL_WINDOW_OPENGL
                                        );
    // create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;

    // init action
    Action *p_action = malloc(sizeof(Action));
    p_action->down = p_action->up = p_action->left = p_action->right = p_action->p1Shooting = false;
    p_action->down2 = p_action->up2 = p_action->left2 = p_action->right2 = p_action->p2Shooting = false;


    bool running = true;

    //SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    // init bullets
    Bullet bullets[MAXBULLETS];
    for(int i = 0; i < MAXBULLETS; i++)
        bullets[i].display = false;

    // setting up the players
    Man player1 = {.x = 50, .y = HEIGHT - 100, .facingLeft = false, .alive = 1, .currentSprite = 4 , .hp = 50};
    Man player2 = {.x = WIDTH - 100, .y = HEIGHT - 100, .facingLeft = true, .alive = 1, .currentSprite = 4 , .hp = 50};

    // gameState init
    gameState game = {.p_p1 = &player1, .p_p2 = &player2, .action = p_action, .bullets = bullets, .frames = 0, .gameIsOver = false};

    // loading images
    SDL_Surface *sheet;
    if((sheet = IMG_Load("sheet.png")) == NULL)
            printf("sheet.png not found\n");
    player1.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
    if((sheet = IMG_Load("badman_sheet.png")) == NULL)
            printf("badman_sheet.png not found\n");
    player2.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);
    if((sheet = IMG_Load("banana.png")) == NULL)
            printf("banana.png not found\n");
    game.bulletTexture = SDL_CreateTextureFromSurface(renderer, sheet);
    SDL_FreeSurface(sheet);

    // Animation loop
    while(running)
    {
        // events processing
        while(SDL_PollEvent(&event))
        {
            eventsDetection(&event, p_action, &running, &game);
        }

        // logic stuff
        logicStuff(game, &game);

        // rendering
        renderStuff(renderer, game);

        // Show what was drawn
        SDL_RenderPresent(renderer);

        game.frames = game.frames + 1;

    } // end of animation loop

    // releasing resources
    free(p_action);
    SDL_DestroyTexture(game.bulletTexture);
    SDL_DestroyTexture(player1.sheetTexture);
    SDL_DestroyTexture(player2.sheetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
