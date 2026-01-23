#ifndef CARDS_H
#define CARDS_H

#include "../gen/assets/backgrounds.h"
#include "../gen/assets/numbers.h"
#include "gt/feature/random/random.h"
#include "input.h"

#define DISCARD_PILE_X 48
#define DISCARD_PILE_Y 104
#define FLIPPED_PILE_X 80
#define FLIPPED_PILE_Y 104
#define DISCARD_BEGINS 27
#define RANDOM_SEED 9
#define SELECT_SHIFT 5
#define SCORE_X 103
#define SCORE_Y 17

typedef struct {
  char row;
  char card;
} CardPosition;

// These two arrays just hold the cards positions in the tableau grid.
char divy[] = {12, 28, 44, 60, 76, 92, 108};

// This is the playfield
char board[7][7];
char select[7][7];
CardPosition eligibileCards[7];

// The is the deck that we deal from and where the next discard will come from
int deck[52];
char discardPtr = 0;
char cursorRow = 6;
char cursorCard = 0;
char eligPos = 0;
char eligCount = 7;
char numSelections = 0;
char deckflips = 0;
char remainingCardCount = 0;

bool isOnBoard = true;
bool isOnDiscard = false;
bool flipIsSelected = false;

SpriteSlot background;
SpriteSlot testSlot;
SpriteSlot remainingCards;

char *selectionText = "Selected Card 1: ";

bool hasWon() { return remainingCardCount == 0; }

void scanForEligibility() {
  char row, col, entry;
  for (entry = 0; entry < 6; entry++) {
    CardPosition pos;
    eligibileCards[entry] = pos;
  }
  entry = 0;
  for (col = 0; col < 7; col++) {
    for (row = 6; row > 0; row--) {
      if (row == 6 && board[row][col] > 0) {
        CardPosition pos;
        pos.row = row;
        pos.card = col;

        eligibileCards[entry] = pos;
        entry++;
      } else if (board[row][col] > 0 && board[row + 1][col] == 0 &&
                 board[row + 1][col + 1] == 0) {
        CardPosition pos;
        pos.row = row;
        pos.card = col;

        eligibileCards[entry] = pos;
        entry++;
      }
    }
  }
  eligCount = entry;
}

void moveRightUsingEligibility() {
  CardPosition pos;

  if (eligPos < eligCount) {
    eligPos++;
  }

  pos = eligibileCards[eligPos];
  if (eligPos == 0 || (pos.row > 0 && pos.card > 0)) {
    cursorRow = pos.row;
    cursorCard = pos.card;
  }
}

void moveLeftUsingEligibility() {
  CardPosition pos;

  if (eligPos > 0) {
    eligPos--;
  }

  pos = eligibileCards[eligPos];
  cursorRow = pos.row;
  cursorCard = pos.card;
}

void moveBackDiscard() {
  bool searching = true;

  while (searching) {
    if (discardPtr == DISCARD_BEGINS) {
      searching = false;
      break;
    }

    if (deck[discardPtr - 1] > 0) {
      searching = false;
    }
    discardPtr--;
  }
}

void advanceNextDiscard() {
  bool searching = true;

  while (searching) {
    if (deckflips == 3) {
      searching = false;
      break;
      // Game over
    }
    if (discardPtr + 1 == 51) {
      discardPtr = DISCARD_BEGINS;
      deckflips++;
    }
    if (deck[discardPtr + 1] > 0) {
      searching = false;
    }
    discardPtr++;
  }
}

void iterateBoard(void (*execute)(char row, char col, char search),
                  char search) {
  char rows, cols;

  for (rows = 0; rows < 7; rows++) {
    for (cols = 0; cols < 7; cols++) {
      execute(rows, cols, search);
    }
  }
}

void zeroRecord(char row, char col, char search) { select[row][col] = 0; }

void removeCard(char row, char col, char search) {
  if (board[row][col] == search) {
    board[row][col] = 0;
  }
}

void canSelect(char row, char col, char search) {
  if (row == 7 && board[row][col] > 0) {
    select[row][col] |= 2;
  } else {
    select[row][col] |= 2;
  }
}

void resetSelections() {
  iterateBoard(zeroRecord, 0);
  numSelections = 0;
  flipIsSelected = false;
}

void removeCardFromBoard(char cardNo) {
  if (cardNo > 0) {
    iterateBoard(removeCard, cardNo);
  }
}

void determineSelectability() { iterateBoard(canSelect, 0); }

void loadPyramidBoard() {
  char r, c, num, counter;
  num = 0;
  counter = 1;
  for (r = 0; r < 7; r++) {
    for (c = 0; c < counter; c++) {
      board[r][c] = deck[num];
      num++;
    }
    counter++;
  }
  remainingCardCount = num;
  discardPtr = DISCARD_BEGINS;
}

void loadPyramidDeck(char *deck[]) {
  char i = 0;
  for (i = 1; i < 52; i++) {
    deck[i - 1] = i;
  }
}

void shufflePyramidDeck(char *cards[], char times) {
  char valueToSwap;
  char indexToSwap;
  char i, t;

  for (t = 0; t < times; t++) {
    for (i = 0; i < 51; i++) {
      indexToSwap = rnd_range(1, 50);
      valueToSwap = cards[indexToSwap];
      cards[indexToSwap] = cards[1];
      cards[1] = valueToSwap;
    }
  }
}

int getvalue(char cardno) {
  char value = cardno % 13;
  if (value == 0) {
    value = 13;
  }
  return value;
}

