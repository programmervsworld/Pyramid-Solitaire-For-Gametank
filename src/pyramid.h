#ifndef CARDS_H
#define CARDS_H

#include "../gen/assets/backgrounds.h"
#include "../gen/assets/cardframes.h"
#include "../gen/assets/numbers.h"
#include "../gen/assets/sfx.h"
#include "gametank.h"
#include "gt/feature/random/random.h"
#include "gt/gfx/draw_queue.h"
#include "input.h"

#define DISCARD_PILE_X 48
#define DISCARD_PILE_Y 104
#define FLIPPED_PILE_X 80
#define FLIPPED_PILE_Y 104
#define DISCARD_BEGINS 27
#define SHUFFLE_SEED 9
#define SELECT_SHIFT 5
#define SCORE_X 103
#define SCORE_Y 17
#define MAX_DECK_FLIPS 3

typedef struct {
  char row;
  char card;
} CardPosition;

// These two arrays just hold the cards positions in the tableau grid.
static const char divy[] = {12, 28, 44, 60, 76, 92, 108};

// This is the playfield
char board[7][7];
char select[7][7];
CardPosition eligibileCards[7];
char animationx[52];
char animationy[52];
char directionx[52];
char directiony[52];

// The is the deck that we deal from and where the next discard will come from
unsigned char deck[52];
char discardPtr = 0;
char cursorRow = 6;
char cursorCard = 0;
char eligPos = 0;
char eligCount = 7;
char numSelections = 0;
char deckflips = 0;
char remainingCardCount = 0;
char wxpos = 64, wypos = 64;

bool isOnBoard = true;
bool isOnDiscard = false;
bool flipIsSelected = false;

SpriteSlot background;
SpriteSlot winBack;
SpriteSlot testSlot;
SpriteSlot remainingCards;

bool hasWon() { return remainingCardCount == 0; }
bool hasLost() { return deckflips == MAX_DECK_FLIPS; }

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
    if (deckflips == MAX_DECK_FLIPS) {
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

void loadPyramidDeck(unsigned char *deck) {
  char i = 0;
  for (i = 1; i < 52; i++) {
    deck[i - 1] = i;
    animationx[i - 1] = 64;
    animationy[i - 1] = 64;
    directionx[i - 1] = rnd_range(-8, 8);
    directiony[i - 1] = rnd_range(-8, 8);
  }
}

void shufflePyramidDeck(unsigned char *cards, unsigned char times) {
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
  background = allocate_sprite(&ASSET__backgrounds__background2_bmp_load_list);
  testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
  remainingCards = allocate_sprite(&ASSET__numbers__generalnums_bmp_load_list);
  winBack = allocate_sprite(&ASSET__backgrounds__winback_bmp_load_list);
  set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
  set_sprite_frametable(remainingCards, &ASSET__numbers__generalnums_json);
  remainingCardCount = 0;
  deckflips = 0;

  loadPyramidDeck(deck);
  //rnd_seed = 234;
  shufflePyramidDeck(deck, SHUFFLE_SEED);
  loadPyramidBoard();
  resetSelections();
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
    play_sound_effect(ASSET__sfx__accept_sfx_ID, 1);
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
    // determineSelectability();
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
    play_sound_effect(ASSET__sfx__flip_sfx_ID, 1);
    resetSelections();
  } else if (player1_new_buttons & INPUT_MASK_A) {
    if (numSelections < 2) {
      if (isOnDiscard) {
        flipIsSelected = true;
        play_sound_effect(ASSET__sfx__blip2_sfx_ID, 1);
        numSelections++;
      } else {
        play_sound_effect(ASSET__sfx__blip2_sfx_ID, 1);
        select[cursorRow][cursorCard] = 1;
        numSelections++;
      }
    } else {
      resetSelections();

      if (isOnDiscard) {
        flipIsSelected = true;
        play_sound_effect(ASSET__sfx__blip2_sfx_ID, 2);

        numSelections++;
      } else {
        play_sound_effect(ASSET__sfx__blip2_sfx_ID, 1);
        select[cursorRow][cursorCard] = 1;
        numSelections++;
      }
    }
    checkSelection();
  } else if (player1_new_buttons & INPUT_MASK_C) {
    play_sound_effect(ASSET__sfx__blip_sfx_ID, 1);
    advanceNextDiscard();
  }
}

void checkInputFromWin() {
  if (player1_new_buttons & INPUT_MASK_START) {
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
  // queue_clear_screen(rnd_range(0, 255));
  queue_draw_sprite(0, 0, 127, 127, 0, 0, winBack);

  for (deckpos = 0; deckpos < 52; deckpos++) {
    if (directionx[deckpos] == 0)
      directionx[deckpos] = 1;
    if (directiony[deckpos] == 0)
      directiony[deckpos] = 1;
    animationx[deckpos] += directionx[deckpos];
    animationy[deckpos] += directiony[deckpos];

    if (animationx[deckpos] == 8) {
      directionx[deckpos] = 1;
    } else if (animationx[deckpos] == 120) {
      directionx[deckpos] = -1;
    }

    if (animationy[deckpos] == 8) {
      directiony[deckpos] = 1;
    } else if (animationy[deckpos] == 112) {
      directiony[deckpos] = -1;
    }

    queue_draw_sprite_frame(testSlot, animationx[deckpos], animationy[deckpos],
                            deckpos, false);
    // queue_draw_sprite_frame(testSlot, rnd_range(8, 110), rnd_range(8, 110),
    // deckpos, false);
  }
  checkInputFromWin();
}

void renderPyramidBoard() {
  bool isRunning = true;
  initializePyramidScene();

  while (isRunning) {
    queue_clear_screen(0);
    queue_clear_border(0);

    if (hasWon()) {
      renderWinSequence();
    } else if (hasLost()) {
      isRunning = false;
    } else {
      renderPyramidBoardNormal();
    }

    await_draw_queue();

    await_vsync(1);

    flip_pages();
    tick_music();
    update_inputs();
  }
}

#endif
