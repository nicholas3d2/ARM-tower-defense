/* Description: main.c file for tower defense game, developed for CPULATOR
 */
//#include "main.h"

/************main.h************/
#define SDRAM_BASE 0xC0000000
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_CHAR_BASE 0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define TIMER_BASE 0xFF202000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define JTAG_UART_BASE 0xFF201000

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define GRID_LEN 20
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// function prototypes
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void clear_screen();
void wait_for_vsync();
void draw_box(int x, int y, short int line_colour);
void draw_turrent_diamon(int x, int y, short int line_colour);

char get_jtag(volatile int *JTAG_UART_ptr);
void put_jtag(volatile int *JTAG_UART_ptr, char c);

volatile int pixel_buffer_start; // global variable
// location of user's grid box
int xcurrent = 0;
int ycurrent = 0;

// location of user's grid one frame before
int xprev1 = 0;
int yprev1 = 0;

// location of the user's grid two frames before
int xprev2 = 0;
int yprev2 = 0;

/************main.h************/

int main(void) {
  volatile int *JTAG_UART_ptr = (int *)0xFF201000; // JTAG UART address
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

  /* set front pixel buffer to start of FPGA On-chip memory */
  *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                      // back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();
  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  clear_screen(); // pixel_buffer_start points to the pixel buffer
  /* set back pixel buffer to start of SDRAM memory */
  *(pixel_ctrl_ptr + 1) = 0xC0000000;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  wait_for_vsync();
  clear_screen();

  // draw grid box (user controlled grid box)

  // TEST JTAG UART
  char text_string[] = "\nJTAG UART test\n> \0";
  char *str, c;

  for (str = text_string; *str != 0; ++str) {
    put_jtag(JTAG_UART_ptr, *str);
  }

  // Main program loop, read user inputs while running
  while (1) {
    // clear 2 frames before
    draw_turrent_diamon(xprev2, yprev2, 0);
    draw_grid_box(xprev2, yprev2, 0);

    // draw
    draw_grid_box(xcurrent, ycurrent, WHITE);
    draw_turrent_diamon(xcurrent, ycurrent, WHITE);
    // update position
    xprev2 = xprev1;
    yprev2 = yprev1;

    xprev1 = xcurrent;
    yprev1 = ycurrent;

    c = get_jtag(JTAG_UART_ptr);
    if (c != '\0') {
      put_jtag(JTAG_UART_ptr, c);
      if (c == 'w') {
        move_box_y(-GRID_LEN); // move up
      } else if (c == 'a') {
        move_box_x(-GRID_LEN); // move left
      } else if (c == 's') {
        move_box_y(GRID_LEN); // move down
      } else if (c == 'd') {
        move_box_x(GRID_LEN); // move left
      }
    }

    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  }
}

// moves a 20x20 box around on the screen
void move_box_x(int direction) {
  int nextX = direction + xcurrent;
  xcurrent = (nextX >= RESOLUTION_X || nextX < 0) ? xcurrent : nextX;
}

void move_box_y(int direction) {
  int nextY = direction + ycurrent;
  ycurrent = (nextY >= RESOLUTION_Y || nextY < 0) ? ycurrent : nextY;
}

void draw_grid_box(int x0, int y0, short int colour) {
  draw_line(x0, y0, x0 + (GRID_LEN - 1), y0, colour);
  draw_line(x0, y0, x0, y0 + (GRID_LEN - 1), colour);
  draw_line(x0 + (GRID_LEN - 1), y0 + (GRID_LEN - 1), x0 + (GRID_LEN - 1), y0,
            colour);
  draw_line(x0 + (GRID_LEN - 1), y0 + (GRID_LEN - 1), x0, y0 + (GRID_LEN - 1),
            colour);
}

// draws a pixel given its location and colour
void plot_pixel(int x, int y, short int line_color) {
  if ((y > RESOLUTION_Y - 1 || y < 0) || (x > RESOLUTION_X - 1 || x < 0))
    return;
  *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

char get_jtag(volatile int *JTAG_UART_ptr) {
  int data;
  data = *(JTAG_UART_ptr);
  if (data & 0x00008000) // check RVALID
    return ((char)data & 0xFF);
  else
    return ('\0');
}

void put_jtag(volatile int *JTAG_UART_ptr, char c) {
  int control;
  control = *(JTAG_UART_ptr + 1); // read control reg
  if (control & 0xFFFF0000) {
    *(JTAG_UART_ptr) = c;
  }
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

// draws the diamond for a 20x20 box
void draw_turrent_diamond(int x, int y, short int line_colour) {
  draw_line(x + 9, y + 2, x + 2, y + 9, line_colour); // top left edges
  draw_line(x + 9, y + 3, x + 3, y + 9, line_colour);
  draw_line(x + 10, y + 2, x + 17, y + 9, line_colour); // top right edges
  draw_line(x + 10, y + 3, x + 16, y + 9, line_colour);
  draw_line(x + 2, y + 10, x + 9, y + 17, line_colour); // bottom left edges
  draw_line(x + 3, y + 10, x + 9, y + 16, line_colour);
  draw_line(x + 10, y + 16, x + 16, y + 10, line_colour); // bottom right edges
  draw_line(x + 10, y + 17, x + 17, y + 10, line_colour);
}
