/*
KISS FC OSD v3
By Felix Niessen (felix.niessen@googlemail.com)
and Samuel Daurat (sdaurat@outlook.de)

GITHUB: https://github.com/SAMUD/KISS-OSD

Changelog:
*code optimizing
*added timer
*added middle-info "failsafe" ATTENTION: it is not working correctly. This means that after the first not failsafe-state, if there is a new failsafe it wont be shown. This seems to be a bug in the FC-firmware.


TODO:
*adding stats at the end of flight
*optimizing code
*adding comments to code

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/




// CONFIGURATION
//=========================================================================================================================

//uncomment the line | remove the "//" in front of the line to activate it.
//um eine Linie zu aktivieren bitte das "//" entfernen

// video system (only one uncommented)
//Videosystem (in Europa meistens PAL | bitte nur eine Auswahl aktivieren)
//========================================================================
#define PAL
//#define NTSC

// Pilot-name to be displayed (only small letters are allowed)
//Pilotenname (nur kleine Buchstaben erlaubt)
//TODO: große Buchstaben in Schriftart hinzufügen
//========================================================================
const char Pilotname[]="samuel";

// MAX7456 Charset (change if you get sensless signs)
//MAX7456 Charset (ändern wenn man kryptische Zeichen bekommt)
//============================================================
#define USE_MAX7456_ASCII
//#define USE_MAX7456_MAXIM

// motors magnepole count (to display the right RPMs)
//Anzahl der Motorpole um die richtige Drehzahl anzuzeigen
//========================================================
#define MAGNETPOLECOUNT 14 // 2 for ERPMs

// Filter for ESC datas (higher value makes them less erratic) 0 = no filter, 20 = very strong filter
//Filter für die ESC-Daten (20=starker Filter, 0=kein Filter)
//===================================================================================================
#define ESC_FILTER 10

// reduced mode channel config | setting this to 0 forces always the normal mode
//welcher Kanal von 1-4 für den OSD-Modus verwendet wird. | wenn 0 eingestellt wird, wird nur der normale modus angezeigt
//=======================================================
#define RED_MODE_AUX_CHAN 4 // 0-4, 0 = none
//HINWEIS:
//selected channel / Kanal > 500 --> reduced mode 2
//selected channel / Kanal between -500 and 500 --> normal mode
//selected channel / Kanal < -500 --> reduced mode 1



// displayed datas in normal mode (if you turn off "DISPLAY_LIPO_VOLTAGE" or "DISPLAY_MA_CONSUMPTION" you don't see the alarms)
//angezeigte Daten im normalen Modus (wenn man "DISPLAY_LIPO_VOLTAGE" oder "DISPLAY_MA_CONSUMPTION" ausschaltet, bekommt man keine Warnungen)
//===========================================================================================================================================
//#define DISPLAY_RC_THROTTLE
//#define DISPLAY_COMB_CURRENT
#define DISPLAY_LIPO_VOLTAGE
//#define DISPLAY_MA_CONSUMPTION
//#define DISPLAY_ESC_KRPM
//#define DISPLAY_ESC_CURRENT
//#define DISPLAY_ESC_TEMPERATURE
#define DISPLAY_PILOTNAME
#define DISPLAY_TIMER

// displayed datas in reduced mode 1 (if you turn off "DISPLAY_LIPO_VOLTAGE" or "DISPLAY_MA_CONSUMPTION" you don't see the alarms)
//das gleiche hier, nur für den reduzierten Modus
//==============================================================================================================================
//#define RED_DISPLAY_RC_THROTTLE
//#define RED_DISPLAY_COMB_CURRENT
#define RED_DISPLAY_LIPO_VOLTAGE
//#define RED_DISPLAY_MA_CONSUMPTION
//#define RED_DISPLAY_ESC_KRPM
//#define RED_DISPLAY_ESC_CURRENT
//#define RED_DISPLAY_ESC_TEMPERATURE
//#define RED_DISPLAY_PILOTNAME
//#define DISPLAY_TIMER

// displayed datas in reduced mode 2 (if you turn off "DISPLAY_LIPO_VOLTAGE" or "DISPLAY_MA_CONSUMPTION" you don't see the alarms)
//das gleiche hier, nur für den reduzierten Modus 2
//==============================================================================================================================
//#define RED2_DISPLAY_RC_THROTTLE
#define RED2_DISPLAY_COMB_CURRENT
#define RED2_DISPLAY_LIPO_VOLTAGE
#define RED2_DISPLAY_MA_CONSUMPTION
#define RED2_DISPLAY_ESC_KRPM
#define RED2_DISPLAY_ESC_CURRENT
#define RED2_DISPLAY_ESC_TEMPERATURE
#define RED2_DISPLAY_PILOTNAME
#define DISPLAY_TIMER





//margin left and right for the last line. Value can be something between 0 (no margin) and perhaps 10 as maximum (usefull if you can't read values in the corner of your FatShark)
//Wenn man in den Ecken unten links und rechts schlecht Werte lesen kann, kann man so die Werte weiter in die Mitte schieben (0=am Rand)
//=================================================================================================================================================================================
const uint8_t marginLastRow=3;

//Low voltage for triggering an Battery-Alarm (12,82V equals to 1282 as a stored value)
//Spannungen für den Batterie-Alarm (12,82V entspricht dabei 1282)
//=====================================================================================
const uint16_t LowVoltage3s=1050; //10,5V
const uint16_t LowVoltage4s=1400; //14,0V
//Separation between 3s and 4s
//Spannung mit welcher am Anfang 3s oder 4s erkannt wird
const uint16_t SeparationVoltage3s4s=1370; //13,7V
//hysteresis for the voltage Alarm (Alarm will turn off if voltage goes higher than LowVoltage?s+hysteresis)
//Hysteresis für die SPannungswarnung (Alarm wird ausgeschaltet wenn die Spannung höher wird als LowVoltage?s+hysteresis
const uint16_t hysteresis=30;

//Voltage-offset (will change the displayed Voltage and the alarm)
//Value can be anything between -127 and 127. Setting the Voltage works like above for the Voltage Alarm (127=1,27V)
//Spannungsoffset (wird auf die angezeigte Spannung und den Alarm angewendet)
//maximaler Wertebereich -127 bis 127 | Werte werden wie oben beim Batteriealarm eingestellt (127=1,27V)
//===========================================================================
const int8_t VoltageOffset=-10;

//Information for battery detection beeing displayed(only small letters are allowed) (maximum 20 letters allowed)
//angezeigte Informationen wenn Batterie erkannt wurde (nur kleine Buchstaben erlaubt) (maximal 20 Zeichen erlaubt)
//TODO: große Buchstaben in Schriftart hinzufügen
//========================================================================
const char threeSBatteryDetected[]="3s bat - crit@ 10.5V";
const char fourSBatteryDetected[]=" 4s bat - crit@ 14V ";

//Warning for used mah
//Warnung für KApazität in mah
//============================
//TODO: WARNING: this feature is not tested (I don't have the money for the 24A ESCs right now), but normally it should work
const uint16_t CapacityThreshold=1150;





// END OF CONFIGURATION
//=========================================================================================================================
//START OF PROGRAMM

#include <SPI.h>
#include <MAX7456.h>

const byte osdChipSelect             =            6;
const byte masterOutSlaveIn          =            MOSI;
const byte masterInSlaveOut          =            MISO;
const byte slaveClock                =            SCK;
const byte osdReset                  =            2;

MAX7456 OSD( osdChipSelect );


static char clean[30];

uint8_t firstloop=0;
uint8_t BatteryCells=0; //stores the number of cells recognized in the first run
boolean VoltageAlarm=false; //works with the const defined in the beginning | Filters Voltage drops to avoid erratic voltage alarms

static int16_t  throttle = 0;
static uint16_t current = 0;
static int16_t LipoVoltage = 0;
static uint16_t LipoMAH = 0;
static uint16_t motorKERPM[4] = {0,0,0,0};
static uint16_t motorCurrent[4] = {0,0,0,0};
static uint16_t ESCTemps[4] = {0,0,0,0};
static int16_t  AuxChanVals[4] = {0,0,0,0};
static uint8_t  reducedMode = 0;
static uint8_t  reducedMode2 = 0;
static uint8_t  reducedModeDisplay = 0;
static uint8_t armed=0;
static uint8_t armedOld=0;
static uint8_t failsafe=0;

static unsigned long start_time = 0;
static unsigned long time = 0;
static unsigned long total_time = 0;

static uint8_t percent=0;
static uint8_t firstarmed=0;

static unsigned long armedstarted=0;



//==============
//SETUP function

void setup(){
  uint8_t i = 0;
  SPI.begin();
  SPI.setClockDivider( SPI_CLOCK_DIV2 );
  //OSD.begin();
  #if defined(PAL)
    OSD.begin(28,15,0);
    OSD.setTextOffset(-1,-6);
    OSD.setDefaultSystem(MAX7456_PAL);
  #endif
  #if defined(NTSC)
    OSD.begin(MAX7456_COLS_N1,MAX7456_ROWS_N0);
    OSD.setDefaultSystem(MAX7456_NTSC);
  #endif
  OSD.setSwitchingTime( 5 );
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
  for(i=0;i<20;i++){
      OSD.setCursor( 0, i );
      OSD.print( clean );
  }

  //set blinktime
  OSD.setBlinkingTime(2); //0-3
  OSD.setBlinkingDuty(1); //0-3

  Serial.begin(115200);
}

// END OF SETUP function
//======================



//=====================
// fonction print_int16

uint8_t print_int16(int16_t p_int, char *str, uint8_t dec, uint8_t AlignLeft){
	uint16_t useVal = p_int;
	uint8_t pre = ' ';
	if(p_int < 0){
		useVal = p_int*-1;
		pre = '-';
	}
	uint8_t aciidig[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	uint8_t i = 0;
        uint8_t digits[6] = {0,0,0,0,0,0};
	while(useVal >= 10000){digits[0]++; useVal-=10000;}
	while(useVal >= 1000){digits[1]++; useVal-=1000;}
	while(useVal >= 100){digits[2]++; useVal-=100;}
	while(useVal >= 10){digits[3]++; useVal-=10;}
	digits[4] = useVal;
        char result[6] = {' ',' ',' ',' ',' ','0'};
	uint8_t signdone = 0;
	for(i = 0; i < 6;i++){
		if(i == 5 && signdone == 0) continue;
		else if(aciidig[digits[i]] != '0' && signdone == 0){
			result[i] = pre;
			signdone = 1;
		}else if(signdone) result[i] = aciidig[digits[i-1]];
	}
        uint8_t CharPos = 0;
        for(i = 0; i < 6;i++){
          if(result[i] != ' ' || (AlignLeft == 0 || (i > 5-dec))) str[CharPos++] = result[i];
          if(dec != 0 && i == 5-dec) str[CharPos++] = '.';
          if(dec != 0 && i > 5-dec && str[CharPos-1] == ' ') str[CharPos-1] = '0';
        }

        return CharPos;
}

// END OF print_int16 fonction
//============================


//===========
// ESC-Filter

uint32_t ESC_filter(uint32_t oldVal, uint32_t newVal){
  return (uint32_t)((uint32_t)((uint32_t)((uint32_t)oldVal*ESC_FILTER)+(uint32_t)newVal))/(ESC_FILTER+1);
}
// END OF ESC-Filter
//==================

//========================
//Convert time in a string
void print_time(unsigned long time, char *time_str) {
    uint16_t seconds = time / 1000;
    uint8_t mills = time % 1000;
    uint8_t minutes = 0;
    if (seconds >= 60)
    {
      minutes = seconds/60;
    }
    else
    {
      minutes = 0;
    }
    seconds = seconds % 60; // reste
    static char time_sec[6];
    static char time_mil[6];
    uint8_t i = 0;
    uint8_t time_pos = print_int16(minutes, time_str,0,1);
    time_str[time_pos++] = 'm';

    uint8_t sec_pos = print_int16(seconds, time_sec,0,1);
    for (i=0; i<sec_pos; i++)
    {
      time_str[time_pos++] = time_sec[i];
    }
    //time_str[time_pos++] = '.';

    //uint8_t mil_pos = print_int16(mills, time_mil,0,1);
    //time_str[time_pos++] = time_mil[0];
    for (i=time_pos; i<9; i++)
    {
      time_str[time_pos++] = ' ';
    }
}
//================


// Main-Loop
//==========

void loop(){
  uint16_t i = 0;
  uint8_t KRPMPoses[4];
  static uint8_t lastMode = 0;

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

  static uint8_t serialBuf[255];
  static uint8_t minBytes = 0;
  static uint8_t recBytes = 0;

  static char Time[10];

  static uint32_t LastLoopTime = 0;

  //big if with all code
  if(micros()-LastLoopTime > 10000){
    LastLoopTime = micros();

    Serial.write(0x20); // request telemetrie

    minBytes = 100;
    recBytes = 0;
    //aquire serial data and write it to normal variables
    while(recBytes < minBytes && micros()-LastLoopTime < 20000)
    {
      #define STARTCOUNT 2
      while(Serial.available()) serialBuf[recBytes++] = Serial.read();
      if(recBytes == 1 && serialBuf[0] != 5)recBytes = 0; // check for start byte, reset if its wrong
      if(recBytes == 2) minBytes = serialBuf[1]+STARTCOUNT+1; // got the transmission length
      if(recBytes == minBytes)
      {
         uint32_t checksum = 0;
         for(i=2;i<minBytes;i++)
         {
            checksum += serialBuf[i];
         }
         checksum = (uint32_t)checksum/(minBytes-3);

         if(checksum == serialBuf[recBytes-1])
         {

           throttle = ((serialBuf[STARTCOUNT]<<8) | serialBuf[1+STARTCOUNT])/10;
           armed =   ((serialBuf[15+STARTCOUNT]<<8) | serialBuf[16+STARTCOUNT]);
           LipoVoltage =   ((serialBuf[17+STARTCOUNT]<<8) | serialBuf[18+STARTCOUNT]);
           if (serialBuf[41+STARTCOUNT] > 0)
           {
               failsafe++;
           }
           else
           {
               failsafe=0;
           }

           uint32_t tmpVoltage = 0;
           uint32_t voltDev = 0;
           if(((serialBuf[85+STARTCOUNT]<<8) | serialBuf[86+STARTCOUNT]) > 5) // the ESC's read the voltage better then the FC
           {
             tmpVoltage += ((serialBuf[85+STARTCOUNT]<<8) | serialBuf[86+STARTCOUNT]);
             voltDev++;
           }
           if(((serialBuf[95+STARTCOUNT]<<8) | serialBuf[96+STARTCOUNT]) > 5)
           {
             tmpVoltage += ((serialBuf[95+STARTCOUNT]<<8) | serialBuf[96+STARTCOUNT]);
             voltDev++;
           }
           if(((serialBuf[105+STARTCOUNT]<<8) | serialBuf[106+STARTCOUNT]) > 5)
           {
             tmpVoltage += ((serialBuf[105+STARTCOUNT]<<8) | serialBuf[106+STARTCOUNT]);
             voltDev++;
           }
           if(((serialBuf[115+STARTCOUNT]<<8) | serialBuf[116+STARTCOUNT]) > 5)
           {
             tmpVoltage += ((serialBuf[115+STARTCOUNT]<<8) | serialBuf[116+STARTCOUNT]);
             voltDev++;
           }
           if(((serialBuf[125+STARTCOUNT]<<8) | serialBuf[126+STARTCOUNT]) > 5)
           {
             tmpVoltage += ((serialBuf[125+STARTCOUNT]<<8) | serialBuf[126+STARTCOUNT]);
             voltDev++;
           }
           if(((serialBuf[125+STARTCOUNT]<<8) | serialBuf[126+STARTCOUNT]) > 5)
           {
             tmpVoltage += ((serialBuf[125+STARTCOUNT]<<8) | serialBuf[126+STARTCOUNT]);
             voltDev++;
           }

           if(voltDev!=0) LipoVoltage = tmpVoltage/voltDev;
           LipoVoltage=LipoVoltage+VoltageOffset;


           LipoMAH =       ((serialBuf[148+STARTCOUNT]<<8) | serialBuf[149+STARTCOUNT]);

           static uint32_t windedupfilterdatas[8];

           windedupfilterdatas[0] = ESC_filter((uint32_t)windedupfilterdatas[0],(uint32_t)((serialBuf[91+STARTCOUNT]<<8) | serialBuf[92+STARTCOUNT])/(MAGNETPOLECOUNT/2)<<4);
           windedupfilterdatas[1] = ESC_filter((uint32_t)windedupfilterdatas[1],(uint32_t)((serialBuf[101+STARTCOUNT]<<8) | serialBuf[102+STARTCOUNT])/(MAGNETPOLECOUNT/2)<<4);
           windedupfilterdatas[2] = ESC_filter((uint32_t)windedupfilterdatas[2],(uint32_t)((serialBuf[111+STARTCOUNT]<<8) | serialBuf[112+STARTCOUNT])/(MAGNETPOLECOUNT/2)<<4);
           windedupfilterdatas[3] = ESC_filter((uint32_t)windedupfilterdatas[3],(uint32_t)((serialBuf[121+STARTCOUNT]<<8) | serialBuf[122+STARTCOUNT])/(MAGNETPOLECOUNT/2)<<4);

           motorKERPM[0] = windedupfilterdatas[0]>>4;
           motorKERPM[1] = windedupfilterdatas[1]>>4;
           motorKERPM[2] = windedupfilterdatas[2]>>4;
           motorKERPM[3] = windedupfilterdatas[3]>>4;


           windedupfilterdatas[4] = ESC_filter((uint32_t)windedupfilterdatas[4],(uint32_t)((serialBuf[87+STARTCOUNT]<<8) | serialBuf[88+STARTCOUNT])<<4);
           windedupfilterdatas[5] = ESC_filter((uint32_t)windedupfilterdatas[5],(uint32_t)((serialBuf[97+STARTCOUNT]<<8) | serialBuf[98+STARTCOUNT])<<4);
           windedupfilterdatas[6] = ESC_filter((uint32_t)windedupfilterdatas[6],(uint32_t)((serialBuf[107+STARTCOUNT]<<8) | serialBuf[108+STARTCOUNT])<<4);
           windedupfilterdatas[7] = ESC_filter((uint32_t)windedupfilterdatas[7],(uint32_t)((serialBuf[117+STARTCOUNT]<<8) | serialBuf[118+STARTCOUNT])<<4);

           motorCurrent[0] = windedupfilterdatas[4]>>4;
           motorCurrent[1] = windedupfilterdatas[5]>>4;
           motorCurrent[2] = windedupfilterdatas[6]>>4;
           motorCurrent[3] = windedupfilterdatas[7]>>4;


           ESCTemps[0] = ((serialBuf[83+STARTCOUNT]<<8) | serialBuf[84+STARTCOUNT]);
           ESCTemps[1] = ((serialBuf[93+STARTCOUNT]<<8) | serialBuf[94+STARTCOUNT]);
           ESCTemps[2] = ((serialBuf[103+STARTCOUNT]<<8) | serialBuf[104+STARTCOUNT]);
           ESCTemps[3] = ((serialBuf[113+STARTCOUNT]<<8) | serialBuf[114+STARTCOUNT]);

           AuxChanVals[0] = ((serialBuf[8+STARTCOUNT]<<8) | serialBuf[9+STARTCOUNT]);
           AuxChanVals[1] = ((serialBuf[10+STARTCOUNT]<<8) | serialBuf[11+STARTCOUNT]);
           AuxChanVals[2] = ((serialBuf[12+STARTCOUNT]<<8) | serialBuf[13+STARTCOUNT]);
           AuxChanVals[3] = ((serialBuf[14+STARTCOUNT]<<8) | serialBuf[15+STARTCOUNT]);

           current = (uint16_t)(motorCurrent[0]+motorCurrent[1]+motorCurrent[2]+motorCurrent[3])/10;
        }
      }
    } //end of aquiring telemetry data

    //calculating Voltage alarm
    if(firstloop<254 && LipoVoltage>200)
    {
      //generating a delay, because voltage reading is inaccurate in the beginning.
      firstloop++;
      percent=(firstloop*100)/255; //for showing percentage in osd
    }
    if(firstloop==254 && LipoVoltage>200)
    {
      //check the battery cells to display the correct alarm later
      if(LipoVoltage<SeparationVoltage3s4s)
      {
        BatteryCells=3;
      }
      else
      {
        BatteryCells=4;
      }
      firstloop=255;
    }
    //Voltage Alarm
    if(BatteryCells==3 && LipoVoltage<LowVoltage3s && firstloop==255|| BatteryCells==4 && LipoVoltage<LowVoltage4s && firstloop==255)
    {
      VoltageAlarm=true;
    }
    //no Voltage Alarm
    if(BatteryCells==3 && LipoVoltage>(LowVoltage3s+hysteresis) && firstloop==255|| BatteryCells==4 && LipoVoltage>(LowVoltage4s+hysteresis) && firstloop==255)
    {
      VoltageAlarm=false;
    }

    //wait if OSD is not in sync
    while (!OSD.notInVSync());

    //Variable which is set, so only upon the first arming the message with the detected battery is being displayed
    if(armed==1 && firstarmed==0)
    {
      firstarmed==1;
    }

    //calculating for time-display
    // switch disarmed => armed
   if (armedOld == 0 && armed > 0)
   {
     start_time = millis();
   }
   // switch armed => disarmed
   else if (armedOld > 0 && armed == 0)
   {
     total_time = total_time + (millis() - start_time);
     start_time = 0;
   }
   else if (armed > 0)
   {
     time = millis() - start_time+ total_time;
   }
   armedOld = armed;


    //strange for-loop
    for(i=0;i<10;i++)
    {
      Motor1KERPM[i] = ' ';
      Motor2KERPM[i] = ' ';
      Motor3KERPM[i] = ' ';
      Motor4KERPM[i] = ' ';

      Motor1Current[i] = ' ';
      Motor2Current[i] = ' ';
      Motor3Current[i] = ' ';
      Motor4Current[i] = ' ';

      ESC1Temp[i] = ' ';
      ESC2Temp[i] = ' ';
      ESC3Temp[i] = ' ';
      ESC4Temp[i] = ' ';

      LipoVoltC[i] = ' ';
      Throttle[i] = ' ';
    }


    uint8_t ThrottlePos = print_int16(throttle, Throttle,0,1);
    Throttle[ThrottlePos++] = '%';

    uint8_t CurrentPos = print_int16(current, Current,1,0);
    Current[CurrentPos++] = 'a';
    Current[CurrentPos++] = 't';

    KRPMPoses[0] = print_int16(motorKERPM[0], Motor1KERPM,1,1);
    Motor1KERPM[KRPMPoses[0]++] = 'k';
    Motor1KERPM[KRPMPoses[0]++] = 'r';

    KRPMPoses[1] = print_int16(motorKERPM[1], Motor2KERPM,1,0);
    Motor2KERPM[KRPMPoses[1]++] = 'k';
    Motor2KERPM[KRPMPoses[1]++] = 'r';

    KRPMPoses[2] = print_int16(motorKERPM[2], Motor3KERPM,1,0);
    Motor3KERPM[KRPMPoses[2]++] = 'k';
    Motor3KERPM[KRPMPoses[2]++] = 'r';

    KRPMPoses[3] = print_int16(motorKERPM[3], Motor4KERPM,1,1);
    Motor4KERPM[KRPMPoses[3]++] = 'k';
    Motor4KERPM[KRPMPoses[3]++] = 'r';


    CurrentPoses[0] = print_int16(motorCurrent[0], Motor1Current,2,1);
    Motor1Current[CurrentPoses[0]++] = 'a';

    CurrentPoses[1] = print_int16(motorCurrent[1], Motor2Current,2,0);
    Motor2Current[CurrentPoses[1]++] = 'a';

    CurrentPoses[2] = print_int16(motorCurrent[2], Motor3Current,2,0);
    Motor3Current[CurrentPoses[2]++] = 'a';

    CurrentPoses[3] = print_int16(motorCurrent[3], Motor4Current,2,1);
    Motor4Current[CurrentPoses[3]++] = 'a';



    TempPoses[0] = print_int16(ESCTemps[0], ESC1Temp,0,1);
    ESC1Temp[TempPoses[0]++] = '°';

    TempPoses[1] = print_int16(ESCTemps[1], ESC2Temp,0,0);
    ESC2Temp[TempPoses[1]++] = '°';

    TempPoses[2] = print_int16(ESCTemps[2], ESC3Temp,0,0);
    ESC3Temp[TempPoses[2]++] = '°';

    TempPoses[3] = print_int16(ESCTemps[3], ESC4Temp,0,1);
    ESC4Temp[TempPoses[3]++] = '°';




    uint8_t lipoVoltPos = print_int16(LipoVoltage, LipoVoltC,2,1);
    LipoVoltC[lipoVoltPos++] = 'v';

    uint8_t lipoMAHPos = print_int16(LipoMAH, LipoMAHC,0,1);

    uint8_t ESCmarginBot       = 0;
    uint8_t ESCmarginTop       = 0;
    uint8_t TMPmargin          = 0;
    uint8_t CurrentMargin      = 0;
    uint8_t MarginMiddleY      = 6;
    uint8_t MarginMiddleY_Old  = 6;

    uint8_t displayRCthrottle  = 0;
    uint8_t displayCombCurrent = 0;
    uint8_t displayLipoVoltage = 0;
    uint8_t displayConsumption = 0;
    uint8_t displayKRPM        = 0;
    uint8_t displayCurrent     = 0;
    uint8_t displayTemperature = 0;
    uint8_t displayTime        = 0;
    uint8_t displayPilot       = 0;


    //from here we will start the code for displaying the datas
    //reduced mode1
    #if(RED_MODE_AUX_CHAN != 0)

      if(AuxChanVals[RED_MODE_AUX_CHAN-1]<-250)
      {
        reducedModeDisplay = 1;
      }
      else if(AuxChanVals[RED_MODE_AUX_CHAN-1]>250)
      {
        reducedModeDisplay = 2;
      }
      else reducedModeDisplay = 0 ;
    #endif

    //when display mode has changed clear the screen
    if(reducedModeDisplay != lastMode)
    {
      lastMode = reducedModeDisplay;
      for(i=0;i<20;i++){
          OSD.setCursor( 0, i );
          OSD.print( clean );
      }
      while (!OSD.notInVSync());
    }

    if(reducedModeDisplay == 0){
      #if defined(DISPLAY_RC_THROTTLE)
      displayRCthrottle = 1;
      #endif
      #if defined(DISPLAY_COMB_CURRENT)
      displayCombCurrent = 1;
      #endif
      #if defined(DISPLAY_LIPO_VOLTAGE)
      displayLipoVoltage = 1;
      #endif
      #if defined(DISPLAY_MA_CONSUMPTION)
      displayConsumption = 1;
      #endif
      #if defined(DISPLAY_ESC_KRPM)
      displayKRPM = 1;
      #endif
      #if defined(DISPLAY_ESC_CURRENT)
      displayCurrent = 1;
      #endif
      #if defined(DISPLAY_ESC_TEMPERATURE)
      displayTemperature = 1;
      #endif
      #if defined(DISPLAY_PILOTNAME)
      displayPilot = 1;
      #endif
      #if defined(DISPLAY_TIMER)
      displayTime = 1;
      #endif
    }
    else if(reducedModeDisplay==1)
    {
      #if defined(RED_DISPLAY_RC_THROTTLE)
      displayRCthrottle = 1;
      #endif
      #if defined(RED_DISPLAY_COMB_CURRENT)
      displayCombCurrent = 1;
      #endif
      #if defined(RED_DISPLAY_LIPO_VOLTAGE)
      displayLipoVoltage = 1;
      #endif
      #if defined(RED_DISPLAY_MA_CONSUMPTION)
      displayConsumption = 1;
      #endif
      #if defined(RED_DISPLAY_ESC_KRPM)
      displayKRPM = 1;
      #endif
      #if defined(RED_DISPLAY_ESC_CURRENT)
      displayCurrent = 1;
      #endif
      #if defined(RED_DISPLAY_ESC_TEMPERATURE)
      displayTemperature = 1;
      #endif
      #if defined(RED_DISPLAY_PILOTNAME)
      displayPilot = 1;
      #endif
      #if defined(DISPLAY_TIMER)
      displayTime = 1;
      #endif
    }
    else if(reducedModeDisplay==2)
    {
      #if defined(RED2_DISPLAY_RC_THROTTLE)
      displayRCthrottle = 1;
      #endif
      #if defined(RED2_DISPLAY_COMB_CURRENT)
      displayCombCurrent = 1;
      #endif
      #if defined(RED2_DISPLAY_LIPO_VOLTAGE)
      displayLipoVoltage = 1;
      #endif
      #if defined(RED2_DISPLAY_MA_CONSUMPTION)
      displayConsumption = 1;
      #endif
      #if defined(RED2_DISPLAY_ESC_KRPM)
      displayKRPM = 1;
      #endif
      #if defined(RED2_DISPLAY_ESC_CURRENT)
      displayCurrent = 1;
      #endif
      #if defined(RED2_DISPLAY_ESC_TEMPERATURE)
      displayTemperature = 1;
      #endif
      #if defined(RED2_DISPLAY_PILOTNAME)
      displayPilot = 1;
      #endif
      #if defined(DISPLAY_TIMER)
      displayTime = 1;
      #endif
    }

    if(displayRCthrottle)
    {
      OSD.setCursor( 0, 0 );
      OSD.print( "throt:" );
      OSD.print( Throttle );
      ESCmarginTop = 1;
    }
    if(displayCombCurrent)
    {
      OSD.setCursor( -CurrentPos, 0 );
      OSD.print( Current );
      ESCmarginTop = 1;
    }

    if(displayPilot)
    {
      OSD.setCursor( 10, 0 );
      OSD.print( Pilotname );
    }

    if(displayLipoVoltage)
    {
      OSD.setCursor( marginLastRow, -1 );
      //OSD.print( "bat:" );
      if(VoltageAlarm==true)
      {
        OSD.blink();
        OSD.print( LipoVoltC );
        OSD.noBlink();
      }
      else
      {
        OSD.print( LipoVoltC );
      }

      ESCmarginBot = 1;
    }

    if(displayTime)
    {
      OSD.setCursor( marginLastRow+8, -1 );
      print_time(time, Time);
      OSD.print(" ");
      OSD.print(Time);
    }

    if(displayConsumption)
    {
      OSD.setCursor( -(5+(lipoMAHPos+marginLastRow)), -1 );
      //OSD.print( "co:" );
      if(LipoMAH>CapacityThreshold)
      {
        OSD.blink();
        OSD.print( LipoMAHC );
        OSD.print( "ma" );
        OSD.noBlink();
      }
      else
      {
        OSD.print( LipoMAHC );
        OSD.print( "ma" );
      }
      ESCmarginBot = 1;
    }

    if(displayKRPM)
    {
      OSD.setCursor( 0, ESCmarginTop );
      OSD.print( Motor1KERPM );
      OSD.setCursor( -KRPMPoses[1], ESCmarginTop );
      OSD.print( Motor2KERPM );
      OSD.setCursor( -KRPMPoses[2], -(1+ESCmarginBot) );
      OSD.print( Motor3KERPM );
      OSD.setCursor( 0, -(1+ESCmarginBot) );
      OSD.print( Motor4KERPM );
      TMPmargin++;
      CurrentMargin++;
    }

    if(displayCurrent)
    {
      OSD.setCursor( 0, CurrentMargin+ESCmarginTop );
      OSD.print( Motor1Current );
      OSD.setCursor( -CurrentPoses[1], CurrentMargin+ESCmarginTop );
      OSD.print( Motor2Current );
      OSD.setCursor( -CurrentPoses[2], -(1+CurrentMargin+ESCmarginBot) );
      OSD.print( Motor3Current );
      OSD.setCursor( 0, -(1+CurrentMargin+ESCmarginBot) );
      OSD.print( Motor4Current );
      TMPmargin++;
    }

    if(displayTemperature)
    {
      OSD.setCursor( 0, TMPmargin+ESCmarginTop );
      OSD.print( ESC1Temp );
      OSD.setCursor( -TempPoses[1], TMPmargin+ESCmarginTop );
      OSD.print( ESC2Temp );
      OSD.setCursor( -TempPoses[2], -(1+TMPmargin+ESCmarginBot) );
      OSD.print( ESC3Temp );
      OSD.setCursor( 0, -(1+TMPmargin+ESCmarginBot) );
      OSD.print( ESC4Temp );
    }

    //show the detected cell count upon the first arming
    if(current==0 && BatteryCells!=0 && armed==0 && firstarmed==0 && firstloop==255)
    {
      OSD.setCursor(4,MarginMiddleY);
      MarginMiddleY++;
      if(BatteryCells==3)
      {
        OSD.print(threeSBatteryDetected);
      }
      if(BatteryCells==4)
      {
        OSD.print(fourSBatteryDetected);
      }
    }
    if(firstloop<255)
    {
      OSD.setCursor(4,MarginMiddleY);
      MarginMiddleY++;
      OSD.blink();
      OSD.print("Wait - don't arm: ");
      OSD.print(percent);
      OSD.noBlink();
    }

    //show armed | dissarmed
    if(armed==0)
    {
      OSD.setCursor(4,MarginMiddleY);
      MarginMiddleY++;
      OSD.print("     dissarmed      ");
      armedstarted=millis();
    }
    else if (armedstarted+2000>millis())
    {
        OSD.setCursor(4,MarginMiddleY);
        MarginMiddleY++;
        OSD.blink();
        OSD.print("       armed        ");
        OSD.noBlink();
    }
    else
    {
      firstarmed==1;
    }

    if(failsafe>10)
    {
      OSD.setCursor(4,MarginMiddleY);
      MarginMiddleY++;
      OSD.print("  failsafe active   ");
    }

    //clearing middle screen below displayed datas
    uint8_t iii=0;
    while((MarginMiddleY+iii)<=10)
    {
      OSD.setCursor(4,MarginMiddleY+iii);
      OSD.print("                    ");
      iii++;
    }

  }
}

// END OF Main-Loop
//=========================================================================================================================
