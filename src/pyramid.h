#ifndef CARDS_H
#define CARDS_H

#include "input.h"
#include "gt/feature/random/random.h"
#include "../gen/assets/backgrounds.h"
#include "gt/feature/text/text.h"

#define DISCARD_PILE_X 48
#define DISCARD_PILE_Y 112
#define FLIPPED_PILE_X 80
#define FLIPPED_PILE_Y 112

// Used as a value table for figuring out what cards total to 13
int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

// These two arrays just hold the cards positions in the tableau grid.
int divx[] = {16, 32, 48, 64, 80, 96, 112};
int divy[] = {16, 32, 48, 64, 80, 96, 112};

// This is the playfield
int board[7][7];
int select[7][7];

// The is the deck that we deal from and where the next discard will come from
int deck[52];
int discardPtr = 0;
int cursorRow = 6;
int cursorCard = 0;
bool isOnBoard = true;
bool isOnDiscard = false;

SpriteSlot background;
SpriteSlot testSlot;
SpriteSlot textSlot;

char *selectionText = "Selected Card 1: ";

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
            discardPtr = num;
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

void draw_selection()
{
    text_init();
    text_cursor_x = 1;
    text_cursor_y = 7;
    text_color = TEXT_COLOR_BLACK;
    text_print_string(selectionText);
}

void initializePyramidScene()
{
    background = allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
    testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
    set_sprite_frametable(background, &ASSET__backgrounds__background1_json);
    loadPyramidDeck(deck);
    shufflePyramidDeck(deck);
    loadPyramidBoard();
}

void checkInput()
{
    if (player1_new_buttons & INPUT_MASK_RIGHT)
    {
        if (isOnBoard)
        {
            if (cursorCard < 6)
            {
                cursorCard++;
            }
        }
        else
        {
            isOnDiscard = false;
        }
    }
    else if (player1_new_buttons & INPUT_MASK_LEFT)
    {
        if (isOnBoard)
        {
            if (cursorCard > 0)
            {
                cursorCard--;
            }
        }
        else
        {
            isOnDiscard = true;
        }
    }
    else if (player1_new_buttons & INPUT_MASK_DOWN)
    {
        isOnBoard = false;
    }
    else if (player1_new_buttons & INPUT_MASK_UP)
    {
        isOnBoard = true;
    }
    else if (player1_new_buttons & INPUT_MASK_A)
    {
        selectionText = "Wow!";
    }
}

void renderPyramidBoard()
{
    int rows, cols, cardx, cardy, counter;

    counter = 1;
    cardx = 64;

    queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
    // queue_draw_sprite_frame(background, 1, 1, 1, false);

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
                // If the cursor is currently over this
                if (cursorRow == rows && cursorCard == cols && isOnBoard)
                {
                    queue_draw_box(cardx - 8, cardy - 8, 16, 16, rnd_range(0, 255));
                }

                // If the card we are is has a selection flag on it
                if (select[rows][cols])
                {
                    cardy = cardy + 8;
                }

                queue_draw_sprite_frame(testSlot, cardx, cardy, board[rows][cols], false);
            }
            cardx += 16;
        }
        counter++;
    }

    if (!isOnBoard && isOnDiscard)
    {
        queue_draw_box(DISCARD_PILE_X - 8, DISCARD_PILE_Y - 8, 16, 16, rnd_range(0, 255));
    }
    else if (!isOnBoard && !isOnDiscard)
    {
        queue_draw_box(FLIPPED_PILE_X - 8, FLIPPED_PILE_Y - 8, 16, 16, rnd_range(0, 255));
    }

    queue_draw_sprite_frame(testSlot, DISCARD_PILE_X, DISCARD_PILE_Y, 0, false);
    queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X, FLIPPED_PILE_Y, 0, false);

    checkInput();
}

#endif