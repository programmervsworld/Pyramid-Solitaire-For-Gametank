#ifndef CARDS_H
#define CARDS_H

#include "gt/feature/random/random.h"

#define DISCARD_PILE_X 48
#define DISCARD_PILE_Y 112
#define FLIPPED_PILE_X 80
#define FLIPPED_PILE_Y 112

int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int divx[] = {16, 32, 48, 64, 80, 96, 112};
int divy[] = {16, 32, 48, 64, 80, 96, 112};
int board[7][7];
int deck[52];

SpriteSlot testSlot;
SpriteSlot textSlot;


void loadPyramidBoard()
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

void loadPyramidDeck(int deck[])
{
    int i = 0;
    for (i = 1; i < 52; i++)
    {
        deck[i - 1] = i;
    }
}

void shufflePyramidDeck(int cards[])
{
    int valueToSwap;
    int indexToSwap;
    int i;

    for (i = 0; i < 51; i++)
    {
        indexToSwap = rnd_range(1, 50);
        valueToSwap = cards[indexToSwap];
        cards[indexToSwap] = cards[1];
        cards[1] = valueToSwap;
    }
}

void initializePyramidScene(){
    testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
    loadPyramidDeck(deck);
    shufflePyramidDeck(deck);
    loadPyramidBoard();
}

void renderPyramidBoard()
{
    int rows, cols, cardx, cardy, counter;

    counter = 1;
    cardx = 64;

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
}

#endif