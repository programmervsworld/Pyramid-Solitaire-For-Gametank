#ifndef INTRO_H
#define INTRO_H

#include "gametank.h"
#include "../gen/assets/backgrounds.h"
#include "input.h"

bool isOnIntro = true;
SpriteSlot background;

void initializeIntroScreen() {
  background = allocate_sprite(&ASSET__backgrounds__splash_bmp_load_list);
}

void checkIntroInput() {
  if (player1_new_buttons & INPUT_MASK_START) {
    isOnIntro = false;
  }
}

char renderIntroScreen() {
  char seed = 0;
  initializeIntroScreen();
  while (isOnIntro) {
    queue_clear_border(0);
    queue_clear_screen(0);
    queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
    ++seed;
    await_draw_queue();

    await_vsync(1);

    flip_pages();
    tick_music();
    update_inputs();

    checkIntroInput();
  }
  return seed;
}

#endif