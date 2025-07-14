#include "config.h"    
void delay(uint32_t time);
void systemClockConfig()
{
	RCC->CR |=(1<<16);              // HSE ON
	while(!(RCC->CR & (1<<17)));    // Wait until HSE is ready
	
	FLASH->ACR|=(3<<1)|(1<<8)|(1<<9)|(1<<10);
	// Flash latency: 3 wait states
	// Enable prefetch buffer
	// Enable instruction cache
	// Enable data cache
	
	RCC->APB1ENR|=(1<<28);          // Power interface clock enable
	PWR->CR |=(3U<<14);             // Voltage regulator scale 1 mode
	
	RCC->CR &=~(1<<24);             // Disable PLL
	while(RCC->CR & (1<<25))        // Wait until PLL is unlocked
		;
	
	//PLL: HSE/M * N/P = 8MHz/8 * 168/2 = 84mhz
	RCC->PLLCFGR = (8 << 0) |       // PLLM = 8 
                   (168 << 6) |     // PLLN = 168 
                   (0 << 16) |      // PLLP = 2 
                   (1 << 22);       // PLLSRC = HSE 

	RCC->CR |= (1 << 24);           // PLL ON
	while(!(RCC->CR & (1 << 25)));  // Wait until PLL is ready	

	RCC->CFGR&=~(15U<<4);           // AHB prescaler = 1, 168MHz
	RCC->CFGR|=(4U<<10);            // APB1 prescaler = 2, 84MHz/2 = 42MHz (max)
	RCC->CFGR&=~(7U<<13);           // APB2 prescaler = 1, 84MHz

	// Select PLL as system clock
	RCC->CFGR |= (2 << 0);          // SW = PLL
	while((RCC->CFGR & (3 << 2)) != (2 << 2))  // Wait until PLL is system clock
		;
	
	// Update system core clock variable
	SystemCoreClockUpdate();
}
void gpioConfig()
{
		RCC->AHB1ENR |= (1 << 1);   
    //PB6=SCL, PB7=SDA
    GPIOB->MODER |= (2 << 12) | (2 << 14);    //af function mode
    GPIOB->OTYPER |= (1 << 6) | (1 << 7);     //open drain
    GPIOB->OSPEEDR |= (3 << 12) | (3 << 14);  //high speed
    GPIOB->PUPDR |= (1 << 12) | (1 << 14);    //pull-up
    GPIOB->AFR[0] |= (4 << 24) | (4 << 28);   //AF4 for I2C1
}

void i2cConfig()
{
	//enable I2C CLOCK
	RCC->APB1ENR |= (1<<21);
	//reset the I2C
	I2C1->CR1 |= (1<<15); 
	//normal operation
	I2C1->CR1 &= ~(1<<15); 
	//i2c disbale
	I2C1->CR1 &=~(1<<0);
	//PCLK1 FREQUENCY in MHz
	I2C1->CR2|=(42<<0);
	//configure the clock control registers
	I2C1->CCR|=(225<<0);
	//configure the rise time register
	I2C1->TRISE|=43;
	//enable I2C
	I2C1->CR1 |= (1<<0);  
}

void i2cStart()
{
	//enable the ACK
	I2C1->CR1 |= (1<<10); 
	//generate START
	I2C1->CR1 |= (1<<8); 
	//wait for SB bit to set
  while (!(I2C1->SR1 & (1<<0)));
}

void i2cStop()
{
    //generate STOP
    I2C1->CR1 |= (1<<9);  
}

void i2cWrite(uint8_t data)
{
	//wait for TXE bit to set
	while(!(I2C1->SR1 & (1<<7)));
	I2C1->DR=data;
	//wait for BTF bit to set
	while (!(I2C1->SR1 & (1<<2))); 
}

void i2cSendAddress(uint8_t address)
{
	//send the address
	I2C1->DR=address;
	//wait for ADDR bit to set
	while (!(I2C1->SR1 & (1<<1))); 
	//read SR1 and SR2 to clear the ADDR bit
	uint8_t temp = I2C1->SR1 | I2C1->SR2;
}

void oledSendCommand(uint8_t command)
{
    i2cStart();
    i2cSendAddress(0x78);  // SSD1306 write address
    i2cWrite(0x00);        //command mode
    i2cWrite(command);
    i2cStop();
}

void oledSendData(uint8_t data)
{
    i2cStart();
    i2cSendAddress(0x78);  // SSD1306 write address
    i2cWrite(0x40);        //data mode
    i2cWrite(data);
    i2cStop();
}

