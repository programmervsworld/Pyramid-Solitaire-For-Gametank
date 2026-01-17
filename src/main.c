#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "../gen/assets/cardframes.h"

char box_x = 30, box_y = 20;
char dx = 1, dy = 1;

SpriteSlot testSlot;
int frame = 1;

void incrementFrame(){
    if(++frame == 52){
        frame = 0;
    }
}


void main () {
    testSlot = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(testSlot, &ASSET__cardframes__frame_deck_json);

    while (1) {                                     //  Run forever
        queue_clear_screen(3);
        //queue_draw_box(box_x, box_y, 8, 8, 92);
        //queue_draw_sprite(box_x, box_y, 16, 16, 16, 0, testSlot);
        queue_draw_sprite_frame(testSlot, box_x, box_y, frame, false);
        queue_clear_border(0);
        
        box_x += dx;
        box_y += dy;
        if(box_x == 1) {
            dx = 1;
            incrementFrame();
        } else if(box_x == 119) {
            dx = -1;
            incrementFrame();
        }
        if(box_y == 8) {
            dy = 1;
            incrementFrame();
        } else if(box_y == 112) {
            dy = -1;
            incrementFrame();
        }
 
        await_draw_queue();
        await_vsync(1
        );
        flip_pages();
 
    }
}