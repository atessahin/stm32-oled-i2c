#include "stm32f4xx.h"                  // Device header

void systemClockConfig();
void gpioConfig();
void i2cConfig();
void i2cStart();
void i2cStop();
void i2cWrite(uint8_t data);
void i2cSendAddress(uint8_t address);
void oledSendCommand(uint8_t command);
void oledSendData(uint8_t data);
void oledInit();
void oledClear();
void oledSetCursor(uint8_t page, uint8_t col);
void oledWriteChar(char c);
void oledWriteString(char* str);
void oledWriteNumber(uint16_t num);
void oledTypewriterEffect(char* name);
void delay(uint32_t time);
