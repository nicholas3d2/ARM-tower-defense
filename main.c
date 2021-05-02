/* 
Description: main.c file for tower defense game, developed for CPULATOR
Authors: Nicholas Ishankov, Jintao Huang
Date: 10/04/2021
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
#define TowerColour 0xFFFE
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

//Tower damage
#define LIGHTTOWERDMG 2.0
#define MEDIUMTOWERDMG 5.0
#define HEAVYTOWERDMG 15.0

// Enemy health stuff
#define LIGHTENEMYHEALTH 5.0
#define MEDIUMENEMYHEALTH 15.0
#define HEAVYENEMYHEALTH 40.0

//Enemy points
#define LIGHTENEMYPOINTS 5.0
#define MEDIUMENEMYPOINTS 15.0
#define HEAVYENEMYPOINTS 25.0

#define NUMENEMIES 15


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/**********#include "interrupt.h"**********/
/* This files provides address values that exist in the system */

#define BOARD "DE1-SoC"

/* Memory */
#define DDR_BASE 0x00000000
#define DDR_END 0x3FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_ONCHIP_END 0xC803FFFF
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define AV_CONFIG_BASE 0xFF203000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C

/* ARM A9 MPCORE devices */
#define PERIPH_BASE 0xFFFEC000       // base address of peripheral devices
#define MPCORE_PRIV_TIMER 0xFFFEC600 // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF 0xFFFEC100 // PERIPH_BASE + 0x100
#define ICCICR 0x00                 // offset to CPU interface control reg
#define ICCPMR 0x04                 // offset to interrupt priority mask reg
#define ICCIAR 0x0C                 // offset to interrupt acknowledge reg
#define ICCEOIR 0x10                // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST 0xFFFED000 // PERIPH_BASE + 0x1000
#define ICDDCR 0x00                // offset to distributor control reg
#define ICDISER 0x100              // offset to interrupt set-enable regs
#define ICDICER 0x180              // offset to interrupt clear-enable regs
#define ICDIPTR 0x800              // offset to interrupt processor targets regs
#define ICDICFR 0xC00              // offset to interrupt configuration regs

#define EDGE_TRIGGERED 0x1
#define LEVEL_SENSITIVE 0x0
#define CPU0 0x01 // bit-mask; bit 0 represents cpu0
#define ENABLE 0x1

#define KEY0 0
#define KEY1 1
#define KEY2 2
#define KEY3 3
#define NONE 4

#define RIGHT 1
#define LEFT 2

#define USER_MODE 0b10000
#define FIQ_MODE 0b10001
#define IRQ_MODE 0b10010
#define SVC_MODE 0b10011
#define ABORT_MODE 0b10111
#define UNDEF_MODE 0b11011
#define SYS_MODE 0b11111

#define INT_ENABLE 0b01000000
#define INT_DISABLE 0b11000000
/* This file provides interrupt IDs */

/* FPGA interrupts (there are 64 in total; only a few are defined below) */
#define INTERVAL_TIMER_IRQ 72
#define KEYS_IRQ 73
#define FPGA_IRQ2 74
#define FPGA_IRQ3 75
#define FPGA_IRQ4 76
#define FPGA_IRQ5 77
#define AUDIO_IRQ 78
#define PS2_IRQ 79
#define JTAG_IRQ 80
#define IrDA_IRQ 81
#define FPGA_IRQ10 82
#define JP1_IRQ 83
#define JP2_IRQ 84
#define FPGA_IRQ13 85
#define FPGA_IRQ14 86
#define FPGA_IRQ15 87
#define FPGA_IRQ16 88
#define PS2_DUAL_IRQ 89
#define FPGA_IRQ18 90
#define FPGA_IRQ19 91

/* ARM A9 MPCORE devices (there are many; only a few are defined below) */
#define MPCORE_GLOBAL_TIMER_IRQ 27
#define MPCORE_PRIV_TIMER_IRQ 29
#define MPCORE_WATCHDOG_IRQ 30

/* HPS devices (there are many; only a few are defined below) */
#define HPS_UART0_IRQ 194
#define HPS_UART1_IRQ 195
#define HPS_GPIO0_IRQ 196
#define HPS_GPIO1_IRQ 197
#define HPS_GPIO2_IRQ 198
#define HPS_TIMER0_IRQ 199
#define HPS_TIMER1_IRQ 200
#define HPS_TIMER2_IRQ 201
#define HPS_TIMER3_IRQ 202
#define HPS_WATCHDOG0_IRQ 203
#define HPS_WATCHDOG1_IRQ 204

void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void config_interval_timer();
void enable_A9_interrupts(void);
void interval_timer_ISR();
void __attribute__((interrupt)) __cs3_isr_irq(void);


/* key_dir is written by interrupt service routine; we have to
 * declare these as volatile to avoid the compiler caching their values in
 * registers */

extern volatile int key_dir;
extern volatile int tick;         // Timer interrupt sets it to 1, must be set to 0 by program

// int main(void) {
//   set_A9_IRQ_stack();      // initialize the stack pointer for IRQ mode
//   config_GIC();            // configure the general interrupt controller
//   // interrupts
//   config_KEYs();          // configure pushbutton KEYs to generate interrupts
//   enable_A9_interrupts(); // enable interrupts
// }

