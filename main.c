#include "efm32gg990f1024.h"

#define LED_PORT 4            // gpioPortE
#define LED_PIN0 2            // LED0 is connected to PortE pin2
#define TOP_VAL_PWM 1000      // sets PWM frequency to 1kHz (1MHz timer clock)
#define TOP_VAL_GP_TIMER 1000 // sets general purpose timer overflow frequency to 1kHz (1MHz timer clock)

#define SWITCH_PORT_D 3 // PORT D
#define SWITCH_PORT_C 2 // PORT C
#define PD0 0           // PD0
#define PD1 1           // PD1
#define PD2 2           // PD2
#define PC0 0           // PC0

void Delay(uint32_t delay)
{
    volatile uint32_t counter;
    int i;

    for (i = 0; i < delay; i++)
    {
        counter = 500;
        while (counter)
            counter--;
    }
}

void Config_LED_PWM()
{
    CMU->HFRCOCTRL = 0x8;                               // Set High Freq. RC Osc. to 1 MHz and use as system source
    CMU->HFPERCLKEN0 = (1 << 13) | (1 << 8) | (1 << 5); // Enable GPIO, Timer0, and Timer3 peripheral clocks
    GPIO->P[LED_PORT].MODEL = (4 << 12) | (4 << 8);     // Configure LED0 pin as digital output (push-pull)

    TIMER0->TOP = TOP_VAL_GP_TIMER; // GP Timer period will be 1ms = 1kHz freq
    TIMER3->TOP = TOP_VAL_PWM;      // PWM period will be 1ms = 1kHz freq

    TIMER0->CNT = 0; // Start counter at 0 (up-count mode)
    TIMER3->CNT = 0; // Start counter at 0 (up-count mode)

    TIMER3->CC[2].CCV = 0;  // Set CC2 compare value (0% duty)
    TIMER3->CC[2].CCVB = 0; // Set CC2 compare buffer value (0% duty)

    TIMER3->CC[2].CTRL = 0x3;             // Put Timer3 CC channel 2 in PWM mode
    TIMER3->ROUTE = (1 << 16) | (1 << 2); // Connect PWM output (timer3, channel 2) to PE2 (LED0). See EFM32GG990 datasheet for details.

    TIMER0->CMD = 0x1; // Start Timer0
    TIMER3->CMD = 0x1; // Start Timer3
}

void Config_INPUT_PORTS()
{
    // Configure PD1 as switch input
    GPIO->P[SWITCH_PORT_D].MODEL = (4 << 0) | (4 << 4) | (4 << 8);
    GPIO->P[SWITCH_PORT_D].DOUTSET = (1 << PD0) | (1 << PD1) | (1 << PD2); // Enable Pull-ups

    // Configure PC0 as input
    GPIO->P[SWITCH_PORT_C].MODEL = (4 << 0);
    GPIO->P[SWITCH_PORT_C].DOUTSET = (1 << PC0); // Enable Pull-ups
}

void brightness(int chave0, int chave1, int chave2, int chave3)
{
    if (chave0 && chave1 && chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 0;
    }
    else if (!chave0 && chave1 && chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 16;
    }
    else if (chave0 && !chave1 && chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 32;
    }
    else if (chave0 && chave1 && !chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 48;
    }
    else if (chave0 && chave1 && chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 64;
    }
    else if (!chave0 && !chave1 && chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 80;
    }
    else if (!chave0 && chave1 && !chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 96;
    }
    else if (!chave0 && chave1 && chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 112;
    }
    else if (!chave0 && !chave1 && !chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 128;
    }
    else if (!chave0 && !chave1 && chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 144;
    }
    else if (!chave0 && chave1 && !chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 160;
    }
    else if (chave0 && !chave1 && !chave2 && chave3)
    {
        TIMER3->CC[2].CCVB = 176;
    }
    else if (chave0 && !chave1 && chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 192;
    }
    else if (chave0 && chave1 && !chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 208;
    }
    else if (!chave0 && !chave1 && !chave2 && !chave3)
    {
        TIMER3->CC[2].CCVB = 255;
    }
}

int main()
{
    Config_LED_PWM();
    Config_INPUT_PORTS();

    int statusCS = 0;

    while (1)
    {
        brightness(GPIO->P[SWITCH_PORT_D].DIN & (1 << PD0), GPIO->P[SWITCH_PORT_D].DIN & (1 << PD1), GPIO->P[SWITCH_PORT_D].DIN & (1 << PD2), GPIO->P[SWITCH_PORT_C].DIN & (1 << PC0));
    }
}
