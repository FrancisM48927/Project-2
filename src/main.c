/********************************************************************** 
* Author: F. MAILOM
* CPEG222 Project 2, 9/21/26
* NucleoF466ZE CMSIS Sequence LEDs w/Pause/Rev w/BTN
**********************************************************************/
#include "stm32f4xx.h"

#define SWITCH_PORT GPIOC
#define LED_PORT GPIOD
#define ANALOG_PIN 2

#define SHIFT_LEFT 0
#define PAUSE 1
#define SHIFT_RIGHT 2

uint8_t state = PAUSE;
uint8_t LED_PATTERN = 0x00;

void Delay_Count(volatile uint32_t count) 
{
    while (count--) 
    {
    }
}

int main(void) 
{
    // Enable clock for GIOD (LEDs)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;

    // Configurre GPIO pins PD0-PD7 (LEDs) as output
    LED_PORT->MODER &= ~(0xFFFF);
    LED_PORT->MODER |= (0x5555);

    // Configure PC8 - PC11 (Switches) as inputs
    SWITCH_PORT->MODER &= ~(0x00FF0000);

    while(1) 
    {
        if (state == PAUSE)
        {
            // Read switches & copy status to LEDs
            uint32_t switches = (SWITCH_PORT->IDR >> 8) & 0x0F;
            LED_PORT->ODR = (LED_PORT->ODR & ~0x0F) | switches;
            LED_PATTERN = switches;
        }

        else if (state == SHIFT_LEFT)
        {
            // Shift LEDs left
            LED_PATTERN = (LED_PATTERN << 1) | (LED_PATTERN >> 7);
            LED_PORT->ODR = LED_PATTERN;

            // Temporarily adding delay
            Delay_Count(400000);
        }

        else if (state == SHIFT_RIGHT)
        {
            // Shift LEDs right
            LED_PATTERN = (LED_PATTERN >> 1) | (LED_PATTERN << 7);
            LED_PORT->ODR = LED_PATTERN;

            // Temporarily adding delay
            Delay_Count(400000);
        }

        // Delay based on potentiometer
    }
}