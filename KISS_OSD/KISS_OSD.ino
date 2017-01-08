/* 1. Information
//=========================================================================================================================

KISS FC OSD
by Samuel Daurat (sdaurat@outlook.de)
based on the code by Felix Niessen (felix.niessen@googlemail.com)
*/
# define OSDVersion "5.3.2"
#define DMemoryVersion 5
/*
*****************************************************************************************************
If you like my work and want to support me, I would love to get some support:  https://paypal.me/SamuelDaurat

Wenn Ihr meine Arbeit mögt, würde ich mich über etwas Support freuen: https://paypal.me/SamuelDaurat
*****************************************************************************************************

GITHUB: https://github.com/SAMUD/KISS-OSD


Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES ORnOTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/




// 2. USER CONFIGURATION
//=========================================================================================================================

//uncomment the line | remove the "//" in front of the line to activate it.
//um eine Linie zu aktivieren bitte das "//" entfernen

//Please refer to the github wiki page for explanations on the settings
//https://github.com/SAMUD/KISS-OSD/wiki


//video system
#define PAL
//#define NTSC

// Pilot-name
const char Pilotname[] = " SAMU";

// MAX7456 Charset
#define USE_MAX7456_ASCII
//#define USE_MAX7456_MAXIM



// END OF CONFIGURATION
//=========================================================================================================================
//START OF PROGRAMM

#include <SPI.h>
#include <MAX7456.h>
#include "symbols.h"
#include <EEPROMex.h>
#include "GlobalVar.h"
#include <avr/wdt.h>


//==============
//SETUP function
void setup() {

	//turn on watchdog timer
	wdt_enable(WDTO_8S);
	wdt_reset();


	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);

#if defined(PAL)
	OSD.begin(28, 15, 0);
	OSD.setTextOffset(-1, -6);
	OSD.setDefaultSystem(MAX7456_PAL);
#endif
#if defined(NTSC)
	OSD.begin(MAX7456_COLS_N1, MAX7456_ROWS_N0);
	OSD.setDefaultSystem(MAX7456_NTSC);
#endif

	OSD.setSwitchingTime(4);

#if defined(USE_MAX7456_ASCII)
	OSD.setCharEncoding(MAX7456_ASCII);
#endif
#if defined(USE_MAX7456_MAXIM)
	OSD.setCharEncoding(MAX7456_MAXIM);
#endif

	OSD.display();
	
	//clean used area
	uint8_t i = 0;
	for (i = 0; i<30; i++) clean[i] = ' ';
	while (!OSD.notInVSync());
	for (i = 0; i<20; i++)
	{
		OSD.setCursor(0, i);
		OSD.print(clean);
	}

	//set blinktime
	OSD.setBlinkingTime(2); //0-3
	OSD.setBlinkingDuty(1); //0-3

	Serial.begin(115200);

	//init memory
	EEPROMinit();

	//set the Offset
	OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);
}



// Main-Loop
//==========
void loop()
{

	//big if with all code
	if (micros() - LastLoopTime > 20000) //limits the speed of the OSD to 20Hz
	{
		LastLoopTime = micros();

		getSerialData();

		//open menu if yaw left and disarmed
		if (armed == 0 && StickChanVals[3]>500)
		{
			menumain();
			OSD.clear();
			EEPROMsave();
			delay(1000);
		}

		//calculate the datas to display
		CalculateOSD();

		//Display the datas
		DisplayOSD();

		//Reset wdt
		wdt_reset();
	}

}
