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
#define GRID_WIDTH 16
#define GRID_HEIGHT 12
#define BOX_LEN 2
#define NUM_BOXES 8

#define PATH_COLOUR 0x07E0
#define FALSE 0
#define TRUE 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// function prototypes
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void clear_screen();
void wait_for_vsync();
void draw_box(int x, int y, short int line_colour);
void draw_turret_diamond(int x, int y, short int line_colour);
void draw_turret_light(int x, int y, short int colour);
void draw_turret_medium(int x, int y, short int colour);
void draw_turret_heavy(int x, int y, short int colour);
//path drawing function prototypes
void draw_path_horizontal(int x, int y, short int colour);
void draw_path_vertical(int x, int y, short int colour);
// <Vert Direction>_<Horizontal Direction> (of opening)
void draw_path_down_right(int x, int y, short int colour);
void draw_path_down_left(int x, int y, short int colour);
void draw_path_up_right(int x, int y, short int colour);
void draw_path_up_left(int x, int y, short int colour);

// health related functions
void updateHealthToLEDR();
void loseHealth();
//draw Grid
void draw_grid();
void clear_grid();
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

// Health
int health = 10;
// Grid Elements
typedef enum{
  Empty,
  Light,				//Towers
  Medium,
  Heavy,
  //Path
  Path_Horizontal_Left,  // <--- enemy moves left
  Path_Horizontal_Right, // --->	enemy moves right
  Path_Vertical_Up,      //	 |^ enemy moves up   the grid
  Path_Vertical_Down,    //  |_	enemy moves down the grid
  Path_Right_Down,       // --> moves right, then down
                         //  |
                         // \/
  Path_Right_Up,         //    ^ moves right then up
                         // -->|
  Path_Down_Right,
  Path_Down_Left,
  Path_Left_Up,
  Path_Left_Down,
  Path_Up_Right,
  Path_Up_Left
}GridElements;

GridElements Grid[12][16] = 
{
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Path_Up_Right, Path_Horizontal_Right , Path_Horizontal_Right, Path_Horizontal_Right , Path_Right_Down, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Path_Vertical_Up, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Path_Vertical_Up, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Path_Vertical_Up, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Path_Vertical_Down , Empty, Empty , Empty, Empty , Empty,Empty , Path_Vertical_Up, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Path_Down_Right , Path_Horizontal_Right, Path_Horizontal_Right , Path_Horizontal_Right, Path_Horizontal_Right , Path_Horizontal_Right ,Path_Horizontal_Right , Path_Right_Up, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty},
	{Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Path_Vertical_Down, Empty , Empty}
};
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

	// initialize health
	updateHealthToLEDR();
	// Main program loop, read user inputs while running
	while (1) {
		// clear 2 frames before
		draw_turret_light(xprev2, yprev2, 0);
		//draw_turret_diamond(xprev2, yprev2, 0);
		draw_grid_box(xprev2, yprev2, 0);

		// draw
		draw_grid();
		draw_grid_box(xcurrent, ycurrent, WHITE);
		//draw_turret_diamond(xcurrent, ycurrent, WHITE);
		draw_turret_light(xcurrent, ycurrent, WHITE);
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
			} else if (c == 'e')
				loseHealth();
		}

		wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

		// game over if no health left, exits the program
		if(health == 0){	
			char game_over[] = "\nGame Over\n> \0";
			for (str = game_over; *str != 0; ++str) {
				put_jtag(JTAG_UART_ptr, *str);
			}
			break;
		}
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
void draw_turret_diamond(int x, int y, short int line_colour) {
  draw_line(x + 9, y + 2, x + 2, y + 9, line_colour); // top left edges
  draw_line(x + 9, y + 3, x + 3, y + 9, line_colour);
  draw_line(x + 10, y + 2, x + 17, y + 9, line_colour); // top right edges
  draw_line(x + 10, y + 3, x + 16, y + 9, line_colour);
  draw_line(x + 2, y + 10, x + 9, y + 17, line_colour); // bottom left edges
  draw_line(x + 3, y + 10, x + 9, y + 16, line_colour);
  draw_line(x + 10, y + 16, x + 16, y + 10, line_colour); // bottom right edges
  draw_line(x + 10, y + 17, x + 17, y + 10, line_colour);
}


void draw_turret_light(int x, int y, short int colour){
  for(int i = x+5; i < x+15; i++){
    for(int j = y+3; j < y+18; j++){
      plot_pixel(i,j,colour);
    }
  }
  plot_pixel(x+5, y+3, 0);
  plot_pixel(x+6, y+3, 0);
  plot_pixel(x+7, y+3, 0);
  plot_pixel(x+12, y+3, 0);
  plot_pixel(x+13, y+3, 0);
  plot_pixel(x+14, y+3, 0);

  plot_pixel(x+5, y+4, 0);
  plot_pixel(x+6, y+4, 0);
  plot_pixel(x+13, y+4, 0);
  plot_pixel(x+14, y+4, 0);

  plot_pixel(x+5, y+5, 0);
  plot_pixel(x+14, y+5, 0);

  for(int i = x+7; i < x+13; i++){
    for(int j = y+8; j < y+11; j++){
      plot_pixel(i, j, 0);
    }
  }

  plot_pixel(x+5, y+8, 0);
  plot_pixel(x+5, y+9, 0);
  plot_pixel(x+5, y+10, 0);

  plot_pixel(x+14, y+8, 0);
  plot_pixel(x+14, y+9, 0);
  plot_pixel(x+14, y+10, 0);

}


