// TODO Add a sanity data-check after receiving data --> ex LipoVoltage<100V and >1V




/* 1. Information
//=========================================================================================================================

KISS FC OSD
by Samuel Daurat (sdaurat@outlook.de)
based on the code by Felix Niessen (felix.niessen@googlemail.com)
*/

#define OSDVersion "1.3RC35"
#define DMemoryVersion 12
//#define DEBUG
#define STEELE_PDB
/*
***************************************************************************************************************************************************
Donations help A LOT during development, buying me a COFFE you will keep me awake at night so I can add more stuff:  https://paypal.me/SamuelDaurat

Spenden helfen eine MENGE beim Programmieren. Mir einen KAFFE zu bezahlen, hilft mir nachts l�nger wach zu bleiben
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



// MAX7456 Charset
#define USE_MAX7456_ASCII
//#define USE_MAX7456_MAXIM



// END OF CONFIGURATION
//=========================================================================================================================
//START OF PROGRAMM


//#include <SerialPort.h>
#include <SPI.h>
#include <MAX7456.h>
#include "symbols.h"
#include <EEPROMex.h>
#include "GlobalVar.h"


//SerialPort<0, 63, 0> NewSerial;

//==============
//SETUP function
void setup() {

	//start SPI
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);

	//init memory
	EEPROMinit();

#if (defined(IMPULSERC_VTX) || defined(STEELE_PDB))
	MAX7456Setup();
	delay(100);
#endif

	if (Settings.VideoMode == 1)
	{
		OSD.begin(28, 13 + Settings.LineAddition, 0);
		OSD.setDefaultSystem(MAX7456_PAL);
	}
	else
	{
		OSD.begin(MAX7456_COLS_N1, 12 + Settings.LineAddition);
		OSD.setDefaultSystem(MAX7456_NTSC);
	}



	OSD.setSwitchingTime(5);					//lower value will make text a little bit sharper

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

	KissConnection = ConnectionEtablished;
}



// Main-Loop
//==========
void loop()
{

	//big if with all code
	if (micros() - KissStatus.LastLoopTime > 10000)
	{
		KissStatus.LastLoopTime = micros();			//saving current time

		if (getSerialData(GET_TELEMETRY, true))			//requesting serial data
		{
			//received data is ok
			KissStatus.GetTelemetryErrorCount = 0;
		}
		else
		{
			//show error if we had more than 5 consecutive receive errors
			KissStatus.GetTelemetryErrorCount = +1;
			if (KissStatus.GetTelemetryErrorCount > 5)
				KissConnection = LostConnection;
		}

		switch (KissConnection)
		{
		case LostConnection:
			WaitForKissFc();
			break;
		case ConnectionEtablished:
			OSD.clear();
			KissConnection = Connected;
			KissStatus.lastMode = KissStatus.reducedModeDisplay;
			break;
		case Connected:

			KissStatus.SerialErrorReason = 0;
			if (KissStatus.MenuBlockiterations > 0)
				KissStatus.MenuBlockiterations = KissStatus.MenuBlockiterations-1;

			//open menu right if yaw right and disarmed
			if (!KissTelemetrie.armed && KissTelemetrie.StickChanVals[3] > 800 && KissStatus.MenuBlockiterations == 0)
				MenuRight_Main();

			//open menu left if yaw left and disarmed
			if (!KissTelemetrie.armed && KissTelemetrie.StickChanVals[3] < -800 && KissStatus.MenuBlockiterations == 0)
				MenuLeft_Main();

			//calculate the datas
			CalculateOSD();
			FlightSummaryCalculate();

			if (KissTelemetrie.armed == 0 && KissStatus.time > (TIMEOUT_FOR_SUMMARY_SEC * 1000) && KissStatus.BatteryCells>0)	//if disarmed and flighttime>45sec --> show flight summary
			{
				FlightSummary();
				SaveBatStatus();
			}
			else if (KissTelemetrie.armed == 0 &&
				Settings.SavedCurrBat.BatteryMAH < ((Settings.Capacity * (float)Settings.Capacity1st) / 100) &&
				KissStatus.time == 0 &&
				KissTelemetrie.LipoMAH < 10 &&
				Settings.SavedCurrBat.BatteryMAH>0 &&
				Settings.Capacity>0)
				//able to recover an old battery
				DisplayRecover();
			else
			{
				SaveBatStatus();
				DisplayOSD_Main();								//Display the datas
			}


			break;
		}
		//Reset wdt
		//wdt_reset();
		if (OSD.videoSystem() != Settings.VideoMode && Settings.stockSettings == 0)
		{
			if (OSD.videoSystem() == 2)
				Settings.VideoMode = 2;	//Setting to NTSC
			else
				Settings.VideoMode = 1; //Setting to PAL
			EEPROMsave();
		}

	}

}