/* setup the KEY interrupts in the FPGA */
void config_KEYs() {
  volatile int *KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
  *(KEY_ptr + 2) = 0b1111;                    // enable interrupts for KEY0-3
}
/*
 * Initialize the banked stack pointer register for IRQ mode
 */
void set_A9_IRQ_stack(void) {
  int stack, mode;
  stack = A9_ONCHIP_END - 7; // top of A9 onchip memory, aligned to 8 bytes
  /* change processor to IRQ mode with interrupts disabled */
  mode = INT_DISABLE | IRQ_MODE;
  asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
  /* set banked stack pointer */
  asm("mov sp, %[ps]" : : [ps] "r"(stack));
  /* go back to SVC mode before executing subroutine return! */
  mode = INT_DISABLE | SVC_MODE;
  asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}
/*
 * Turn on interrupts in the ARM processor
 */
void enable_A9_interrupts(void) {
  int status = SVC_MODE | INT_ENABLE;
  asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}
/*
 * Configure the Generic Interrupt Controller (GIC)
 */
void config_GIC(void) {
  int address; // used to calculate register addresses
  /* configure the HPS timer interrupt */
  *((int *)0xFFFED8C4) = 0x01000000;
  *((int *)0xFFFED118) = 0x00000080;
  /* configure the FPGA interval timer and KEYs interrupts */
  *((int *)0xFFFED848) = 0x00000101;
  *((int *)0xFFFED108) = 0x00000300;
  // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
  // priorities
  address = MPCORE_GIC_CPUIF + ICCPMR;
  *((int *)address) = 0xFFFF;
  // Set CPU Interface Control Register (ICCICR). Enable signaling of
  // interrupts
  address = MPCORE_GIC_CPUIF + ICCICR;
  *((int *)address) = ENABLE;
  // Configure the Distributor Control Register (ICDDCR) to send pending
  // interrupts to CPUs
  address = MPCORE_GIC_DIST + ICDDCR;
  *((int *)address) = ENABLE;
}
void config_interval_timer() {
  volatile int *interval_timer_ptr =
      (int *)TIMER_BASE; // interal timer base address
  /* set the interval timer period for scrolling the HEX displays */
  int counter = 50000000; // 1/(100 MHz) x 5x10^6 = 50 msec
  *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
  *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;
  /* start interval timer, enable its interrupts */
  *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1
}

void pushbutton_ISR(void) {
    volatile int *KEY_ptr = (int *)KEY_BASE;
    int press;
    press = *(KEY_ptr + 3); // read the pushbutton interrupt register
    *(KEY_ptr + 3) = press; // Clear the interrupt
    if(press == 0b1)         // KEY0 pressed
        key_dir = 1;           // Toggle key_dir value
    else if(press == 0b10)  //KEY1 pressed
        key_dir = 2;
    else if (press == 0b100) // KEY1 pressed
      key_dir = 3;
    else if (press == 0b1000) // KEY1 pressed
      key_dir = 4;
    printf("%d", press);
    return;
}
void __attribute__((interrupt)) __cs3_isr_irq(void) {
    // Read the ICCIAR from the processor interface
    int address = MPCORE_GIC_CPUIF + ICCIAR;
    int int_ID = *((int *)address);
    if (int_ID == KEYS_IRQ) // check if interrupt is from the KEYs
        pushbutton_ISR();
    else if (int_ID == INTERVAL_TIMER_IRQ)
      interval_timer_ISR();
    else 
      while (1)
        ; // if unexpected, then stay here
    // Write to the End of Interrupt Register (ICCEOIR)
    address = MPCORE_GIC_CPUIF + ICCEOIR;
    *((int *)address) = int_ID;
    return;
}
void interval_timer_ISR() {
  volatile int *interval_timer_ptr = (int *)TIMER_BASE;
  tick++;
  *(interval_timer_ptr) = 0;               // Clear the interrupt
  return;
}

/**********#include "interrupt.h"**********/
/**********#include "cvector.h"***********/
// from Github: https://github.com/eteran/c-vector
#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <assert.h> /* for assert */
#include <stdlib.h> /* for malloc/realloc/free */
#define CVECTOR_LOGARITHMIC_GROWTH
/**
 * @brief cvector_vector_type - The vector type used in this library
 */
#define cvector_vector_type(type) type *

/**
 * @brief cvector_set_capacity - For internal use, sets the capacity variable of
 * the vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define cvector_set_capacity(vec, size)                                        \
  do {                                                                         \
    if (vec) {                                                                 \
      ((size_t *)(vec))[-1] = (size);                                          \
    }                                                                          \
  } while (0)

/**
 * @brief cvector_set_size - For internal use, sets the size variable of the
 * vector
 * @param vec - the vector
 * @param size - the new capacity to set
 * @return void
 */
#define cvector_set_size(vec, size)                                            \
  do {                                                                         \
    if (vec) {                                                                 \
      ((size_t *)(vec))[-2] = (size);                                          \
    }                                                                          \
  } while (0)

/**
 * @brief cvector_capacity - gets the current capacity of the vector
 * @param vec - the vector
 * @return the capacity as a size_t
 */
