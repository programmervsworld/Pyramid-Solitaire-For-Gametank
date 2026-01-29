
#include "gt/audio/audio_coprocessor.h"
#include "gt/audio/music.h"
#include "gt/feature/random/random.h"
#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "intro.h"
#include "pyramid.h"

#define PIXEL_WIDTH 128
#define PIXEL_HEIGHT 128
#define CARD_HEIGHT 16
#define CARD_WIDTH 16

void main() {

  char seed = renderIntroScreen();
  rnd_seed = seed;

  initializePyramidScene();

  init_audio_coprocessor();
  init_music();

  //  Run forever
  while (1) {
    queue_clear_screen(3);
    queue_clear_border(0);

    renderPyramidBoard();

    await_draw_queue();

    await_vsync(1);

    flip_pages();
    tick_music();
    update_inputs();
  }
}