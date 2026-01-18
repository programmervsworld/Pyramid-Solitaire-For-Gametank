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
int deck[52];
SpriteSlot testSlot;
SpriteSlot textSlot;
int frame = 1;

void setup()
{
    int i = 0;
    testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
    loadDeck(deck);
    shuffleDeck(deck);
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
        int rows, cols, cardx, cardy, num, counter;

        num = 0;
        counter = 1;

        queue_clear_screen(3);
        queue_clear_border(0);

        for (rows = 0; rows < 7; rows++)
        {
            cardy = divy[rows];

            if(rows != 0){
                cardy -= rows * 4;
            }
           
            for (cols = 0; cols < counter; cols++)
            {
                cardx = divx[cols];
                queue_draw_sprite_frame(testSlot, cardx, cardy, deck[num], false);
                num++;
            }
            counter++;
        }

        // queue_draw_sprite_frame(testSlot, box_x, box_y, deck[frame], false);

        /*box_x += dx;
        box_y += dy;
        if (box_x == 1)
        {
            dx = 1;
            incrementFrame();
        }
        else if (box_x == 119)
        {
            dx = -1;
            incrementFrame();
        }
        if (box_y == 8)
        {
            dy = 1;
            incrementFrame();
        }
        else if (box_y == 112)
        {
            dy = -1;
            incrementFrame();
        }*/

        await_draw_queue();
        await_vsync(1);
        flip_pages();
    }
}