void initializePyramidScene() {
  // background =
  // allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
  background = allocate_sprite(&ASSET__backgrounds__background2_bmp_load_list);
  testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
  remainingCards = allocate_sprite(&ASSET__numbers__generalnums_bmp_load_list);
  set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
  set_sprite_frametable(background, &ASSET__backgrounds__background1_json);
  set_sprite_frametable(remainingCards, &ASSET__numbers__generalnums_json);
  loadPyramidDeck(&deck);
  shufflePyramidDeck(&deck, RANDOM_SEED);
  loadPyramidBoard();
  resetSelections();
  determineSelectability();
  scanForEligibility();
}

void checkSelection() {
  char cardOne, cardTwo, rows, cols, value1, value2, total;
  cardOne = 0;
  cardTwo = 0;
  value1 = 0;
  value2 = 0;
  total = 0;
  for (rows = 0; rows < 7; rows++) {
    for (cols = 0; cols < 7; cols++) {
      if (select[rows][cols] == 1) {
        if (cardOne == 0) {
          cardOne = board[rows][cols];
          value1 = getvalue(cardOne + 1);
        } else {
          cardTwo = board[rows][cols];
          value2 = getvalue(cardTwo + 1);
        }
      }
    }
  }

  total = value1 + value2;
  if (flipIsSelected) {
    total = value1 + getvalue(deck[discardPtr] + 1);
  }

  if (13 == total) {
    if (cardOne != 0) {
      removeCardFromBoard(cardOne);
      remainingCardCount--;
    }
    if (cardTwo != 0) {
      removeCardFromBoard(cardTwo);
      remainingCardCount--;
    }
    if (flipIsSelected) {
      deck[discardPtr] = 0;
      moveBackDiscard();
    }
    resetSelections();
    determineSelectability();
    scanForEligibility();
    if (isOnBoard) {
      moveLeftUsingEligibility();
    }
  }
}

void checkInput() {
  if (player1_new_buttons & INPUT_MASK_RIGHT) {
    moveRightUsingEligibility();
  } else if (player1_new_buttons & INPUT_MASK_LEFT) {
    // moveLeftRedux();
    moveLeftUsingEligibility();
  } else if (player1_new_buttons & INPUT_MASK_DOWN) {
    isOnBoard = false;
    isOnDiscard = true;
  } else if (player1_new_buttons & INPUT_MASK_UP) {
    isOnBoard = true;
    isOnDiscard = false;
    cursorCard = 0;
    moveLeftUsingEligibility();
  } else if (player1_new_buttons & INPUT_MASK_B) {
    resetSelections();
  } else if (player1_new_buttons & INPUT_MASK_A) {
    if (numSelections < 2) {
      if (isOnDiscard) {
        flipIsSelected = true;
        numSelections++;
      } else {

        select[cursorRow][cursorCard] = 1;
        numSelections++;
      }
    } else {
      resetSelections();

      if (isOnDiscard) {
        flipIsSelected = true;
        numSelections++;
      } else {
        select[cursorRow][cursorCard] = 1;
        numSelections++;
      }
    }
    checkSelection();
  } else if (player1_new_buttons & INPUT_MASK_C) {
    advanceNextDiscard();
  }
}

void checkInputFromWin() {
  if (player1_new_buttons & INPUT_MASK_A) {
    initializePyramidScene();
  }
}

void renderPyramidBoardNormal() {
  char rows, cols, cardx, cardy, counter;

  counter = 1;
  cardx = 64;

  queue_draw_sprite(0, 0, 127, 127, 0, 0, background);

  for (rows = 0; rows < 7; rows++) {
    cardy = divy[rows];

    if (rows != 0) {
      cardy -= rows * 4;
      cardx = 64 - ((16 * rows) / 2);
    }

    for (cols = 0; cols < counter; cols++) {

      if (board[rows][cols] != 0) {
        // If the cursor is currently over this
        if (cursorRow == rows && cursorCard == cols && isOnBoard) {
          queue_draw_box(cardx - 8, cardy - 8, 16, 16, rnd_range(0, 255));
        }
        // If the card we are is has a selection flag on it
        if (select[rows][cols] & 1) {
          queue_draw_sprite_frame(testSlot, cardx, cardy + SELECT_SHIFT,
                                  board[rows][cols], false);
        } else {
          queue_draw_sprite_frame(testSlot, cardx, cardy, board[rows][cols],
                                  false);
        }
      }
      cardx += 16;
    }
    counter++;
  }

  if (!isOnBoard && isOnDiscard) {
    queue_draw_box(FLIPPED_PILE_X - 8, FLIPPED_PILE_Y - 8, 16, 16,
                   rnd_range(0, 255));
  }

  queue_draw_sprite_frame(testSlot, DISCARD_PILE_X, DISCARD_PILE_Y, 0, false);
  if (discardPtr > DISCARD_BEGINS) {
    if (flipIsSelected) {
      queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X + SELECT_SHIFT,
                              FLIPPED_PILE_Y, deck[discardPtr], false);
    } else {
      queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X, FLIPPED_PILE_Y,
                              deck[discardPtr], false);
    }
  } else {
    queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X, FLIPPED_PILE_Y, 0, false);
  }

  queue_draw_sprite_frame(remainingCards, SCORE_X, SCORE_Y, remainingCardCount,
                          false);

  checkInput();
}

void renderWinSequence() {
  char deckpos;
  queue_clear_screen(rnd_range(0, 255));
  for (deckpos = 0; deckpos < 52; deckpos++) {
    queue_draw_sprite_frame(testSlot, rnd_range(8, 120), rnd_range(8, 120),
                            deckpos, false);
  }
  checkInputFromWin();
}

void renderPyramidBoard() {
  if (hasWon()) {
    renderWinSequence();
  } else {
    renderPyramidBoardNormal();
  }
}

#endif