//default values used for initialising the EEPROM
//*************************************************
												//*
// displayed datas in normal mode				//*
#define RED_DISPLAY_RC_THROTTLE 0				//*
#define RED_DISPLAY_COMB_CURRENT 1				//*
#define RED_DISPLAY_LIPO_VOLTAGE 1				//*
#define RED_DISPLAY_MA_CONSUMPTION 1			//*
#define RED_DISPLAY_ESC_KRPM 0					//*
#define RED_DISPLAY_ESC_CURRENT 0				//*
#define RED_DISPLAY_ESC_TEMPERATURE 0			//*
#define RED_DISPLAY_PILOTNAME 1					//*
#define RED_DISPLAY_TIMER 1						//*
#define RED_DISPLAY_ANGLE 1						//*
// displayed datas in reduced mode 1			//*
#define RED2_DISPLAY_RC_THROTTLE 0				//*
#define RED2_DISPLAY_COMB_CURRENT 0				//*
#define RED2_DISPLAY_LIPO_VOLTAGE 1				//*
#define RED2_DISPLAY_MA_CONSUMPTION 1			//*
#define RED2_DISPLAY_ESC_KRPM 0					//*
#define RED2_DISPLAY_ESC_CURRENT 0				//*
#define RED2_DISPLAY_ESC_TEMPERATURE 0			//*
#define RED2_DISPLAY_PILOTNAME 0				//*
#define RED2_DISPLAY_TIMER 0					//*
#define RED2_DISPLAY_ANGLE 0					//*
// displayed datas in reduced mode 2			//*
#define RED3_DISPLAY_RC_THROTTLE 0				//*
#define RED3_DISPLAY_COMB_CURRENT 0				//*
#define RED3_DISPLAY_LIPO_VOLTAGE 1				//*
#define RED3_DISPLAY_MA_CONSUMPTION 1			//*
#define RED3_DISPLAY_ESC_KRPM 1					//*
#define RED3_DISPLAY_ESC_CURRENT 1				//*
#define RED3_DISPLAY_ESC_TEMPERATURE 1			//*
#define RED3_DISPLAY_PILOTNAME 1				//*
#define RED3_DISPLAY_TIMER 1					//*
#define RED3_DISPLAY_ANGLE 1					//*
//margin left and right for the last line.		//*
#define DmarginLastRow  4;						//*
//Voltage Settings								//*
#define DLowVoltage1st 350;						//*
#define DLowVoltage2nd  320;					//*
#define Dhysteresis  10;						//*
#define DVoltageOffset  0;						//*
#define DLowVoltageAllowed 1;					//*
//Capacity settings								//*
#define DCapacity  1800;						//*
#define DCapacity1st  70;						//*
#define DCapacity2nd  80;						//*
// motors magnepole count						//*
#define DMAGNETPOLECOUNT 14 // 2 for ERPMs		//*
// Filter for ESC datas							//*
#define DESC_FILTER 10							//*
// reduced mode channel config					//*
#define DRED_MODE_AUX_CHAN 4 // 0-4, 0 = none	//*
												//*
//*************************************************


const byte osdChipSelect = 6;
const byte masterOutSlaveIn = MOSI;
const byte masterInSlaveOut = MISO;
const byte slaveClock = SCK;
const byte osdReset = 2;

MAX7456 OSD(osdChipSelect);

static char clean[30];

static uint8_t firstloop = 0;
static uint8_t BatteryCells = 0;		//stores the number of cells recognized in the first run
static boolean VoltageAlarm = false;	//works with the const defined in the beginning | Filters Voltage drops to avoid erratic voltage alarms
static boolean VoltageAlarm2nd = false;	//2nd stage of voltage alarms

static uint16_t current = 0;
static int16_t LipoVoltage = 0;
static uint16_t LipoMAH = 0;
static uint16_t motorKERPM[4] = { 0,0,0,0 };
static uint16_t motorCurrent[4] = { 0,0,0,0 };
static uint16_t ESCTemps[4] = { 0,0,0,0 };
static int16_t  AuxChanVals[4] = { 0,0,0,0 };
static int16_t  StickChanVals[4] = { 0,0,0,0 };
static uint8_t  reducedMode = 0;
static uint8_t  reducedMode2 = 0;
static uint8_t  reducedModeDisplay = 0;
static uint8_t armed = 0;
static uint8_t armedOld = 0;
static uint8_t failsafe = 0;
static uint16_t calibGyroDone = 0;

static int16_t angley = 0;

static unsigned long start_time = 0;
static unsigned long time = 0;
static unsigned long total_time = 0;

static uint8_t percent = 0;
static uint8_t firstarmed = 0;

static unsigned long armedstarted = 0;

static uint8_t extra_space_mah = 0;

static uint16_t i = 0;
static uint8_t KRPMPoses[4];
static uint8_t lastMode = 0;

char Motor1KERPM[30];
char Motor2KERPM[30];
char Motor3KERPM[30];
char Motor4KERPM[30];

static uint8_t CurrentPoses[4];
static char Motor1Current[30];
static char Motor2Current[30];
static char Motor3Current[30];
static char Motor4Current[30];

static uint8_t TempPoses[4];
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

static uint32_t tmpVoltage = 0;
static uint32_t voltDev = 0;

static uint8_t ThrottlePos;
static uint8_t CurrentPos;

static int configAdress = 0;
static bool memValid = true;

struct StoreStruct {

	uint16_t LowVoltage1st; //
	uint16_t LowVoltage2nd; //
	uint8_t LowVoltageAllowed; //
	uint16_t hysteresis;
	int8_t VoltageOffset;
	uint8_t marginLastRow;
	uint8_t RED_MODE_AUX_CHANNEL;
	uint8_t MAGNETPOLECOUNT;
	uint8_t ESC_FILTER;
	uint16_t Capacity;
	uint8_t Capacity1st;
	uint8_t Capacity2nd;
	uint8_t MemoryVersion;
	uint8_t DispRCThrottle1;
	uint8_t DispCombCurrent1;
	uint8_t DispLipoVoltage1;
	uint8_t DispMaConsumption1;
	uint8_t DispEscKrpm1;
	uint8_t DispEscCurrent1;
	uint8_t DispEscTemp1;
	uint8_t DispPilotname1;
	uint8_t DispTimer1;
	uint8_t DispAngle1;
	uint8_t DispRCThrottle2;
	uint8_t DispCombCurrent2;
	uint8_t DispLipoVoltage2;
	uint8_t DispMaConsumption2;
	uint8_t DispEscKrpm2;
	uint8_t DispEscCurrent2;
	uint8_t DispEscTemp2;
	uint8_t DispPilotname2;
	uint8_t DispTimer2;
	uint8_t DispAngle2;
	uint8_t DispRCThrottle3;
	uint8_t DispCombCurrent3;
	uint8_t DispLipoVoltage3;
	uint8_t DispMaConsumption3;
	uint8_t DispEscKrpm3;
	uint8_t DispEscCurrent3;
	uint8_t DispEscTemp3;
	uint8_t DispPilotname3;
	uint8_t DispTimer3;
	uint8_t DispAngle3;
	uint8_t stockSettings;
};

static StoreStruct Settings;