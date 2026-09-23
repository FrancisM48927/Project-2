/********************************************************************** 
* Author: F. MAILOM
* CPEG222 Project 2, 9/21/26
* NucleoF466ZE CMSIS Sequence LEDs w/Pause/Rev w/BTN
**********************************************************************/
#include "stm32f4xx.h"

#define SWITCH_PORT GPIOC
#define LED_PORT GPIOD
#define ANALOG_PIN 2

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
        // Read switches & copy status to LEDs
        uint32_t switches = (SWITCH_PORT->IDR >> 8) & 0x0F;
        LED_PORT->ODR = (LED_PORT->ODR & ~0x0F) | switches;
    }
}