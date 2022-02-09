#include "efm32gg990f1024.h"
// #include "em_chip.h" // required for CHIP_Init() function

#define LED_PORT 4 // gpioPortE
#define LED_PIN0 2 // LED0 is connected to PortE pin2
// #define LED_PIN1 3                // LED1 is connected to PortE pin3
#define TOP_VAL_PWM 1000          // sets PWM frequency to 1kHz (1MHz timer clock)
#define TOP_VAL_GP_TIMER 1000     // sets general purpose timer overflow frequency to 1kHz (1MHz timer clock)
#define UPDATE_PERIOD 250         // update compare value and toggle LED1 every 250ms
#define INC_VAL (TOP_VAL_PWM / 4) // adjust compare value amount

uint16_t ms_counter = 0; // global variable to count general purpose timer overflow events

// TIMER ISR, executes every ms
void TIMER0_IRQHandler(void)
{
    TIMER0->IFC = 1; // Clear source of interrupt
    ms_counter++;    // Increment Counter
}

int main()
{

    uint16_t compare_val = 0; // Initial PWM duty cycle is 0% (LED0 off)
    uint8_t inc = 1;          // Increment = true

    // CHIP_Init(); // This function addresses some chip errata and should be called at the start of every EFM32 application (need em_system.c)

    CMU->HFRCOCTRL = 0x8;                               // Set High Freq. RC Osc. to 1 MHz and use as system source
    CMU->HFPERCLKEN0 = (1 << 13) | (1 << 8) | (1 << 5); // Enable GPIO, Timer0, and Timer3 peripheral clocks
    GPIO->P[LED_PORT].MODEL = (4 << 12) | (4 << 8);     // Configure LED0 and LED1 pins as digital outputs (push-pull)
    // GPIO->P[LED_PORT].DOUTSET = (1 << LED_PIN1);        // Turn on LED1 (PE3)

    TIMER0->TOP = TOP_VAL_GP_TIMER; // GP Timer period will be 1ms = 1kHz freq
    TIMER3->TOP = TOP_VAL_PWM;      // PWM period will be 1ms = 1kHz freq

    TIMER0->CNT = 0; // Start counter at 0 (up-count mode)
    TIMER3->CNT = 0; // Start counter at 0 (up-count mode)

    TIMER3->CC[2].CCV = compare_val;  // Set CC2 compare value (0% duty)
    TIMER3->CC[2].CCVB = compare_val; // Set CC2 compare buffer value (0% duty)

    TIMER0->IEN = 1;             // Enable Timer0 overflow interrupt
    NVIC_EnableIRQ(TIMER0_IRQn); // Enable TIMER0 interrupt vector in NVIC

    TIMER3->CC[2].CTRL = 0x3;             // Put Timer3 CC channel 2 in PWM mode
    TIMER3->ROUTE = (1 << 16) | (1 << 2); // Connect PWM output (timer3, channel 2) to PE2 (LED0). See EFM32GG990 datasheet for details.

    TIMER0->CTRL = (1 << 6); // Allow timer to run while in debug mode
    TIMER3->CTRL = (1 << 6); // Allow timer to run while in debug mode

    TIMER0->CMD = 0x1; // Start Timer0
    TIMER3->CMD = 0x1; // Start Timer3

    while (1)
    {
        if (ms_counter == UPDATE_PERIOD)
        {
            if (inc)
            {                           // If increment = true
                compare_val += INC_VAL; // Increase the compare value
            }
            else
            {                           // If increment = false
                compare_val -= INC_VAL; // Decrease the compare value
            }
            TIMER3->CC[2].CCVB = compare_val; // Write new value to compare buffer
            // GPIO->P[LED_PORT].DOUTTGL = (1 << LED_PIN1); // Toggle LED1
            ms_counter = 0; // Reset counter
        }
        if (compare_val > (TOP_VAL_PWM - 1))
        {
            inc = 0;
        } // If compare value is at max, start decrementing
        if (compare_val < 1)
        {
            inc = 1;
        } // If compare value is at min, start incrementing
    }
}