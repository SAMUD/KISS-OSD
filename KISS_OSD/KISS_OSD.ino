/* 1. Information
//=========================================================================================================================

KISS FC OSD
by Samuel Daurat (sdaurat@outlook.de)
based on the code by Felix Niessen (felix.niessen@googlemail.com)
*/
const char OSDVersion[]="     V 4.5.1     ";
/*
*****************************************************************************************************
If you like my work and want to support me, I would love to get some support:  paypal.me/SamuelDaurat

Wenn Ihr meine Arbeit mögt, würde ich mich über etwas Support freuen: https://paypal.me/SamuelDaurat
*****************************************************************************************************

GITHUB: https://github.com/SAMUD/KISS-OSD

Changelog:
*added a space between the clock and the current consumption to avoid having constellations like 1:251236mah
*added 2nd stage mah alarm

TODO:
*adding stats at the end of flight
*adding virtual horizon (Milestone1 achieved)
*adding flight mode indicator


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




// 2. CONFIGURATION
//=========================================================================================================================

//uncomment the line | remove the "//" in front of the line to activate it.
//um eine Linie zu aktivieren bitte das "//" entfernen

//Please refer to the github wiki page for explanations on the settings
//https://github.com/SAMUD/KISS-OSD/wiki/OSD-Settings-explained


//video system
#define PAL
//#define NTSC

// Pilot-name
const char Pilotname[]=" SAMU";

// MAX7456 Charset
#define USE_MAX7456_ASCII
//#define USE_MAX7456_MAXIM

// motors magnepole count
#define MAGNETPOLECOUNT 14 // 2 for ERPMs

// Filter for ESC datas
#define ESC_FILTER 10

// reduced mode channel config
#define RED_MODE_AUX_CHAN 4 // 0-4, 0 = none

// displayed datas in normal mode
//#define DISPLAY_RC_THROTTLE
#define DISPLAY_COMB_CURRENT
#define DISPLAY_LIPO_VOLTAGE
#define DISPLAY_MA_CONSUMPTION
//#define DISPLAY_ESC_KRPM
//#define DISPLAY_ESC_CURRENT
//#define DISPLAY_ESC_TEMPERATURE
#define DISPLAY_PILOTNAME
#define DISPLAY_TIMER
#define DISPLAY_ANGLE

// displayed datas in reduced mode 1 
//#define RED_DISPLAY_RC_THROTTLE
//#define RED_DISPLAY_COMB_CURRENT
#define RED_DISPLAY_LIPO_VOLTAGE
#define RED_DISPLAY_MA_CONSUMPTION
//#define RED_DISPLAY_ESC_KRPM
//#define RED_DISPLAY_ESC_CURRENT
//#define RED_DISPLAY_ESC_TEMPERATURE
//#define RED_DISPLAY_PILOTNAME
//#define RED_DISPLAY_TIMER
//#define RED_DISPLAY_ANGLE

// displayed datas in reduced mode 2
//#define RED2_DISPLAY_RC_THROTTLE
#define RED2_DISPLAY_COMB_CURRENT
#define RED2_DISPLAY_LIPO_VOLTAGE
#define RED2_DISPLAY_MA_CONSUMPTION
#define RED2_DISPLAY_ESC_KRPM
#define RED2_DISPLAY_ESC_CURRENT
#define RED2_DISPLAY_ESC_TEMPERATURE
#define RED2_DISPLAY_PILOTNAME
#define RED2_DISPLAY_TIMER
#define RED2_DISPLAY_ANGLE

//margin left and right for the last line.
const uint8_t marginLastRow=3;

//Voltage Settings
const uint16_t LowVoltage3s=1050;
const uint16_t LowVoltage4s=1410;
const uint16_t SeparationVoltage3s4s=1370;
const uint16_t hysteresis=30;
const uint16_t MinimalCellVoltage2nd=320;
const int8_t VoltageOffset=-10;
const char threeSBatteryDetected[]="3S BAT - CRIT@ 10.5V";
const char fourSBatteryDetected[]=" 4S BAT - CRIT@ 14.1V ";

//Capacity settings
const uint16_t CapacityThreshold=1050;
const uint16_t CapacityThreshold2ndStage=1200;





// END OF CONFIGURATION
//=========================================================================================================================
//START OF PROGRAMM

#include <SPI.h>
#include <MAX7456.h>
#include "symbols.h"

const byte osdChipSelect              =             6;
const byte masterOutSlaveIn           =             MOSI;
const byte masterInSlaveOut           =             MISO;
const byte slaveClock                 =             SCK;
const byte osdReset                   =             2;

MAX7456 OSD( osdChipSelect );

static char clean[30];

uint8_t firstloop                     =             0;
uint8_t BatteryCells                  =             0;		//stores the number of cells recognized in the first run
boolean VoltageAlarm                  =             false;	//works with the const defined in the beginning | Filters Voltage drops to avoid erratic voltage alarms
boolean VoltageAlarm2nd               =             false;	//2nd stage of voltage alarms

static uint16_t current               =             0;
static int16_t LipoVoltage            =             0;
static uint16_t LipoMAH               =             0;
static uint16_t motorKERPM[4]         =             {0,0,0,0};
static uint16_t motorCurrent[4]       =             {0,0,0,0};
static uint16_t ESCTemps[4]           =             {0,0,0,0};
static int16_t  AuxChanVals[4]        =             {0,0,0,0};
static int16_t  StickChanVals[4]	  =				{0,0,0,0};
static uint8_t  reducedMode           =             0;
static uint8_t  reducedMode2          =             0;
static uint8_t  reducedModeDisplay    =             0;
static uint8_t armed                  =             0;
static uint8_t armedOld               =             0;
static uint8_t failsafe               =             0;
static uint16_t calibGyroDone         =             0;

static int16_t angley                 =             0;

static unsigned long start_time       =             0;
static unsigned long time             =             0;
static unsigned long total_time       =             0;

static uint8_t percent                =             0;
static uint8_t firstarmed             =             0;

static unsigned long armedstarted     =             0;

static uint8_t extra_space_mah        =             0;

uint16_t i							  =				0;
uint8_t KRPMPoses[4];
static uint8_t lastMode				  =				0;

static char Motor1KERPM[30];
static char Motor2KERPM[30];
static char Motor3KERPM[30];
static char Motor4KERPM[30];

uint8_t CurrentPoses[4];
static char Motor1Current[30];
static char Motor2Current[30];
static char Motor3Current[30];
static char Motor4Current[30];

uint8_t TempPoses[4];
static char ESC1Temp[30];
static char ESC2Temp[30];
static char ESC3Temp[30];
static char ESC4Temp[30];

static char LipoVoltC[30];
static char LipoMAHC[30];

static char Throttle[30];
static char Current[30];

static char Time[10];

static uint32_t LastLoopTime;

uint32_t tmpVoltage					  =				0;
uint32_t voltDev					  =				0;

static uint8_t ThrottlePos;
static uint8_t CurrentPos;

//Var für Menu
//breite max 18 char
char* MenuPage1[] = { "CAPACITY FIRST ALARM", "CAPACITY 80% BAT", "VOLTAGE ALARM 3S", "VOLTAGE ALARM 4S", "VOLTAGE CRITICAL /CELL" , "EXIT        ",
"PLACEHOLDER3", "PLACEHOLDER4","PLACEHOLDER5", "PLACEHOLDER6","PLACEHOLDER7" };
int16_t ValuePage1[11] = { CapacityThreshold,CapacityThreshold2ndStage,LowVoltage3s,LowVoltage4s,MinimalCellVoltage2nd,0,3,4,5,6,7 };



//==============
//SETUP function
void setup(){
  uint8_t i = 0;
  SPI.begin();
  SPI.setClockDivider( SPI_CLOCK_DIV2 );
  #if defined(PAL)
    OSD.begin(28,15,0);
    OSD.setTextOffset(-1,-6);
    OSD.setDefaultSystem(MAX7456_PAL);
  #endif
  #if defined(NTSC)
    OSD.begin(MAX7456_COLS_N1,MAX7456_ROWS_N0);
    OSD.setDefaultSystem(MAX7456_NTSC);
  #endif
  OSD.setSwitchingTime( 4 );
  #if defined(USE_MAX7456_ASCII)
    OSD.setCharEncoding( MAX7456_ASCII );
  #endif
  #if defined(USE_MAX7456_MAXIM)
    OSD.setCharEncoding( MAX7456_MAXIM );
  #endif
  OSD.display();

  //clean used area
  for(i=0;i<30;i++) clean[i] = ' ';
  while (!OSD.notInVSync());
  for(i=0;i<20;i++)
  {
      OSD.setCursor( 0, i );
      OSD.print( clean );
  }

  //set blinktime
  OSD.setBlinkingTime(2); //0-3
  OSD.setBlinkingDuty(1); //0-3

  Serial.begin(115200);
}



// Main-Loop
//==========
void loop()
{
  
  //big if with all code
  if(micros()-LastLoopTime > 10000) //limits the speed of the OSD to 100Hz
  {
    LastLoopTime = micros();

    getSerialData();

	//open menu if yaw left and disarmed
	if(armed==0 && StickChanVals[3]>500)
	{
		menumain();
		OSD.clear();
		ValuePage1[5]=0;
	}
	
	//calculate the datas to display
	CalculateOSD();

	//Display the datas
    DisplayOSD();
  }

}