void draw_turret_medium(int x, int y, short int colour){
  for(int i = x+3; i < x+17; i++){
    for(int j = y + 2; j < y+18; j++){
      plot_pixel(i,j,colour);
    }
  }
  plot_pixel(x+7, y+2, 0);
  plot_pixel(x+7, y+3, 0);
  plot_pixel(x+12, y+2, 0);
  plot_pixel(x+12, y+3, 0);

  for(int i = x+3; i < x+6; i++){
    for(int j = y+6; j < y+14; j++){
      plot_pixel(i,j,0);  
    }
  }

  plot_pixel(x+5, y+6, colour);
  plot_pixel(x+5, y+11, colour);
  plot_pixel(x+5, y+12, colour);
  plot_pixel(x+5, y+13, colour);

  for(int i = x+14; i < x+17; i++){
    for(int j = y+6; j < y+14; j++){
      plot_pixel(i,j,0);  
    }
  }

  plot_pixel(x+14, y+6, colour);
  plot_pixel(x+14, y+11, colour);
  plot_pixel(x+14, y+12, colour);
  plot_pixel(x+14, y+13, colour);

}

void draw_turret_heavy(int x, int y, short int colour){

}

// drawing path
void draw_path_horizontal(int x, int y, short int colour){
	draw_line(x, y+3, x+19, y+3, colour);
	draw_line(x, y+16, x+19, y+16, colour);
	plot_pixel(x + 6, y + 6, YELLOW);
	plot_pixel(x+15, y+9, YELLOW);
	plot_pixel(x+3, y+3, YELLOW);
}

void draw_path_vertical(int x, int y, short int colour){
	draw_line(x+3, y, x+3, y+19, colour);
	draw_line(x+16, y, x+16, y+19, colour);
	plot_pixel(x+6, y+3, YELLOW);
	plot_pixel(x+12, y+7, YELLOW);
	plot_pixel(x+8, y+13, YELLOW);
	plot_pixel(x+13, y+18, YELLOW);
}
void draw_path_down_right(int x, int y, short int colour){
	draw_line(x+3, y+3, x+19, y+3, colour);
	draw_line(x+3, y+4, x+3, y+19, colour);
	draw_line(x+16, y+16, x+19, y+16, colour);
	draw_line(x+16, y+17, x+16, y+19, colour);
	plot_pixel(x+13, y+7, YELLOW);
	plot_pixel(x+8, y+12, YELLOW);
	plot_pixel(x+13, y+18, YELLOW);
}
void draw_path_down_left(int x, int y, short int colour){
  draw_line(x , y + 3, x + 16, y + 3, colour);
  draw_line(x + 16, y + 4, x + 16, y + 19, colour);
  draw_line(x + 3, y + 16, x , y + 16, colour);
  draw_line(x + 3, y + 17, x + 3, y + 19, colour);
  plot_pixel(x+3, y+8, YELLOW);
  plot_pixel(x + 13, y + 7, YELLOW);
  plot_pixel(x + 8, y + 12, YELLOW);
  plot_pixel(x + 13, y + 18, YELLOW);
}
void draw_path_up_right(int x, int y, short int colour){
  draw_line(x + 3, y + 16, x + 19, y + 16, colour);
  draw_line(x + 3, y + 15, x + 3, y , colour);
  draw_line(x + 16, y + 3, x + 19, y + 4, colour);
  draw_line(x + 16, y + 0, x + 16, y + 2, colour);
  plot_pixel(x + 7, y + 2, YELLOW);
  plot_pixel(x + 11, y + 12, YELLOW);
  plot_pixel(x + 17, y + 7, YELLOW);
}
void draw_path_up_left(int x, int y, short int colour){
	draw_line(x, y + 16, x + 16, y + 16, colour);
	draw_line(x + 16, y + 0, x + 16, y + 15, colour);
	draw_line(x + 3, y + 3, x, y + 3, colour);
	draw_line(x + 3, y + 0, x + 3, y + 2, colour);
	plot_pixel(x + 3, y + 8, YELLOW);
	plot_pixel(x + 13, y + 7, YELLOW);
	plot_pixel(x + 8, y + 12, YELLOW);
	plot_pixel(x+10, y+2, YELLOW);
}
void draw_grid(){
	for(int x = 0; x < GRID_WIDTH; x++){
		for(int y = 0; y < GRID_HEIGHT; y++){
			switch (Grid[y][x]){
				//need to add
				case Empty: break;
				case Light:	break;		
				case Medium:break;
				case Heavy: break;
				
				case Path_Horizontal_Left:  
				case Path_Horizontal_Right: 
					draw_path_horizontal(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);
					break;
				case Path_Vertical_Up: 
				case Path_Vertical_Down:   
					draw_path_vertical(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);
					break;
				case Path_Right_Down:  
				case Path_Up_Left:
					draw_path_down_left(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);  
					break; 
				case Path_Right_Up: 
				case Path_Down_Left:  
					draw_path_up_left(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);  
					break;
				case Path_Down_Right:
				case Path_Left_Up:
					draw_path_up_right(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);  
					break;
				case Path_Left_Down:
				case Path_Up_Right:
					draw_path_down_right(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);  
				default: break;
			}
		}
	}
}
// health related functions
void updateHealthToLEDR(){
	volatile int *LEDR = (int *)LEDR_BASE;
	*LEDR = ((int)pow(2, health))-1 & 0b1111111111;
}
void loseHealth(){
	health--;
	updateHealthToLEDR();
}