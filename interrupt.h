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
void config_HPS_timer(void);
void config_HPS_GPIO1(void);
void config_interval_timer(void);
void config_KEYs(void);
void enable_A9_interrupts(void);
void __attribute__((interrupt)) __cs3_isr_irq(void);

/* key_dir is written by interrupt service routine; we have to
 * declare these as volatile to avoid the compiler caching their values in
 * registers */

extern volatile int key_dir;

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
    else
        while (1)
        ; // if unexpected, then stay here
    // Write to the End of Interrupt Register (ICCEOIR)
    address = MPCORE_GIC_CPUIF + ICCEOIR;
    *((int *)address) = int_ID;
    return;
}