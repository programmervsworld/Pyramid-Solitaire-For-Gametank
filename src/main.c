#include "../gen/assets/cardframes.h"
#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "pyramid.h"

#define PIXEL_WIDTH 128
#define PIXEL_HEIGHT 128
#define CARD_HEIGHT 16
#define CARD_WIDTH 16

typedef enum { INTRO, PYRAMID } SceneType;

typedef struct {
  void (*initialize)();
  void (*render)();
} Scene;

Scene pyramid = {&initializePyramidScene, &renderPyramidBoard};

Scene scenes[2];
int currentScene = PYRAMID;

void main() {
  scenes[1] = pyramid;

  scenes[currentScene].initialize();

  //  Run forever
  while (1) {
    queue_clear_screen(3);
    queue_clear_border(0);

    scenes[currentScene].render();

    await_draw_queue();
    await_vsync(1);
    flip_pages();
    update_inputs();
  }
}