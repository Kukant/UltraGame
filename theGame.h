/**
 *  Header file for theGame.
 *

 */
 #include <SDL2/SDL_ttf.h>
 #include <SDL2/SDL_mixer.h>
 #include <sys/time.h>

 // macro definitions
#define HEIGHT 1080 //540
#define WIDTH 1920 //960
#define SPEED 3
#define MAXBULLETS 1000
#define BULLETSPEED 700
#define GRAVITY 0.14f
#define MAXLEDGES 300

 // structs definitions
typedef struct{
    bool up, down, left, right, up2, down2, left2, right2;
    bool p1Shooting, p2Shooting;
} Action;

typedef struct{
    SDL_Texture *p1Won, *p2Won, *gameOver, *time, *hp1, *hp2;
    SDL_Rect wRect, gORect, tRect,hp1Rect,hp2Rect;
}Texts;

typedef struct{
    float x, y, dy;
    bool walking, shooting, alive, facingLeft, onLedge, jump;
    int currentSprite;
    int hp;

    SDL_Texture *sheetTexture;
} Man;

typedef struct{
    float x, y, w, h;
    bool display; // is it on the screen
    bool goingRight;


} Bullet;

typedef struct{
    float x, y, w, h;
    bool vertical, drawn;
}Ledge;

typedef struct{
    Man *p_p1;
    Man *p_p2;
    Bullet *bullets;
    Ledge *ledges;
    bool gameIsOver;
    Texts *p_texts;

    Mix_Chunk *ak47;

    Action *action;
    SDL_Texture *bulletTexture, *backTexture, *ledgeTexture, *ledgeTextureYX;
    SDL_Renderer *renderer;
    int startTime, endTime;

    unsigned long long int frames, lastHit;

} gameState;

/**
 *  Function detects keyboard actions.
 */
void eventsDetection(SDL_Event *event, Action *action, bool *running, gameState *game, SDL_Window *window);
void renderStuff(SDL_Renderer *renderer, gameState game);
void logicStuff(gameState *game);
int isInWindow(int x, int y);
void initNewGame(gameState *game);
void movingBullets(gameState *game);
void merge(char *result, char *one, char *two, char *three);
void setLedges(Ledge *ledges);
void initTexts(gameState game);
void loadImages(gameState *game);
void collDetect(gameState *game, Man *man);
bool ledgeBullDetect(Bullet bullet, gameState *game);

