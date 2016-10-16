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

	uint16_t LowVoltage3s;
	uint16_t LowVoltage4s;
	uint16_t SeparationVoltage3s4s;
	uint16_t hysteresis;
	uint16_t MinimalCellVoltage2nd;
	int8_t VoltageOffset;
	uint8_t marginLastRow;
	uint8_t RED_MODE_AUX_CHANNEL;
	uint8_t MAGNETPOLECOUNT;
	uint8_t ESC_FILTER;
	uint16_t CapacityThreshold;
	uint16_t CapacityThreshold2ndStage;
	uint8_t MemoryVersion;

} static Settings;

enum Page {
	null,
	Info,
	Voltage,
	Capacity,
	other_Settings
	

};
