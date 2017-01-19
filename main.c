/**
 *  theGame
 *  Author: Tomas K.
 *
 *
 *  linker options: -std=c99 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


#include "theGame.h"

#define HEIGHT 1080 //540
#define WIDTH 1920 //960
#define SPEED 300
#define BULLETSPEED 700
#define MAXBULLETS 1000
#define GRAVITY 0.50
#define MAXLEDGES 300


//************************************MAIN******************
int main()
{
    // initializing
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);



    // create a SDL window
    SDL_Window *window = SDL_CreateWindow(
                                          "The game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH,
                                          HEIGHT,
                                          SDL_WINDOW_OPENGL
                                        );
    // set window fullscreen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;

    // init action
    Action *p_action = malloc(sizeof(Action));
    p_action->down = p_action->up = p_action->left = p_action->right = p_action->p1Shooting = false;
    p_action->down2 = p_action->up2 = p_action->left2 = p_action->right2 = p_action->p2Shooting =  false;


    bool running = true;

    // init audio
    Mix_Chunk *ak47 = Mix_LoadWAV("ak47.wav");

    // init bullets
    Bullet bullets[MAXBULLETS];
    for(int i = 0; i < MAXBULLETS; i++)
        bullets[i].display = false;

    // init ledges
    Ledge ledges[MAXLEDGES];
    for(int i = 0; i < MAXLEDGES && (i - 1)*300 < WIDTH; i++)
    {
        ledges[i].x = i * 300;
        ledges[i].y = HEIGHT - 10;
        ledges[i].w = 300;
        ledges[i].h = 10;
    }

    // setting up the players
    Man player1 = {.x = 50, .y = HEIGHT - 100, .facingLeft = false, .alive = 1, .currentSprite = 4 , .hp = 50, .AI = false, .dy = 0};
    Man player2 = {.x = WIDTH - 100, .y = HEIGHT - 100, .facingLeft = true, .alive = 1, .currentSprite = 4 , .hp = 50, .AI = false, .dy = 0};

    // starttime
    time_t startTime = time(NULL);

    // gameState init
    gameState game = {.p_p1 = &player1, .p_p2 = &player2, .action = p_action, .bullets = bullets, .frames = 0, .gameIsOver = false, .renderer = renderer, .ak47 = ak47, .walkAI = true,
                      .startTime = startTime, .ledges = ledges, .p_texts = malloc(sizeof(Texts)), .lastHit = 0};

    // init of texts
    TTF_Init();
    TTF_Font *Blox2 = TTF_OpenFont("Blox2.ttf", 100);
    SDL_Color Red = {255, 0, 0};

    SDL_Surface *textSurface = TTF_RenderText_Solid(Blox2, "GAME OVER", Red);
    game.p_texts->gameOver = SDL_CreateTextureFromSurface(game.renderer, textSurface);
    int lenght = 9;
    game.p_texts->gORect.x = WIDTH / 2 - 297;
    game.p_texts->gORect.y =  30;
    game.p_texts->gORect.w = 100*lenght*0.66;
    game.p_texts->gORect.h = 100;

    TTF_CloseFont(Blox2);
    Blox2 = TTF_OpenFont("Blox2.ttf", 60);

    textSurface = TTF_RenderText_Solid(Blox2, "PLAYER 1 WON", Red);
    game.p_texts->p1Won = SDL_CreateTextureFromSurface(game.renderer, textSurface);
    lenght = 12;
    game.p_texts->wRect.x = WIDTH / 2 - 237;
    game.p_texts->wRect.y = HEIGHT / 2 - 200;
    game.p_texts->wRect.w = 60*lenght*0.66;
    game.p_texts->wRect.h = 60;

    textSurface = TTF_RenderText_Solid(Blox2, "PLAYER 2 WON", Red);
    game.p_texts->p2Won = SDL_CreateTextureFromSurface(game.renderer, textSurface);

    game.p_texts->tRect.x = WIDTH / 2 - 198;
    game.p_texts->tRect.y = HEIGHT / 2 - 320;
    game.p_texts->tRect.w = 60*9*0.66;
    game.p_texts->tRect.h = 60;

    TTF_CloseFont(Blox2);
    SDL_FreeSurface(textSurface);


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
    if((sheet = IMG_Load("background.png")) == NULL)
            printf("background.png not found\n");
    game.backTexture = SDL_CreateTextureFromSurface(renderer, sheet);
    if((sheet = IMG_Load("wall.png")) == NULL)
            printf("wall.png not found\n");
    game.ledgeTexture = SDL_CreateTextureFromSurface(renderer, sheet);
    SDL_FreeSurface(sheet);


    // Animation loop
    while(running)
    {
        // events processing
        while(SDL_PollEvent(&event))
        {
            eventsDetection(&event, p_action, &running, &game, window);
        }

        // logic stuff
        logicStuff(game, &game);

        // rendering
        renderStuff(renderer, game);

        // Show what was drawn
        SDL_RenderPresent(renderer);

        game.frames = game.frames + 1;

        //if (!game.gameIsOver)
            SDL_Delay(500/60);



    } // end of animation loop

    // releasing resources
    free(p_action);
    SDL_DestroyTexture(game.p_texts->gameOver);
    SDL_DestroyTexture(game.p_texts->p1Won);
    SDL_DestroyTexture(game.p_texts->p2Won);
    SDL_DestroyTexture(game.p_texts->time);
    SDL_DestroyTexture(game.p_texts->hp1);
    SDL_DestroyTexture(game.p_texts->hp2);
    free(game.p_texts);
    TTF_Quit();
    SDL_DestroyTexture(game.bulletTexture);
    SDL_DestroyTexture(player1.sheetTexture);
    SDL_DestroyTexture(player2.sheetTexture);
    SDL_DestroyTexture(game.backTexture);
    SDL_DestroyTexture(game.ledgeTexture);
    Mix_FreeChunk(ak47);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
