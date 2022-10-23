#include <citro2d.h>
#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define MAX_SPRITES  100
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

int frame;
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

    C2D_SpriteFromSheet(&nebi->spr, spriteSheet, 0);
    C2D_SpriteSetCenter(&nebi->spr, 0.5f, 0.5f);
    C2D_SpriteSetPos(&nebi->spr, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2);
    nebi->dx = 10.0f;
    nebi->dy = 10.0f;
    printf("\x1b[4;0Hnebi X: %f", nebi->dx);
    printf("\x1b[5;0Hnebi Y: %f", nebi->dy);

    Sprite* squart = &sprites[1];

    C2D_SpriteFromSheet(&squart->spr, spriteSheet, 1);
    C2D_SpriteSetCenter(&squart->spr, 0.5f, 0.5f);
    C2D_SpriteSetPos(&squart->spr, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2);
}

void draw() {
    printf("\x1b[1;0HHello World!");
    printf("\x1b[2;0HFramecount: %i", frame);

    frame++;

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)); 
    C2D_SceneBegin(bottom);
    for (size_t i = 0; i < 2; i++) {
        C2D_DrawSprite(&sprites[i].spr);
    }
    C3D_FrameEnd(0);
}

void input() {
    //hidScanInput();
    //u32 kDown = hidKeysDown();
}

void logic() {

}


int main(int argc, char **argv) {
    // init 
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_TOP, NULL);

	gfxSetDoubleBuffering(GFX_BOTTOM, true);

    // Setup target (bottom screen)
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    // Load sprite files
    spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    if (!spriteSheet) svcBreak(USERBREAK_PANIC);

    // double buffering is fast
	u8* frameBuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);

    frame = 0;

    setup();

    while(aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        draw();
        input();
        logic();

        if (kDown & KEY_START) break;
    }

    C2D_SpriteSheetFree(spriteSheet);

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();

    return 0;
}
