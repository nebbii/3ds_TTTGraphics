#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MAX_SPRITES  100
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

u32 kDown;

C3D_RenderTarget* bottom;

typedef struct
{
    C2D_Sprite spr;
    float dx, dy;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite cellSprites[9];
int lastSprite;

int turn;

void drawCell(int sprIndex, int x, int y) {
    Sprite* cell = &cellSprites[++lastSprite];
    C2D_SpriteFromSheet(&cell->spr, spriteSheet, sprIndex);
    C2D_SpriteSetCenter(&cell->spr, 0.5f, 0.5f);
    cell->dx = static_cast<float>(x);
    cell->dy = static_cast<float>(y);
}

void resetBoard() {
    for(int i = 0; i < 9; i++) {
        drawCell(0, (20 * i), (30 * i));
    }
    printf("\x1b[9;1HHello world! %i has won!", 2);
}

/*
void detectCell(int px, int py) {

}
*/

void setup() {
    Sprite** cellSprites = new Sprite*[20]; 
    lastSprite = -1;
}

void draw() {
    // setup frame
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)); 
    C2D_SceneBegin(bottom);

    // draw cells
    for (size_t i = 0; i < lastSprite; i++) {
        C2D_SpriteSetPos(&cellSprites[i].spr, cellSprites[i].dx, cellSprites[i].dy);
        C2D_DrawSprite(&cellSprites[i].spr);
    }

    C3D_FrameEnd(0);
}

void input(u32 kDown) {
    touchPosition touch;
    hidTouchRead(&touch);

    //if(kDown & KEY_TOUCH)
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

    resetBoard();

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
