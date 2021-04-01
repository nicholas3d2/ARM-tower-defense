/* Description: main.c file for tower defense game
 */

#include "main.h"

int main(void) {}

// draws a pixel given its location and colour
void plot_pixel(int x, int y, short int line_color) {
  *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// draws a line given two points and colour
void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
  bool isSteep = ABS(y1 - y0) > ABS(x1 - x0);
  if (isSteep) {
    int temp = x0; // swap(x0, y0)
    x0 = y0;
    y0 = temp;

    temp = x1; // swap(x1, y1)
    x1 = y1;
    y1 = temp;
  }
  if (x0 > x1) {
    // swap x0 x1
    int temp = x0;
    x0 = x1;
    x1 = temp;
    // swap y0 y1
    temp = y0;
    y0 = y1;
    y1 = temp;
  }
  int deltaX = x1 - x0;
  int deltaY = ABS(y1 - y0);
  int error = -(deltaX / 2);
  int y = y0;
  int y_step = y0 < y1 ? 1 : -1;
  for (int x = x0; x <= x1; x++) {
    if (isSteep)
      plot_pixel(y, x, line_color);
    else
      plot_pixel(x, y, line_color);
    error = error + deltaY;
    if (error >= 0) {
      y = y + y_step;
      error = error - deltaX;
    }
  }
}
void clear_screen() {
  for (int x = 0; x < RESOLUTION_X; x++) {
    for (int y = 0; y < RESOLUTION_Y; y++)
      plot_pixel(x, y, 0x0); // draws black;
  }
}
void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xff203020;
  register int status;

  *pixel_ctrl_ptr = 1; // start synchronization process

  // wait for s = 0
  status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01) != 0) {
    status = *(pixel_ctrl_ptr + 3);
  }
}

void draw_box(int x, int y, short int line_colour) {
  for (int x0 = x; x0 <= x + BOX_LEN; x0++) {
    draw_line(x0, y, x0, y + BOX_LEN, line_colour);
  }
}