#define cvector_capacity(vec) ((vec) ? ((size_t *)(vec))[-1] : (size_t)0)

/**
 * @brief cvector_size - gets the current size of the vector
 * @param vec - the vector
 * @return the size as a size_t
 */
#define cvector_size(vec) ((vec) ? ((size_t *)(vec))[-2] : (size_t)0)

/**
 * @brief cvector_empty - returns non-zero if the vector is empty
 * @param vec - the vector
 * @return non-zero if empty, zero if non-empty
 */
#define cvector_empty(vec) (cvector_size(vec) == 0)

/**
 * @brief cvector_grow - For internal use, ensures that the vector is at least
 * <count> elements big
 * @param vec - the vector
 * @param count - the new capacity to set
 * @return void
 */
#define cvector_grow(vec, count)                                               \
  do {                                                                         \
    const size_t cv_sz = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2);      \
    if (!(vec)) {                                                              \
      size_t *cv_p = malloc(cv_sz);                                            \
      assert(cv_p);                                                            \
      (vec) = (void *)(&cv_p[2]);                                              \
      cvector_set_capacity((vec), (count));                                    \
      cvector_set_size((vec), 0);                                              \
    } else {                                                                   \
      size_t *cv_p1 = &((size_t *)(vec))[-2];                                  \
      size_t *cv_p2 = realloc(cv_p1, (cv_sz));                                 \
      assert(cv_p2);                                                           \
      (vec) = (void *)(&cv_p2[2]);                                             \
      cvector_set_capacity((vec), (count));                                    \
    }                                                                          \
  } while (0)

/**
 * @brief cvector_pop_back - removes the last element from the vector
 * @param vec - the vector
 * @return void
 */
#define cvector_pop_back(vec)                                                  \
  do {                                                                         \
    cvector_set_size((vec), cvector_size(vec) - 1);                            \
  } while (0)

/**
 * @brief cvector_erase - removes the element at index i from the vector
 * @param vec - the vector
 * @param i - index of element to remove
 * @return void
 */
