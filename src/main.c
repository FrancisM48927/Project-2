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

void EXTI9_5_IRQHandler(void)
{
    // LEFT button: PF9
    if (EXTI->PR & (1 << 9))
    {
        state = SHIFT_LEFT;
        EXTI->PR = (1 << 9);
    }

    // CENTER button: PF8
    if (EXTI->PR & (1 << 8))
    {
        state = PAUSE;
        EXTI->PR = (1 << 8);
    }

    // RIGHT button: PE6
    if (EXTI->PR & (1 << 6))
    {
        state = SHIFT_RIGHT;
        EXTI->PR = (1 << 6);
    }
}

int main(void) 
{
    // Enable clock for GPIOC, GPIOD, GPIOE, and GPIOF
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | 
                    RCC_AHB1ENR_GPIODEN |
                    RCC_AHB1ENR_GPIOEEN |
                    RCC_AHB1ENR_GPIOFEN;        

    // Configure GPIO pins PD0-PD7 (LEDs) as output
    LED_PORT->MODER &= ~(0xFFFF);
    LED_PORT->MODER |= (0x5555);
    // Configure GPIO pins PC8 - PC11 (Switches) as inputs
    SWITCH_PORT->MODER &= ~(0x00FF0000);
    // Configure GPIO pins PF8-9 & PE6 (Buttons) as inputs
    GPIOE->MODER &= ~(3 << (6 * 2));
    GPIOF->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));

    // Configure GPIO for all buttons to pull-up
    // Pressing the button creates a falling edge
    GPIOF->PUPDR &= ~(3 << (8 * 2)); // PF8 (Center)
    GPIOF->PUPDR |= (1 << (8 * 2));
    GPIOF->PUPDR &= ~(3 << (9 * 2)); // PF9 (Left)
    GPIOF->PUPDR |= (1 << (9 * 2));
    GPIOE->PUPDR &= ~(3 << (6 * 2)); // PE6 (Right)
    GPIOE->PUPDR |= (1 << (6 * 2));

    // Enable EXTI clock and connect PF8, PF9, and PE6 to EXTI689
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[2] &= ~(0x00FF);
    SYSCFG->EXTICR[2] |= (0x0055);
    SYSCFG->EXTICR[1] &= ~(0xF << 8);
    SYSCFG->EXTICR[1] |= (0x4 << 8);

    // Configure EXTI to detect falling edges and enable EXTI lines
    EXTI->FTSR |= (1 << 6) | 
                  (1 << 8) | 
                  (1 << 9);
    EXTI->IMR |= (1 << 6) | 
                 (1 << 8) | 
                 (1 << 9);

    // Enable EXTI9_5 interrupt in NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    while(1) 
    {
        if (state == PAUSE)
        {
            // Turn off any LEDs that should not be on
            LED_PORT->ODR &= ~(0xF0);
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
            Delay_Count(100000);
        }

        else if (state == SHIFT_RIGHT)
        {
            // Shift LEDs right
            LED_PATTERN = (LED_PATTERN >> 1) | (LED_PATTERN << 7);
            LED_PORT->ODR = LED_PATTERN;

            // Temporarily adding delay
            Delay_Count(100000);
        }

        // Delay based on potentiometer
    }
}