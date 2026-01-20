#ifndef CARDS_H
#define CARDS_H

#include "../gen/assets/backgrounds.h"
#include "gt/feature/random/random.h"
#include "gt/feature/text/text.h"
#include "input.h"

#define DISCARD_PILE_X 48
#define DISCARD_PILE_Y 112
#define FLIPPED_PILE_X 80
#define FLIPPED_PILE_Y 112
#define CHECK_BIT(var, pos) ((var >> pos) & 1)

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
int numSelections = 0;
int deckflips = 0;

bool isOnBoard = true;
bool isOnDiscard = true;

SpriteSlot background;
SpriteSlot testSlot;
SpriteSlot textSlot;

char *selectionText = "Selected Card 1: ";

void breakpoint() {}

void moveRight() {
  bool searching = true;
  if (isOnBoard) {

    while (searching) {

      cursorCard++;

      if (cursorCard > 6) {
        cursorCard = 0;
      }

      if (cursorRow < 6 && board[cursorRow + 1][cursorCard] > 0) {
        cursorRow++;
      }

      if(cursorRow < 6 && board[cursorRow + 1][cursorCard + 1] > 0){
        cursorRow++;
        cursorCard++;
        searching = false;
      }

      if (board[cursorRow][cursorCard] > 0) {
        searching = false;
      } else {
        if (board[cursorRow][cursorCard + 1] == 0) {
          cursorRow--;
          searching = false;
        }
      }
    }
  } else {
    isOnDiscard = true;
  }
}

void moveLeft() {
  bool searching = true;
  if (isOnBoard) {
    while (searching) {
      if (cursorCard == 0) {
        cursorCard = 7;
      } else {
        cursorCard--;
      }

      if (cursorRow < 6 && board[cursorRow + 1][cursorCard] > 0) {
        cursorRow++;
      }

      if (board[cursorRow][cursorCard] > 0) {
        searching = false;
      } else {
        if (board[cursorRow][cursorCard - 1] == 0 && board[cursorRow][cursorCard - 2] == 0) {
          cursorRow--;
          searching = false;
        }
      }
    }
  } else {
    isOnDiscard = true;
  }
}

void advanceNextDiscard(){
  bool searching = true;
  
  while(searching){
    if(deckflips == 3){
      searching = false;
      break;
      //Game over
    }
    if(discardPtr + 1 == 51){
      discardPtr = 27;
      deckflips++;
    }
    if(deck[discardPtr] > 0){
      searching = false;
    }
    discardPtr++;
  }
}

void iterateBoard(void (*execute)(int row, int col, int search), int search) {
  int rows, cols;

  for (rows = 0; rows < 7; rows++) {
    for (cols = 0; cols < 7; cols++) {
      execute(rows, cols, search);
    }
  }
}

void zeroRecord(int row, int col, int search) { select[row][col] = 0; }

void removeCard(int row, int col, int search) {
  if (board[row][col] == search) {
    board[row][col] = 0;
  }
}

void canSelect(int row, int col, int search) {
  if (row == 7 && board[row][col] > 0) {
    select[row][col] |= 2;
  } else {
    select[row][col] |= 2;
  }
}

void resetSelections() {
  iterateBoard(zeroRecord, 0);
  numSelections = 0;
}

void removeCardFromBoard(int cardNo) { iterateBoard(removeCard, cardNo); }

void determineSelectability() { iterateBoard(canSelect, 0); }

void loadPyramidBoard() {
  int r, c, num, counter;
  num = 0;
  counter = 1;
  for (r = 0; r < 7; r++) {
    for (c = 0; c < counter; c++) {
      board[r][c] = deck[num];
      num++;
      discardPtr = num;
    }
    counter++;
  }
}

void loadPyramidDeck(int deck[]) {
  int i = 0;
  for (i = 1; i < 52; i++) {
    deck[i - 1] = i;
  }
}

void shufflePyramidDeck(int cards[], int times) {
  int valueToSwap;
  int indexToSwap;
  int i, t;

  for (t = 0; t < times; t++) {
    for (i = 0; i < 51; i++) {
      indexToSwap = rnd_range(1, 50);
      valueToSwap = cards[indexToSwap];
      cards[indexToSwap] = cards[1];
      cards[1] = valueToSwap;
    }
  }
}

void draw_selection() {
  text_init();
  text_cursor_x = 1;
  text_cursor_y = 7;
  text_color = TEXT_COLOR_BLACK;
  text_print_string(selectionText);
}

int getvalue(int cardno) {
  int value = cardno % 13;
  if (value == 0) {
    value = cardno;
  }
  return value;
}

void initializePyramidScene() {
  background = allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
  testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
  set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);
  set_sprite_frametable(background, &ASSET__backgrounds__background1_json);
  loadPyramidDeck(deck);
  shufflePyramidDeck(deck, 7);
  loadPyramidBoard();
  resetSelections();
  determineSelectability();

  text_init();
  text_load_font();
  text_color = TEXT_COLOR_BLACK;
  text_print_line_start = 10;
  text_print_width = 128;
}

void checkSelection() {
  int cardOne, cardTwo, rows, cols, value1, value2;
  cardOne = 0;
  cardTwo = 0;
  value1 = 0;
  value2 = 0;
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
        // select[rows][cols] = 0;
      }
    }
  }

  if (value1 == 13) {
    removeCardFromBoard(cardOne);
    resetSelections();
    determineSelectability();
    if (isOnBoard) {
      moveRight();
    }

  } else if (13 == (value1 + value2)) {
    removeCardFromBoard(cardOne);
    removeCardFromBoard(cardTwo);
    resetSelections();
    determineSelectability();
    if (isOnBoard) {
      moveRight();
    }
  }
}

void checkInput() {
  if (player1_new_buttons & INPUT_MASK_RIGHT) {
    moveRight();
  } else if (player1_new_buttons & INPUT_MASK_LEFT) {
    moveLeft();
  } else if (player1_new_buttons & INPUT_MASK_DOWN) {
    isOnBoard = false;
    isOnDiscard = true;
  } else if (player1_new_buttons & INPUT_MASK_UP) {
    isOnBoard = true;
    isOnDiscard = false;
  } else if (player1_new_buttons & INPUT_MASK_B) {
    resetSelections();
  } else if (player1_new_buttons & INPUT_MASK_A) {
    if (numSelections < 2) {
      select[cursorRow][cursorCard] = 1;
      numSelections++;
    } else {
      resetSelections();

      select[cursorRow][cursorCard] = 1;
      numSelections++;
    }
    checkSelection();
  }else if(player1_new_buttons & INPUT_MASK_C){
    advanceNextDiscard();
  }
}

void renderPyramidBoard() {
  int rows, cols, cardx, cardy, counter;

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
          queue_draw_sprite_frame(testSlot, cardx, cardy + 8, board[rows][cols],
                                  false);
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
  if(discardPtr > 24){
    queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X, FLIPPED_PILE_Y, deck[discardPtr], false);
  }else{
    queue_draw_sprite_frame(testSlot, FLIPPED_PILE_X, FLIPPED_PILE_Y, 0, false);
  }
  
  

  checkInput();

}

#endif