#define cvector_erase(vec, i)                                                  \
  do {                                                                         \
    if (vec) {                                                                 \
      const size_t cv_sz = cvector_size(vec);                                  \
      if ((i) < cv_sz) {                                                       \
        cvector_set_size((vec), cv_sz - 1);                                    \
        size_t cv_x;                                                           \
        for (cv_x = (i); cv_x < (cv_sz - 1); ++cv_x) {                         \
          (vec)[cv_x] = (vec)[cv_x + 1];                                       \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  } while (0)

/**
 * @brief cvector_free - frees all memory associated with the vector
 * @param vec - the vector
 * @return void
 */
#define cvector_free(vec)                                                      \
  do {                                                                         \
    if (vec) {                                                                 \
      size_t *p1 = &((size_t *)(vec))[-2];                                     \
      free(p1);                                                                \
    }                                                                          \
  } while (0)

/**
 * @brief cvector_begin - returns an iterator to first element of the vector
 * @param vec - the vector
 * @return a pointer to the first element (or NULL)
 */
#define cvector_begin(vec) (vec)

/**
 * @brief cvector_end - returns an iterator to one past the last element of the
 * vector
 * @param vec - the vector
 * @return a pointer to one past the last element (or NULL)
 */
#define cvector_end(vec) ((vec) ? &((vec)[cvector_size(vec)]) : NULL)

/* user request to use logarithmic growth algorithm */
#ifdef CVECTOR_LOGARITHMIC_GROWTH

/**
 * @brief cvector_push_back - adds an element to the end of the vector
 * @param vec - the vector
 * @param value - the value to add
 * @return void
 */
#define cvector_push_back(vec, value)                                          \
  do {                                                                         \
    size_t cv_cap = cvector_capacity(vec);                                     \
    if (cv_cap <= cvector_size(vec)) {                                         \
      cvector_grow((vec), !cv_cap ? cv_cap + 1 : cv_cap * 2);                  \
    }                                                                          \
    vec[cvector_size(vec)] = (value);                                          \
    cvector_set_size((vec), cvector_size(vec) + 1);                            \
  } while (0)

#else

/**
 * @brief cvector_push_back - adds an element to the end of the vector
 * @param vec - the vector
 * @param value - the value to add
 * @return void
 */
#define cvector_push_back(vec, value)                                          \
  do {                                                                         \
    size_t cv_cap = cvector_capacity(vec);                                     \
    if (cv_cap <= cvector_size(vec)) {                                         \
      cvector_grow((vec), cv_cap + 1);                                         \
    }                                                                          \
    vec[cvector_size(vec)] = (value);                                          \
    cvector_set_size((vec), cvector_size(vec) + 1);                            \
  } while (0)

#endif /* CVECTOR_LOGARITHMIC_GROWTH */

/**
 * @brief cvector_copy - copy a vector
 * @param from - the original vector
 * @param to - destination to which the function copy to
 * @return void
 */
#define cvector_copy(from, to)                                                 \
  do {                                                                         \
    for (size_t i = 0; i < cvector_size(from); i++) {                          \
      cvector_push_back(to, from[i]);                                          \
    }                                                                          \
  } while (0)

#endif /* CVECTOR_H_ */
/**********#include "cvector.h"***********/

// function prototypes
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void clear_screen();
void wait_for_vsync();
void draw_box(int x, int y, short int line_colour);
void draw_turret_light(int x, int y, short int colour);
void draw_turret_medium(int x, int y, short int colour);
void draw_turret_heavy(int x, int y, short int colour);
void draw_enemy_light(int x, int y, short int colour);
void draw_enemy_medium(int x, int y, short int colour);
void draw_enemy_heavy(int x, int y, short int colour);
//path drawing function prototypes
void draw_path_horizontal(int x, int y, short int colour);
void draw_path_vertical(int x, int y, short int colour);
// <Vert Direction>_<Horizontal Direction> (of opening)
void draw_path_down_right(int x, int y, short int colour);
void draw_path_down_left(int x, int y, short int colour);
void draw_path_up_right(int x, int y, short int colour);
void draw_path_up_left(int x, int y, short int colour);

//update score
void updateScoreOnHEX(volatile int *HEX3_0_ptr, volatile int *HEX5_4_ptr);

// health related functions
void updateHealthToLEDR();
void loseHealth();
//draw Grid
void draw_grid();
void clear_grid();

//draw enemies
void drawEnemies();
// draw enemies' health bars
void drawEnemyHealthBar();
// placing and upgrading towers
void placeOrUpgradeTower();
char get_jtag(volatile int *JTAG_UART_ptr);
void put_jtag(volatile int *JTAG_UART_ptr, char c);

// drawing circles
void circleBres(int xc, int yc, int r, short int colour);		//xc = current x
void drawCircle(int xc, int yc, int x, int y, short int colour); 
void drawTowerRange(); 		// draws tower circles
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
int health = 10; //starting value

// Points
int points = 75; //starting value

// Spawn Rate
int spawnTime = 15;
int spawnRate = 15;

//paused boolean
bool paused = true;

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
  Path_Up_Left,
  Path_End
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
	{Empty , Empty, Empty , Empty, Empty , Empty, Empty , Empty,Empty , Empty, Empty , Empty, Empty , Path_End, Empty , Empty}
};

char seg7[] = {0x3f, 0x06, 0x5B, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67};

// number of towers on the map currently
int numTowers = 0;

//number of enemies on map
int numEnemies = 0;

bool noEnemies = true;

// tower properties:
struct tower{
  int x, y;       // location of the grid, do we need??
  int range;      // range of tower's fire
  int damage;     // damage dealt to enemies
  int reload_time;  // time to reload after each shot
  bool fired;     // false if it didn't fire this frame, true if it did
  bool readyToFire; // if remaining_reload_time = 0, then it is ready to fire (true)
  int remaining_reload_time;  // after firing, copy reload_time and count down to 0, then set readyToFire to true
}Towers[200];  //max 200 towers, enough to cover the whole map

//enemy properties:
struct enemy{
  int x,y;  //position of enemy
  int speed;  //this is the speed of the enemy
  int health; //hp of enemy
  int points; //amount of points the enemy is worth
  int type; //0 = light, 1 = medium, 2 = heavy
  bool active; //see if enemy spawned, and on screen
  
}Enemies[NUMENEMIES]; //max 50 enemies

// tower setup functions
void setTowers(GridElements gridElement, int x, int y);
// update tower's reload etc given tick (variable from interrupt)
void updateTowers();

// spawn enemy
void spawnEnemy(int wave);

// Interrupt KEY
volatile int key_dir = 0;
volatile int tick = 0;


// pixel array stuff
typedef struct XY {
  int x;
  int y;
} xy;

// tower fire control functions
int distance(int x0, int y0, int x1, int y1);	// returns the straight line distance between two points
void towerFireControl();						// controls the firing of the towers
//pixel array initialize
cvector_vector_type(xy) pixel_current = NULL;	// current pixels drawn
cvector_vector_type(xy) pixel_prev1 = NULL;		// previous frame drawn
cvector_vector_type(xy) pixel_prev2 = NULL;		// 2 frams ago (clears the screen using this)
void update_pixel_buffer();						// shifts pixel buffer
void clear_pixels();							// clears pixels drawn
void resetData(); //clean up datastructures
/************main.h************/

int main(void) {
  
  
  int wave = 1;

  volatile int *JTAG_UART_ptr = (int *)0xFF201000; // JTAG UART address
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  volatile int *HEX3_0_ptr = (int *)HEX3_HEX0_BASE;
  volatile int *HEX5_4_ptr = (int *)HEX5_HEX4_BASE;

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

	// Setting up interrupts
	set_A9_IRQ_stack();      // initialize the stack pointer for IRQ mode
	config_GIC();            // configure the general interrupt controller
	// interrupts
	config_KEYs();          // configure pushbutton KEYs to generate interrupts
	config_interval_timer();
	enable_A9_interrupts(); // enable interrupts

	// draw grid box (user controlled grid box)

	// JTAG UART
	char text_string[] = "\nWelcome to ARM Tower Defense\n> \0";
	char *str, c;
		
	for (str = text_string; *str != 0; ++str) {
		put_jtag(JTAG_UART_ptr, *str);
	}

	// initialize health
	updateHealthToLEDR();
  
	// Main program loop, read user inputs while running
	while (1) {
    noEnemies = true;
    while(paused){
      placeOrUpgradeTower(); //key 3 read in here!
      //do nothing, wait for user to unpause
    }

    //update score on HEX3-0
    updateScoreOnHEX(HEX3_0_ptr, HEX5_4_ptr);
    
		// clear 2 frames before
		clear_pixels();
		// draw
		draw_grid();
		draw_grid_box(xcurrent, ycurrent, WHITE);
		drawEnemyHealthBar(); // draw active enemy health bars
		drawEnemies(); //draw active enemies

		towerFireControl();
		drawTowerRange(); 	
		//spawn an enemy
		if(spawnRate == spawnTime){
		spawnEnemy(wave);
		spawnRate = 0; //reset respawn time
      if(numEnemies % 5 == 0 && spawnTime > 10){ //every 5 spawns lower spawn time
        spawnTime--;
      }
		}
		// update position
		updateTowers();
		xprev2 = xprev1;
		yprev2 = yprev1;

		xprev1 = xcurrent;
		yprev1 = ycurrent;
		update_pixel_buffer();

		if(key_dir!=0)
			placeOrUpgradeTower();
		c = get_jtag(JTAG_UART_ptr);
		if (c != '\0') {
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

    if(numEnemies == NUMENEMIES && noEnemies){ //round ended
      char game_over[] = "\nWave ended\n> \0";
			for (str = game_over; *str != 0; ++str) {
				put_jtag(JTAG_UART_ptr, *str);
			}
      numEnemies = 0;
      wave++;
    }

		// game over if no health left, exits the program
		if(health == 0){	
			char game_over[] = "\nGame Over\n> \0";
			for (str = game_over; *str != 0; ++str) {
				put_jtag(JTAG_UART_ptr, *str);
			}
			break;
		}
    if(wave == 4){
      char game_over[] = "\nYou Win!\n> \0";
			for (str = game_over; *str != 0; ++str) {
				put_jtag(JTAG_UART_ptr, *str);
			}
			break;
    }
	}
  resetData(); 
}

void resetData(){
  clear_screen();
  numEnemies = 0;
  numTowers = 0;
}

void updateScoreOnHEX(volatile int *HEX3_0_ptr, volatile int *HEX5_4_ptr){
  *HEX3_0_ptr = seg7[points % 10 & 0xF] | seg7[(points/10)%10 & 0xF] << 8 
  | seg7[(points/100)%10 & 0xF] << 16 | seg7[(points/1000)%10 & 0xF] << 24;
  *HEX5_4_ptr = seg7[(points/10000)%10 & 0xF] | seg7[(points/100000)%10 & 0xF] << 8;
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
//draw user "cursor" as a box on the grid
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
  if(line_color!=0 && line_color!=GREEN && line_color != TowerColour){
	  xy new;
	  new.x = x;
	  new.y = y;
	  cvector_push_back(pixel_current, new);
  }
}

//get user input
char get_jtag(volatile int *JTAG_UART_ptr) {
  int data;
  data = *(JTAG_UART_ptr);
  if (data & 0x00008000) // check RVALID
    return ((char)data & 0xFF);
  else
    return ('\0');
}

//print user text to console
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
  for(int i = x+1; i < x+19; i++){
    for(int j = y+2; j <y+ 18; j++){
      plot_pixel(i, j, colour);
    }
  }
  plot_pixel(x+4, y+2, 0);
  plot_pixel(x+5, y+2, 0);
  plot_pixel(x+6, y+2, 0);
  plot_pixel(x+7, y+2, 0);
  plot_pixel(x+7, y+3, 0);

  plot_pixel(x+15, y+2, 0);
  plot_pixel(x+14, y+2, 0);
  plot_pixel(x+13, y+2, 0);
  plot_pixel(x+12, y+2, 0);
  plot_pixel(x+12, y+3, 0);

  for(int i = x+1; i < x+4; i++){
    for(int j = y+7; j < y+15; j++){
      plot_pixel(i, j, 0);
    }
  }

  for(int i = x+16; i < x+19; i++){
    for(int j = y+7; j < y+15; j++){
      plot_pixel(i, j, 0);
    }
  }

  plot_pixel(x+1, y+4, 0);
  plot_pixel(x+1, y+5, 0);
  plot_pixel(x+1, y+6, 0);
  plot_pixel(x+2, y+6, 0);

  plot_pixel(x+18, y+4, 0);
  plot_pixel(x+18, y+5, 0);
  plot_pixel(x+18, y+6, 0);
  plot_pixel(x+17, y+6, 0);

  plot_pixel(x+5, y+6, 0);
  plot_pixel(x+6, y+6, 0);
  plot_pixel(x+5, y+7, 0);
  plot_pixel(x+6, y+7, 0);

  plot_pixel(x+9, y+6, 0);
  plot_pixel(x+10, y+6, 0);
  plot_pixel(x+9, y+7, 0);
  plot_pixel(x+10, y+7, 0);

  plot_pixel(x+13, y+6, 0);
  plot_pixel(x+14, y+6, 0);
  plot_pixel(x+13, y+7, 0);
  plot_pixel(x+14, y+7, 0);

}


void draw_enemy_light(int x, int y, short int colour){
  draw_line(x+10, y+4, x+15, y+9, colour);
  draw_line(x+15, y+10, x+10, y+15, colour);
  draw_line(x+9, y+15, x+4, y+10, colour);
  draw_line(x+4, y+9, x+9, y+4, colour);
	
  draw_line(x+10, y+5, x+14, y+9, colour);
  draw_line(x+14, y+10, x+10, y+14, colour);
  draw_line(x+9, y+14, x+5, y+10, colour);
  draw_line(x+5, y+9, x+9, y+5, colour);
  

}

void draw_enemy_medium(int x, int y, short int colour){
  draw_line(x+10, y+4, x+15, y+9, colour);
  draw_line(x+15, y+10, x+10, y+15, colour);
  draw_line(x+9, y+15, x+4, y+10, colour);
  draw_line(x+4, y+9, x+9, y+4, colour);
	
  draw_line(x+10, y+5, x+14, y+9, colour);
  draw_line(x+14, y+10, x+10, y+14, colour);
  draw_line(x+9, y+14, x+5, y+10, colour);
  draw_line(x+5, y+9, x+9, y+5, colour);
	
  draw_line(x+9, y+6, x+6, y+9, colour);
  draw_line(x+6, y+10, x+10, y+6, colour);
  draw_line(x+10, y+13, x+13, y+10, colour);
  draw_line(x+13, y+9, x+9, y+13, colour);
  

}

void draw_enemy_heavy(int x, int y, short int colour){
  draw_line(x+10, y+4, x+15, y+9, colour);
  draw_line(x+15, y+10, x+10, y+15, colour);
  draw_line(x+9, y+15, x+4, y+10, colour);
  draw_line(x+4, y+9, x+9, y+4, colour);
	
  draw_line(x+10, y+5, x+14, y+9, colour);
  draw_line(x+14, y+10, x+10, y+14, colour);
  draw_line(x+9, y+14, x+5, y+10, colour);
  draw_line(x+5, y+9, x+9, y+5, colour);
	
  draw_line(x+7, y+11, x+11, y+7, colour);
  draw_line(x+7, y+12, x+12, y+7, colour);
  draw_line(x+8, y+12, x+12, y+8, colour);
  
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
  draw_line(x + 16, y + 3, x + 19, y + 3, colour);
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
//draw contents saved for each grid
void draw_grid(){
	for(int x = 0; x < GRID_WIDTH; x++){
		for(int y = 0; y < GRID_HEIGHT; y++){
			switch (Grid[y][x]){
				//need to add
				case Empty: break;
				case Light:	
					draw_turret_light(x*GRID_LEN, y*GRID_LEN, TowerColour);
					break;		
				case Medium:
					draw_turret_medium(x*GRID_LEN, y*GRID_LEN, TowerColour);
					break;
				case Heavy: 
					draw_turret_heavy(x*GRID_LEN, y*GRID_LEN, TowerColour);
					break;
				
				case Path_Horizontal_Left:  
				case Path_Horizontal_Right: 
					draw_path_horizontal(GRID_LEN * x, GRID_LEN * y, PATH_COLOUR);
					break;
				case Path_Vertical_Up: 
				case Path_Vertical_Down:
        case Path_End:   
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
//draw and update enemies
void drawEnemies(){
  for(int i = 0; i < numEnemies; i++){
    if(Enemies[i].active){
    noEnemies = false;
	  if(Enemies[i].health <=0){
		Enemies[i].active = false;
    points += Enemies[i].points;
		continue;
	  }
      if(Enemies[i].type == 0){ //light
        draw_enemy_light(Enemies[i].x, Enemies[i].y, WHITE);
      }else if(Enemies[i].type == 1){ //medium
        draw_enemy_medium(Enemies[i].x, Enemies[i].y, WHITE);
      }else{ //heavy
        draw_enemy_heavy(Enemies[i].x, Enemies[i].y, WHITE);
      }
      //determine type of grid enemy is on, update x,y accordingly
	  int gridY = Enemies[i].y/GRID_LEN;
	  int gridX = Enemies[i].x/GRID_LEN;
      GridElements currentGrid = Grid[Enemies[i].y/GRID_LEN][Enemies[i].x/GRID_LEN];
      if(currentGrid == Path_Vertical_Down ||  currentGrid == Path_Right_Down){
        Enemies[i].y += Enemies[i].speed;
      }else if(currentGrid == Path_Down_Right || currentGrid == Path_Horizontal_Right
	  || (currentGrid == Path_Up_Right && Enemies[i].y == gridY*GRID_LEN)){
        Enemies[i].x += Enemies[i].speed;
      }else if(currentGrid == Path_Right_Up || currentGrid == Path_Vertical_Up || 
	  (currentGrid == Path_Up_Right && Enemies[i].y!= gridY*GRID_LEN)){
        Enemies[i].y -= Enemies[i].speed;
	  
      }else if(currentGrid == Path_End){ //reached PATH_END
        Enemies[i].active = false;
        loseHealth(); //player loses health
        if(Enemies[i].type == 0){ //light
          draw_enemy_light(Enemies[i].x, Enemies[i].y, 0);
        }else if(Enemies[i].type == 1){ //medium
          draw_enemy_medium(Enemies[i].x, Enemies[i].y, 0);
        }else{ //heavy
          draw_enemy_heavy(Enemies[i].x, Enemies[i].y, 0);
        }
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
//place a tower or pause the game
void placeOrUpgradeTower(){
	GridElements currentlySelected = Grid[ycurrent / GRID_LEN][xcurrent/GRID_LEN];
	switch (key_dir) {
		case 1: // purchase light tower
			if (currentlySelected == 0 && points >= 25)
			{
				Grid[ycurrent / GRID_LEN][xcurrent/GRID_LEN] = Light;
        		points -= 25;
				setTowers(Light, xcurrent+10, ycurrent+10);
			}
			break;
		case 2:
			if (currentlySelected == 0 && points >= 50) {
				Grid[ycurrent / GRID_LEN][xcurrent / GRID_LEN] = Medium;
        		points -= 50;
				setTowers(Medium, xcurrent+10, ycurrent+10);
			}
			break;
		case 3:
			if (currentlySelected == 0 && points >= 100) {
				Grid[ycurrent / GRID_LEN][xcurrent / GRID_LEN] = Heavy;
        		points -= 100;
				setTowers(Heavy, xcurrent+10, ycurrent+10);
			}
			break;
		case 4:									// start/stop game
      paused = !paused;
			break;
		default:
			break;
	}
	key_dir = 0;
	return;
}
//spawn a new enemy
void spawnEnemy(int wave){
  if(numEnemies < NUMENEMIES){
    //determine kind of enemy to add to array (random? scripted?)
    if(wave == 1){
      int type = rand() % 3; //random number 0,1,2
      if(type == 0){
        //LIGHT ENEMY
        Enemies[numEnemies].type = 0;
        //spawn x,y coords
        Enemies[numEnemies].x = 2*GRID_LEN;
        Enemies[numEnemies].y = 0;
        Enemies[numEnemies].active = TRUE;
        Enemies[numEnemies].health = LIGHTENEMYHEALTH;
        Enemies[numEnemies].points = LIGHTENEMYPOINTS;
        Enemies[numEnemies].speed = 2;
      }else if(type == 1){
        //MEDIUM ENEMY
        Enemies[numEnemies].type = 1;
        //spawn x,y coords
        Enemies[numEnemies].x = 2*GRID_LEN;
        Enemies[numEnemies].y = 0;
        Enemies[numEnemies].active = TRUE;
        Enemies[numEnemies].health = MEDIUMENEMYHEALTH;
        Enemies[numEnemies].points = MEDIUMENEMYPOINTS;
        Enemies[numEnemies].speed = 2;
      }else{
        //HEAVY ENEMY
        Enemies[numEnemies].type = 2;
        //spawn x,y coords
        Enemies[numEnemies].x = 2*GRID_LEN;
        Enemies[numEnemies].y = 0;
        Enemies[numEnemies].active = TRUE;
        Enemies[numEnemies].health = HEAVYENEMYHEALTH;
        Enemies[numEnemies].points = HEAVYENEMYPOINTS;
        Enemies[numEnemies].speed = 1;
      }

    }else if(wave == 2){
      //HEAVY ENEMY
        Enemies[numEnemies].type = 2;
        //spawn x,y coords
        Enemies[numEnemies].x = 2*GRID_LEN;
        Enemies[numEnemies].y = 0;
        Enemies[numEnemies].active = TRUE;
        Enemies[numEnemies].health = HEAVYENEMYHEALTH;
        Enemies[numEnemies].points = HEAVYENEMYPOINTS;
        Enemies[numEnemies].speed = 2;

    }else{ //wave 3!
      //HEAVY ENEMY
        Enemies[numEnemies].type = 2;
        //spawn x,y coords
        Enemies[numEnemies].x = 2*GRID_LEN;
        Enemies[numEnemies].y = 0;
        Enemies[numEnemies].active = TRUE;
        Enemies[numEnemies].health = 2*HEAVYENEMYHEALTH;
        Enemies[numEnemies].points = HEAVYENEMYPOINTS;
        Enemies[numEnemies].speed = 2;

    }
    numEnemies++;
  }
}

// Function to put pixels
// at subsequence points
void drawCircle(int xc, int yc, int x, int y, short int colour) {
  plot_pixel(xc + x, yc + y, colour);
  plot_pixel(xc - x, yc + y, colour);
  plot_pixel(xc + x, yc - y, colour);
  plot_pixel(xc - x, yc - y, colour);
  plot_pixel(xc + y, yc + x, colour);
  plot_pixel(xc - y, yc + x, colour);
  plot_pixel(xc + y, yc - x, colour);
  plot_pixel(xc - y, yc - x, colour);
}

// Function for circle-generation
// using Bresenham's algorithm
void circleBres(int xc, int yc, int r, short int colour) {
  int x = 0, y = r;
  int d = 3 - 2 * r;
  drawCircle(xc, yc, x, y, colour);
  while (y >= x) {
    // for each pixel we will
    // draw all eight pixels

    x++;

    // check for decision parameter
    // and correspondingly
    // update d, x, y
    if (d > 0) {
      y--;
      d = d + 4 * (x - y) + 10;
    } else
      d = d + 4 * x + 6;
    drawCircle(xc, yc, x, y, colour);
  }
}

// tower setup functions
void setTowers(GridElements gridElement, int x, int y){
    switch (gridElement)
    {
        case Light:
            Towers[numTowers].x = x;
            Towers[numTowers].y = y;
			Towers[numTowers].damage = LIGHTTOWERDMG;
			Towers[numTowers].fired = false;
			Towers[numTowers].range = 50;
			Towers[numTowers].readyToFire = true;
			Towers[numTowers].reload_time = 4;
			Towers[numTowers].remaining_reload_time = 0;
			break;
		case Medium:
			Towers[numTowers].x = x;
            Towers[numTowers].y = y;
			Towers[numTowers].damage = MEDIUMTOWERDMG;
			Towers[numTowers].fired = false;
			Towers[numTowers].range = 80;
			Towers[numTowers].readyToFire = true;
			Towers[numTowers].reload_time = 8;
			Towers[numTowers].remaining_reload_time = 0;
			break;
		case Heavy:
			Towers[numTowers].x = x;
            Towers[numTowers].y = y;
			Towers[numTowers].damage = HEAVYTOWERDMG;
			Towers[numTowers].fired = false;
			Towers[numTowers].range = 100;
			Towers[numTowers].readyToFire = true;
			Towers[numTowers].reload_time = 15;
			Towers[numTowers].remaining_reload_time = 0;
    default:
      break;
    }
    numTowers++;                //increment number of tower
}

//update tower status
void updateTowers(){
	if(tick){
		tick = 0;				// reset tick
    spawnRate++;    // increment spawn timer
		for(int i = 0; i < numTowers; i++){
			if(Towers[i].remaining_reload_time > 0){
				Towers[i].remaining_reload_time--;			// decrement time
				Towers[i].readyToFire = false;
				Towers[i].fired = false;
			}else if(Towers[i].remaining_reload_time == 0 && !Towers[i].fired){
				Towers[i].readyToFire = true;
			}else if(Towers[i].remaining_reload_time == 0 && Towers[i].fired){
				Towers[i].readyToFire = false;
				Towers[i].fired = false;
				Towers[i].remaining_reload_time = Towers[i].reload_time;
			}
		}
	}
} 

// shifts pixel buffer
void update_pixel_buffer(){
	for(int i = 0; i < cvector_size(pixel_prev2); i++){
		cvector_pop_back(pixel_prev2);
	}
	cvector_copy(pixel_prev1, pixel_prev2);
	for(int i = 0; i < cvector_size(pixel_prev1); i++){
		cvector_pop_back(pixel_prev1);
	}
	cvector_copy(pixel_current, pixel_prev1);
	for(int i = 0; i < cvector_size(pixel_current); i++){
		cvector_pop_back(pixel_current);
	}
}
// clears pixels drawn
void clear_pixels(){
	for(int i = 0; i < cvector_size(pixel_prev2); i++){
		plot_pixel(pixel_prev2[i].x, pixel_prev2[i].y, 0);
	}
}
void drawTowerRange() {
  	for (int i = 0; i < numTowers; i++) {
		if (Towers[i].x == xcurrent + 10 && Towers[i].y == ycurrent + 10) {
		circleBres(Towers[i].x, Towers[i].y, Towers[i].range, ORANGE);
		break;
		}
  }
}
// returns the straight line distance between two points
int distance(int x0, int y0, int x1, int y1){
	return sqrt( pow(x0-x1, 2) + pow(y0 - y1, 2));
}

 // controls the firing of the towers
void towerFireControl(){
	for(int i = 0; i < numTowers; i++){
		if(Towers[i].readyToFire){
			for(int j = 0; j < numEnemies; j ++){
				if(Towers[i].readyToFire && 
				   Enemies[j].active && 
				   Towers[i].range >= distance(Towers[i].x, Towers[i].y, Enemies[j].x+10, Enemies[j].y+10)
				   ){
					draw_line(Towers[i].x, Towers[i].y, Enemies[j].x+10, Enemies[j].y+10,
							RED);
					draw_line(Towers[i].x+1, Towers[i].y, Enemies[j].x+11, Enemies[j].y+10,
							RED);
					Towers[i].fired = true;
					Towers[i].readyToFire = false;
					Enemies[j].health -= Towers[i].damage;
				}
			}
		}
	}
}
//draws enemy health bars. 10 pixels for full health
void drawEnemyHealthBar(){
	for(int i = 0; i < numEnemies; i++){
		if(Enemies[i].active){
			int healthBarLength = 0;
			switch (Enemies[i].type)
			{
			case 0:		//light
				healthBarLength = ((double) Enemies[i].health / LIGHTENEMYHEALTH) * 10;
				break;
			case 1:
				healthBarLength = ((double) Enemies[i].health / MEDIUMENEMYHEALTH) * 10;
				break;
			case 2:
				healthBarLength = ((double) Enemies[i].health / HEAVYENEMYHEALTH) * 10;
				break;
			default:
				break;
			}
			draw_line(Enemies[i].x+5, Enemies[i].y, Enemies[i].x+5+healthBarLength, Enemies[i].y, BLUE);
			draw_line(Enemies[i].x+5, Enemies[i].y-1, Enemies[i].x+5+healthBarLength, Enemies[i].y-1, BLUE);
		}
	}
}