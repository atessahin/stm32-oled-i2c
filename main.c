#include "config.h"

int main()
{
    systemClockConfig();
    gpioConfig();
    i2cConfig();

    oledInit();
    oledClear();
		

  while (1)
	{
		
	   //oledWriteNumber(888);
		
		 //oledTypewriterEffect("METE HAN");
		
		 delay(50000000);   
	}
}
