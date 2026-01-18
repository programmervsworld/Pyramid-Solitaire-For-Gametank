#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "../gen/assets/cardframes.h"
#include "cards.h"

#define PIXEL_WIDTH 128
#define PIXEL_HEIGHT 128
#define CARD_HEIGHT 16
#define CARD_WIDTH 16

char box_x = 30, box_y = 20;
char dx = 1, dy = 1;
char rows = PIXEL_WIDTH / CARD_WIDTH;
char cols = PIXEL_HEIGHT / CARD_HEIGHT;
int board[7][7];
SpriteSlot testSlot;
SpriteSlot textSlot;
int frame = 1;

void loadBoard()
{
    int r, c, num, counter;
    num = 0;
    counter = 1;
    for (r = 0; r < 7; r++)
    {
        for (c = 0; c < counter; c++)
        {
            board[r][c] = deck[num];
            num++;
        }
        counter++;
    }
}

void setup()
{
    int i = 0;
    testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
    loadDeck(deck);
    shuffleDeck(deck);
    loadBoard();
}

void incrementFrame()
{
    if (++frame == 52)
    {
        frame = 0;
    }
}

void main()
{

    setup();

    while (1)
    { //  Run forever
        int rows, cols, cardx, cardy, counter;

        counter = 1;
        cardx = 64;

        queue_clear_screen(3);
        queue_clear_border(0);

        for (rows = 0; rows < 7; rows++)
        {
            cardy = divy[rows];

            if (rows != 0)
            {
                cardy -= rows * 4;
                cardx = 64 - ((16 * rows) / 2);
            }

            for (cols = 0; cols < counter; cols++)
            {
                if (board[rows][cols] != 0)
                {
                    queue_draw_sprite_frame(testSlot, cardx, cardy, board[rows][cols], false);
                }
                cardx += 16;
            }
            counter++;
        }

        await_draw_queue();
        await_vsync(1);
        flip_pages();
    }
}