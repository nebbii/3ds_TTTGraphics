#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MAX_SPRITES  100
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240


int frame;
u32 kDown;

C3D_RenderTarget* bottom;

typedef struct
{
    C2D_Sprite spr;
    float dx, dy;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[2];

void setup() {
    Sprite* nebi = &sprites[0];

    nebi->dx = 100;
    nebi->dy = 50;
    C2D_SpriteFromSheet(&nebi->spr, spriteSheet, 0);
    C2D_SpriteSetCenter(&nebi->spr, 0.5f, 0.5f);
    C2D_SpriteSetPos(&nebi->spr, nebi->dx, nebi->dy);
    C2D_SpriteMove(&nebi->spr, nebi->dx, nebi->dy);

    Sprite* squart = &sprites[1];

    C2D_SpriteFromSheet(&squart->spr, spriteSheet, 1);
    C2D_SpriteSetCenter(&squart->spr, 0.5f, 0.5f);
    C2D_SpriteSetPos(&squart->spr, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2);
}

void draw() {
    Sprite* nebi = &sprites[0];

    printf("\x1b[2;0HFramecount: %i", frame);
    printf("\x1b[4;0Hnebi X: %f", nebi->dx);
    printf("\x1b[5;0Hnebi Y: %f", nebi->dy);

    frame++;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)); 
    C2D_SceneBegin(bottom);
    for (size_t i = 0; i < 2; i++) {
        C2D_SpriteSetPos(&sprites[i].spr, sprites[i].dx, sprites[i].dy);
        C2D_DrawSprite(&sprites[i].spr);
    }
    C3D_FrameEnd(0);
}

void input() {
    Sprite* nebi = &sprites[0];
    hidScanInput();
    u32 kDown = hidKeysDown();

    if (kDown & KEY_UP) nebi->dy -= 10;
    if (kDown & KEY_DOWN) nebi->dy += 10;
    if (kDown & KEY_LEFT) nebi->dx -= 10;
    if (kDown & KEY_RIGHT) nebi->dx += 10;
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

    frame = 0;

    setup();

    while(aptMainLoop()) {
        //hidScanInput();
        //u32 kDown = hidKeysDown();

        input();
        draw();

        //if (kDown & KEY_START) break;
    }

    C2D_SpriteSheetFree(spriteSheet);

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();

    return 0;
}
