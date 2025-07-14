# stm32-oled-i2c-register(no hal)
This project demonstrates how to initialize and use an SSD1306 OLED display with an STM32F4 microcontroller using I2C communication. All configurations are done using **direct register access**, not HAL or CMSIS libraries.

## Features

- Custom system clock configuration (HSE + PLL for 84 MHz)
- OLED initialization and data writing
- Basic font rendering (5x7 characters)
- Typewriter-style display effect

## Pinout

| OLED Pin | STM32 Pin | Description   |
|----------|-----------|---------------|
| VCC      | 3.3V      | Power supply  |
| GND      | GND       | Ground        |
| SCL      | PB6       | I2C Clock     |
| SDA      | PB7       | I2C Data      |

## Font Support
The driver includes a built-in 5x7 pixel font supporting:

- **Numbers:** `0–9`  
- **Uppercase letters:** `A–Z`  
- **Lowercase letters:** `a–z`  
- **Colon character:** `:`

You can easily display text, numbers, or a combination using the provided functions.

## How to Use

```c
#include "config.h"

int main()
{
    systemClockConfig();  // Setup system clock (HSE -> PLL -> 84MHz)
    gpioConfig();         // Configure GPIO for I2C
    i2cConfig();          // Initialize I2C1 peripheral

    oledInit();           // Initialize OLED
    oledClear();          // Clear the screen

    oledWriteString("HELLO");           // Print text
    oledWriteNumber(123);               // Print number
    oledTypewriterEffect("STM32 DEMO"); // Typewriter effect

    while (1) {
        delay(50000000);
    }
}

You can also set custom position using:
oledSetCursor(page, column); // page: 0–7, column: 0–127
