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
//stock offset									//*
#define DOffsetX 0								//*
#define DOffsetY 0								//*
//stock current									//*
#define DStandbyCurrent 1000					//*
//*************************************************

#define osdChipSelect	6
#define STARTCOUNT		2

#define GET_TELEMETRY	0x20
#define GET_INFO		0x21
#define	ESC_INFO		0x22
#define	GET_SETTINGS	0x30
#define	SET_SETTINGS	0x10
#define	MOTOR_TEST		0x11

MAX7456 OSD(osdChipSelect);

struct SerialData
{
	uint16_t current = 0;
	uint16_t LipoMAH = 0;
	float standbyCurrentTotal = 0;
	int16_t	 LipoVoltage = 0;
	
	uint16_t motorKERPM[4] = { 0,0,0,0 };
	uint16_t motorCurrent[4] = { 0,0,0,0 };
	uint16_t ESCTemps[4] = { 0,0,0,0 };

	int16_t  AuxChanVals[4] = { 0,0,0,0 };
	int16_t  StickChanVals[4] = { 0,0,0,0 };

	int16_t angley = 0;
	
	uint8_t armed = 0;
	uint8_t failsafe = 0;
	uint16_t calibGyroDone = 0;
} static KissData;

struct Status
{
	uint8_t BatteryCells = 0;			//stores the number of cells recognized in the first run
	boolean VoltageAlarm = false;		//works with the const defined in the beginning | Filters Voltage drops to avoid erratic voltage alarms
	boolean VoltageAlarm2nd = false;	//2nd stage of voltage alarms

	uint8_t  reducedModeDisplay = 0;	//Actual Display-mode
	uint8_t lastMode = 0;				//last Display-mode

	uint8_t armedOld = 0;				//last Armed Status

	unsigned long start_time = 0;		//Time when armed
	unsigned long time = 0;				//Current time to display
	unsigned long total_time = 0;		//Total flight time

	uint8_t firstarmed = 0;
	unsigned long armedstarted = 0;

	int configAdress = 0;				//EEPROM ConfigAdress
	bool memValid = true;				//MemoryIsValid

	uint32_t LastLoopTime;	

} static KissStatus;

static uint16_t i = 0;

static uint8_t TempCharPosition;
static char TempCharConverted[30];

enum SerialStatus					//giving the status of the current serial connection to the Kiss FC
{
	unknown,
	WaitingForConn,
	ConnectionEtablished,
	Connected,
	LostConnection
}static KissConnection;

struct Stats
{
	uint16_t MaxCurrentTotal = 0;
	int16_t  MinVoltage = 32767;
	uint16_t MAXmotorKERPM = 0;
	uint16_t MAXmotorCurrent = 0;
	uint16_t MAXESCTemp =  0;
	uint16_t MAXWatt = 0;
} static KissStats;


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
	int8_t OffsetY;
	int8_t OffsetX;
	uint16_t StandbyCurrent;
} static Settings;

struct Settings
{
	uint16_t PID_P[3];		//holds the P part for all three axis
	uint16_t PID_I[3];		//holds the I part for all three axis
	uint16_t PID_D[3];		//holds the D part for all three axis
	uint16_t PID_A[3];		//holds PID for something called A???
	int16_t ACC_Trim[2];	//Accelerometer trimm data, Pitch and Roll
	int16_t RC_Rate[3];		//holds the RC-Rate
	int16_t RPY_Expo[3];	//RPY_Expo??
	int16_t RPY_Curve[3];	//RPY_Curve??

	//there are a lot of other setting according to here: https://github.com/fedorcomander/kissfc-chrome-gui/blob/master/js/protocol.js
	//for memory saving reasons there will be only this for now


} KissSettingsPID;