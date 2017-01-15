/**
 *  Header file for theGame.
 *
 */

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

    Action *action;
    SDL_Texture *bulletTexture;

} gameState;

/**
 *  Function detects keyboard actions.
 */
void eventsDetection(SDL_Event *event, Action *action, bool *running);
void renderStuff(SDL_Renderer *renderer, gameState game);
void logicStuff(gameState game);
int isInWindow(int x, int y);
