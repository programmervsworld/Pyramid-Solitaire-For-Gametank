#ifndef CARDS_H
#define CARDS_H

#include "gt/feature/random/random.h"

int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int divx[] = {16, 32, 48, 64, 80, 96, 112};
int divy[] = {16, 32, 48, 64, 80, 96, 112};
int board[7][7];
int deck[52];


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

void loadDeck(int deck[])
{
    int i = 0;
    for (i = 1; i < 52; i++)
    {
        deck[i - 1] = i;
    }
}

void shuffleDeck(int cards[])
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

#endif