void oledInit()
{
    delay(100000);  // Power on delay
    
    oledSendCommand(0xAE);  // Display off
    oledSendCommand(0x20);  // Set memory addressing mode
    oledSendCommand(0x00);  // Horizontal addressing mode
    oledSendCommand(0xB0);  // Set page start address
    oledSendCommand(0xC8);  // Set COM output scan direction
    oledSendCommand(0x00);  // Set low column address
    oledSendCommand(0x10);  // Set high column address
    oledSendCommand(0x40);  // Set start line address
    oledSendCommand(0x81);  // Set contrast control register
    oledSendCommand(0xFF);  // Set contrast
    oledSendCommand(0xA1);  // Set segment re-map
    oledSendCommand(0xA6);  // Set normal display
    oledSendCommand(0xA8);  // Set multiplex ratio
    oledSendCommand(0x3F);  // 64 lines
    oledSendCommand(0xA4);  // Output follows RAM content
    oledSendCommand(0xD3);  // Set display offset
    oledSendCommand(0x00);  // No offset
    oledSendCommand(0xD5);  // Set display clock divide ratio
    oledSendCommand(0xF0);  // Set divide ratio
    oledSendCommand(0xD9);  // Set pre-charge period
    oledSendCommand(0x22);  // Set pre-charge period
    oledSendCommand(0xDA);  // Set COM pins hardware configuration
    oledSendCommand(0x12);  // Set COM pins configuration
    oledSendCommand(0xDB);  // Set VCOMH
    oledSendCommand(0x20);  // Set VCOMH deselect level
    oledSendCommand(0x8D);  // Set DC-DC enable
    oledSendCommand(0x14);  // Enable charge pump
    oledSendCommand(0xAF);  // Turn on OLED display
}

void oledClear()
{
    for(int page = 0; page < 8; page++)
    {
        oledSendCommand(0xB0 + page);  //set page address
        oledSendCommand(0x00);         //set low column address
        oledSendCommand(0x10);         //set high column address
        
        for(int col = 0; col < 128; col++)
        {
            oledSendData(0x00);
        }
    }
}

const uint8_t font5x7[][5] = {
    //0-9
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    
    //A-Z
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43},  // Z
		
		
		//'a'–'z'
		{0x20, 0x54, 0x54, 0x54, 0x78}, // a
		{0x7F, 0x48, 0x44, 0x44, 0x38}, // b
		{0x38, 0x44, 0x44, 0x44, 0x20}, // c
		{0x38, 0x44, 0x44, 0x48, 0x7F}, // d
		{0x38, 0x54, 0x54, 0x54, 0x18}, // e
		{0x08, 0x7E, 0x09, 0x01, 0x02}, // f
		{0x0C, 0x52, 0x52, 0x52, 0x3E}, // g
		{0x7F, 0x08, 0x04, 0x04, 0x78}, // h
		{0x00, 0x44, 0x7D, 0x40, 0x00}, // i
		{0x20, 0x40, 0x44, 0x3D, 0x00}, // j
		{0x7F, 0x10, 0x28, 0x44, 0x00}, // k
		{0x00, 0x41, 0x7F, 0x40, 0x00}, // l
		{0x7C, 0x04, 0x18, 0x04, 0x78}, // m
		{0x7C, 0x08, 0x04, 0x04, 0x78}, // n
		{0x38, 0x44, 0x44, 0x44, 0x38}, // o
		{0x7C, 0x14, 0x14, 0x14, 0x08}, // p
		{0x08, 0x14, 0x14, 0x18, 0x7C}, // q
		{0x7C, 0x08, 0x04, 0x04, 0x08}, // r
		{0x48, 0x54, 0x54, 0x54, 0x20}, // s
		{0x04, 0x3F, 0x44, 0x40, 0x20}, // t
		{0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
		{0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
		{0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
		{0x44, 0x28, 0x10, 0x28, 0x44}, // x
		{0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
		{0x44, 0x64, 0x54, 0x4C, 0x44}, // z
};



void oledSetCursor(uint8_t page, uint8_t col)
{
    oledSendCommand(0xB0 + page);        //set page address
    oledSendCommand(0x00 + (col & 0x0F)); //set low column address
    oledSendCommand(0x10 + (col >> 4));   //set high column address
}

void oledWriteChar(char c)
{
    uint8_t index;

    if (c >= '0' && c <= '9') {
        index = c - '0';
    } else if (c == ':') {
        index = 10;
    } else if (c >= 'A' && c <= 'Z') {
        index = 11 + (c - 'A');
		} else if (c >= 'a' && c <= 'z') {
        index = 37 + (c - 'a');
    } else {
        return; 
    }

    for (int i = 0; i < 5; i++) {
        oledSendData(font5x7[index][i]);
    }
    oledSendData(0x00);
}


void oledWriteString(char* str)
{
    while (*str) {
        oledWriteChar(*str++);
    }
}

void oledWriteNumber(uint16_t num)
{
    char buffer[5];

    buffer[0] = (num / 100) + '0';
    buffer[1] = ((num / 10) % 10) + '0';
    buffer[2] = (num % 10) + '0';
    buffer[3] = '\0';

    oledWriteString(buffer);
}

void oledTypewriterEffect(char* name)
{
	
			uint8_t page = 2;  
			uint8_t col = 0; 
			uint8_t charWidth = 6; 
	
			for (int i = 0; name[i] != '\0'; i++)
			{
					oledSetCursor(page, col);
					col+=charWidth;
					if(col>=125)
					{
						col=0;
						page++;
					}
					oledWriteChar(name[i]);
					delay(5000000);         
			}
}


void delay(uint32_t time)
{
    while(time--);
}