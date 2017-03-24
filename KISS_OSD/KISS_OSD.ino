// TODO Add a sanity data-check after receiving data --> ex LipoVoltage<100V and >1V




/* 1. Information
//=========================================================================================================================

KISS FC OSD
by Samuel Daurat (sdaurat@outlook.de)
based on the code by Felix Niessen (felix.niessen@googlemail.com)
*/

#define OSDVersion "6.92"
#define DMemoryVersion 6
//#define IMPULSERC_VTX
//#define DEBUG
/*
***************************************************************************************************************************************************
Donations help A LOT during development, buying me a COFFE you will keep me awake at night so I can add more stuff:  https://paypal.me/SamuelDaurat

Spenden helfen eine MENGE beim Programmieren. Mir einen KAFFE zu bezahlen, hilft mir nachts länger wach zu bleiben
und produktiver zu programmieren: https://paypal.me/SamuelDaurat
***************************************************************************************************************************************************

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
const char Pilotname[] = "SAMUD";

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
//#include <avr/wdt.h>



//==============
//SETUP function
void setup() {

	//SerialPort<0, 64, 0> NewSerial;
	
	
	//turn on watchdog timer
  //wdt_enable(WDTO_8S);
  //wdt_reset();

  //start SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);

#if defined(PAL)
  OSD.begin(28, 15, 0);
  OSD.setDefaultSystem(MAX7456_PAL);
#endif
#if defined(NTSC)
  OSD.begin(MAX7456_COLS_N1, MAX7456_ROWS_N0);
  OSD.setDefaultSystem(MAX7456_NTSC);
#endif

  OSD.setSwitchingTime(0);					//lower value will make text a little bit sharper

#if defined(USE_MAX7456_ASCII)
  OSD.setCharEncoding(MAX7456_ASCII);
#endif
#if defined(USE_MAX7456_MAXIM)
  OSD.setCharEncoding(MAX7456_MAXIM);
#endif

  OSD.display();							//enable OSD output
  while (!OSD.notInVSync());
  OSD.clear();
  while (OSD.clearIsBusy());
  OSD.setBlinkingTime(2); //0-3				//set blinktime
  OSD.setBlinkingDuty(1); //0-3
  OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);  //set the Offset
 
  Serial.begin(115200);

  //init memory
  EEPROMinit();

  KissConnection = WaitingForConn;
}



// Main-Loop
//==========
void loop()
{

  //big if with all code
  if (micros() - KissStatus.LastLoopTime > 100000) //limits the speed of the OSD to 10Hz
  {
	  KissStatus.LastLoopTime = micros();			//saving current time

    getSerialData(GET_TELEMETRY,true);					//requesting serial data
	
	switch (KissConnection)
	{
	case WaitingForConn:
		WaitForKissFc();
		break;
	case LostConnection:
		WaitForKissFc();
		break;
	case ConnectionEtablished:
		OSD.clear();
		KissConnection = Connected;
		break;
	case Connected:
		
		//open menu right if yaw right and disarmed
		if (!KissTelemetrie.armed && KissTelemetrie.StickChanVals[3] > 800)
			MenuRight_Main();

		//open menu left if yaw left and disarmed
		if (!KissTelemetrie.armed && KissTelemetrie.StickChanVals[3] < -800)
			MenuLeft_Main();
		
		//calculate the datas
		CalculateOSD();
		FlightSummaryCalculate();

		if (KissTelemetrie.armed==0 && KissStatus.time > (TIMEOUT_FOR_SUMMARY_SEC * 1000))	//if disarmed and flighttime>45sec --> show flight summary
			FlightSummary();
		else
			DisplayOSD_Main();								//Display the datas
		break;
	default:
		KissConnection = WaitingForConn;				//When the enum gets in an unknown state - should not do normally
		#ifdef DEBUG
			Debug_Fnc("ENUM_KISCONE WRONG CASE");
		#endif
		break;
	}
	//Reset wdt
    //wdt_reset();
  }

}

