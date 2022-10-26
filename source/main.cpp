#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MAX_SPRITES  100

#define PLAYW  160
#define PLAYH  160

u32 kDown;

C3D_RenderTarget* bottom;

typedef struct
{
    C2D_Sprite spr;
    float x, y;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite cellSprites[9];
int lastSprite;

int turn;
int board[9];

void createCell(int sprIndex, int x, int y) {
    Sprite* cell = &cellSprites[++lastSprite];
    C2D_SpriteFromSheet(&cell->spr, spriteSheet, 2);

    cell->x = static_cast<float>(x);
    cell->y = static_cast<float>(y);
}

void drawBoard() {
    int index = -1;
    for(int w = 1; w <= 3; w++) {
        for(int h = 1; h <= 3; h++) {
            createCell(board[++index], 30 + PLAYH / 3 * h, -16 + PLAYW / 3 * w);
        }
    }
}

/*
int* clearBoard() {
    int arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    return arr;
}
*/

int detectCell(int px, int py) {
    // 48x48
    for(int i = 0; i <= lastSprite; i++) {
        if ((px > cellSprites[i].x && px < cellSprites[i].x + 48) &&
            (py > cellSprites[i].y && py < cellSprites[i].y + 48)) {
                return i;
           }
    }
    return -1;
}


void setup() {
    Sprite** cellSprites = new Sprite*[20]; 
    int board[9] = {1, 2, 1, 0, 0, 0, 0, 0, 0};
    lastSprite = -1;
}

void draw() {
    // setup frame
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)); 
    C2D_SceneBegin(bottom);

    // draw cells
    for (size_t i = 0; i <= lastSprite; i++) {
        C2D_SpriteSetPos(&cellSprites[i].spr, cellSprites[i].x, cellSprites[i].y);
        C2D_DrawSprite(&cellSprites[i].spr);
    }

    C3D_FrameEnd(0);
}

void input(u32 kDown) {
    touchPosition touch;
    hidTouchRead(&touch);

    printf("\x1b[1;0HX coordinate: %i     ",touch.px);
    printf("\x1b[2;0HY coordinate: %i     ",touch.py);

    //if(kDown & KEY_TOUCH) {
        int tappedCell = detectCell(touch.px, touch.py);
        printf("\x1b[9;0Hits in the hole: %i     ", tappedCell);
   // }
}

void logic() {

}


int main(int argc, char **argv) {
    // init 
    romfsInit();
    gfxInitDefault();
	gfxSetDoubleBuffering(GFX_BOTTOM, true);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_TOP, NULL);

    // Setup target (bottom screen)
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Load sprite files
    spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    if (!spriteSheet) svcBreak(USERBREAK_PANIC);

    // double buffering is fast
	u8* frameBuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

    setup();

    drawBoard();

    while(aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        input(kDown);
        draw();

        if (kDown & KEY_START) break;
    }

    C2D_SpriteSheetFree(spriteSheet);

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();

    return 0;
}
