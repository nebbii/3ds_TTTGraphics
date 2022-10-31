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

int turn; int state; int tapped;
int score[2];
static Sprite boardSprites[10];
static int board[10];
int lastSprite;


void createCell(int sprIndex, int x, int y) {
    Sprite* cell = &boardSprites[lastSprite];
    C2D_SpriteFromSheet(&cell->spr, spriteSheet, sprIndex);

    cell->x = static_cast<float>(x);
    cell->y = static_cast<float>(y);
}

void createBoard() {
    for(int w = 1; w <= 3; w++) {
        for(int h = 1; h <= 3; h++) {
            createCell(board[++lastSprite], 30 + PLAYH / 3 * h, -16 + PLAYW / 3 * w);
        }
    }
}

void clearBoard() {
    lastSprite = -1;

    memset(boardSprites, 0, sizeof(boardSprites));
    memset(board, 0, sizeof(board));
}

int detectCell(int px, int py) {
    // 48x48
    for(int i = 0; i <= lastSprite; i++) {
        if ((px > boardSprites[i].x && px < boardSprites[i].x + 48) &&
            (py > boardSprites[i].y && py < boardSprites[i].y + 48)) {
                return i;
           }
    }
    return -1;
}

int checkState() {
    for(int i=0; i<3; i++) {
        if((board[0+i*3]==1 && board[1+i*3]==1 && board[2+i*3]==1) ||
           (board[0+i]==1 && board[3+i]==1 && board[6+i]==1))
        {
            return 10;
        }
        else if((board[0+i*3]==2 && board[1+i*3]==2 && board[2+i*3]==2) ||
           (board[0+i]==2 && board[3+i]==2 && board[6+i]==2))
        {
            return 20;
        }
    }

    // diagonals
    if((board[0]==1 && board[4]==1 && board[8]==1) ||
       (board[2]==1 && board[4]==1 && board[6]==1))
    {
        return 10;
    }
    else if((board[0]==2 && board[4]==2 && board[8]==2) ||
       (board[2]==2 && board[4]==2 && board[6]==2))
    {
        return 20;
    }


    return 0;
}


void setup() {
    state = 0; turn = 1; tapped = -1;

    clearBoard();

    createBoard();
}

void draw() {
    // setup frame
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f)); 
    C2D_SceneBegin(bottom);

    // draw cells
    for (size_t i = 0; i <= lastSprite; i++) {
        C2D_SpriteFromSheet(&boardSprites[i].spr, spriteSheet, board[i]);
        C2D_SpriteSetPos(&boardSprites[i].spr, boardSprites[i].x, boardSprites[i].y);
        C2D_DrawSprite(&boardSprites[i].spr);
    }

    C3D_FrameEnd(0);
}

void input(u32 kDown) {
    touchPosition touch;
    hidTouchRead(&touch);

    tapped = -2;

    if(kDown & KEY_TOUCH) {
        tapped = detectCell(touch.px, touch.py);
    }

    printf("\x1b[5;0Htapped: %i     ", tapped);
}

void logic() {
    printf("\x1b[10;0Hnebi score:   %i    ", score[0]);
    printf("\x1b[11;0Hsquart score: %i    ", score[1]);

    if (state != 30) {
        state = checkState();
    }

    switch(state) {
        case 10:
            score[0]++;
            state = 30;
            printf("\x1b[13;0HNEBI WINS!");
            break;
        case 20:
            score[1]++;
            state = 30;
            printf("\x1b[13;0HSQUART WINS!");
            break;
        case 30:
            if(tapped > -2) {
                printf("\x1b[13;0H                    ");
                setup();
            }
            break;
        default:
            if(tapped > -1 && board[tapped] == 0) {
                board[tapped] = turn;
                turn = (turn == 1 ? 2 : 1);
            }
    }
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

    int score[2] = {0, 0};

    setup();

    while(aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        input(kDown);
        draw();
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
