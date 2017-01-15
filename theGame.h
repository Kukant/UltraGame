/**
 *  Header file for theGame.
 *
 */

 #include <SDL2/SDL_ttf.h>

 // macro definitions
#define HEIGHT 540
#define WIDTH 960
#define SPEED 300
#define MAXBULLETS 1000
#define BULLETSPEED 700


 // structs definitions
typedef struct{
    bool up, down, left, right, up2, down2, left2, right2;
    bool p1Shooting, p2Shooting;
} Action;

typedef struct{
    int x;
    int y;
    bool walking, shooting, alive, facingLeft;
    int currentSprite;
    int hp;

    SDL_Texture *sheetTexture;
} Man;

typedef struct{
    int x, y;
    bool display; // is it on the screen
    bool goingRight;
} Bullet;

typedef struct{
    Man *p_p1;
    Man *p_p2;
    Bullet *bullets;
    bool gameIsOver;

    Action *action;
    SDL_Texture *bulletTexture;

    int frames;

} gameState;

/**
 *  Function detects keyboard actions.
 */
void eventsDetection(SDL_Event *event, Action *action, bool *running, gameState *game);
void renderStuff(SDL_Renderer *renderer, gameState game);
void logicStuff(gameState game, gameState *p_game);
int isInWindow(int x, int y);
void drawText(SDL_Renderer *renderer, char *text, int x, int y, int size);
void initNewGame(gameState game);
