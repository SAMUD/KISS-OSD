/* 1. Information
//=========================================================================================================================

Fonctions needed for various things in the OSD.

//=========================================================================================================================
//START OF CODE
*/

//=====================
// fonction print_int16
uint8_t print_int16(int16_t p_int, char *str, uint8_t dec, uint8_t AlignLeft)
{
	uint16_t useVal = p_int;
	uint8_t pre = ' ';
	if (p_int < 0)
	{
		useVal = p_int*-1;
		pre = '-';
	}
	uint8_t aciidig[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	uint8_t i = 0;
	uint8_t digits[6] = { 0,0,0,0,0,0 };
	while (useVal >= 10000) { digits[0]++; useVal -= 10000; }
	while (useVal >= 1000) { digits[1]++; useVal -= 1000; }
	while (useVal >= 100) { digits[2]++; useVal -= 100; }
	while (useVal >= 10) { digits[3]++; useVal -= 10; }
	digits[4] = useVal;
	char result[6] = { ' ',' ',' ',' ',' ','0' };
	uint8_t signdone = 0;
	for (i = 0; i < 6; i++)
	{
		if (i == 5 && signdone == 0) continue;
		else if (aciidig[digits[i]] != '0' && signdone == 0)
		{
			result[i] = pre;
			signdone = 1;
		}
		else if (signdone)
			result[i] = aciidig[digits[i - 1]];
	}
	uint8_t CharPos = 0;
	for (i = 0; i < 6; i++)
	{
		if (result[i] != ' ' || (AlignLeft == 0 || (i > 5 - dec))) str[CharPos++] = result[i];
		if (dec != 0 && i == 5 - dec) str[CharPos++] = '.';
		if (dec != 0 && i > 5 - dec && str[CharPos - 1] == ' ') str[CharPos - 1] = '0';
	}
	return CharPos;
}


//===========
// ESC-Filter
uint32_t ESC_filter(uint32_t oldVal, uint32_t newVal)
{
	return (uint32_t)((uint32_t)((uint32_t)((uint32_t)oldVal*Settings.ESC_FILTER) + (uint32_t)newVal)) / (Settings.ESC_FILTER + 1);
}

//========================
//Convert time in a string
void print_time(unsigned long time, char *time_str)
{
	uint16_t seconds = time / 1000;
	uint8_t mills = time % 1000;
	uint8_t minutes = 0;
	if (seconds >= 60)
	{
		minutes = seconds / 60;
	}
	else
	{
		minutes = 0;
	}
	seconds = seconds % 60; // reste
	static char time_sec[6];
	static char time_mil[6];
	uint8_t i = 0;
	uint8_t time_pos = print_int16(minutes, time_str, 0, 1);
	time_str[time_pos++] = ':';

	uint8_t sec_pos = print_int16(seconds, time_sec, 0, 1);
	for (i = 0; i<sec_pos; i++)
	{
		time_str[time_pos++] = time_sec[i];
	}
	//time_str[time_pos++] = '.';

	//uint8_t mil_pos = print_int16(mills, time_mil,0,1);
	//time_str[time_pos++] = time_mil[0];
	for (i = time_pos; i<9; i++)
	{
		time_str[time_pos++] = ' ';
	}
}

//=========================================
//Read serial data from FC and store in var
void getSerialData()
{
	static uint8_t serialBuf[255];
	uint8_t minBytes = 100;
	uint8_t recBytes = 0;

	Serial.write(0x20); // request telemetrie

						//aquire serial data and write it to normal variables
	while (recBytes < minBytes && micros() - LastLoopTime < 20000)
	{
#define STARTCOUNT 2
		while (Serial.available()) serialBuf[recBytes++] = Serial.read();
		if (recBytes == 1 && serialBuf[0] != 5)recBytes = 0; // check for start byte, reset if its wrong
		if (recBytes == 2) minBytes = serialBuf[1] + STARTCOUNT + 1; // got the transmission length
		if (recBytes == minBytes)
		{
			uint32_t checksum = 0;
			for (i = 2; i<minBytes; i++)
			{
				checksum += serialBuf[i];
			}
			checksum = (uint32_t)checksum / (minBytes - 3);

			if (checksum == serialBuf[recBytes - 1])
			{
				//armed
				armed = ((serialBuf[15 + STARTCOUNT] << 8) | serialBuf[16 + STARTCOUNT]);
				//Lipo Voltage from FC
				LipoVoltage = ((serialBuf[17 + STARTCOUNT] << 8) | serialBuf[18 + STARTCOUNT]);
				//failsafe
				failsafe = ((serialBuf[40 + STARTCOUNT] << 8) | (serialBuf[41 + STARTCOUNT])); //42
																							   //calib Done
				calibGyroDone = ((serialBuf[39 + STARTCOUNT] << 8) | serialBuf[40 + STARTCOUNT]);
				//angle y for displaying in the horizon bar
				angley = ((serialBuf[33 + STARTCOUNT] << 8) | (serialBuf[34 + STARTCOUNT])) / 100; //35

				tmpVoltage = 0;
				voltDev = 0;
				//Voltage ESC
				if (((serialBuf[85 + STARTCOUNT] << 8) | serialBuf[86 + STARTCOUNT]) > 5) // the ESC's read the voltage better then the FC
				{
					tmpVoltage += ((serialBuf[85 + STARTCOUNT] << 8) | serialBuf[86 + STARTCOUNT]);
					voltDev++;
				}
				if (((serialBuf[95 + STARTCOUNT] << 8) | serialBuf[96 + STARTCOUNT]) > 5)
				{
					tmpVoltage += ((serialBuf[95 + STARTCOUNT] << 8) | serialBuf[96 + STARTCOUNT]);
					voltDev++;
				}
				if (((serialBuf[105 + STARTCOUNT] << 8) | serialBuf[106 + STARTCOUNT]) > 5)
				{
					tmpVoltage += ((serialBuf[105 + STARTCOUNT] << 8) | serialBuf[106 + STARTCOUNT]);
					voltDev++;
				}
				if (((serialBuf[115 + STARTCOUNT] << 8) | serialBuf[116 + STARTCOUNT]) > 5)
				{
					tmpVoltage += ((serialBuf[115 + STARTCOUNT] << 8) | serialBuf[116 + STARTCOUNT]);
					voltDev++;
				}
				if (((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]) > 5)
				{
					tmpVoltage += ((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]);
					voltDev++;
				}
				if (((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]) > 5)
				{
					tmpVoltage += ((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]);
					voltDev++;
				}
				if (voltDev != 0)
					LipoVoltage = (tmpVoltage / voltDev);
				LipoVoltage = LipoVoltage + Settings.VoltageOffset;
				//capacity
				LipoMAH = ((serialBuf[148 + STARTCOUNT] << 8) | serialBuf[149 + STARTCOUNT]);
				//read Motor Current and other ESC datas
				static uint32_t windedupfilterdatas[8];
				//RPM
				windedupfilterdatas[0] = ESC_filter((uint32_t)windedupfilterdatas[0], (uint32_t)((serialBuf[91 + STARTCOUNT] << 8) | serialBuf[92 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[1] = ESC_filter((uint32_t)windedupfilterdatas[1], (uint32_t)((serialBuf[101 + STARTCOUNT] << 8) | serialBuf[102 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[2] = ESC_filter((uint32_t)windedupfilterdatas[2], (uint32_t)((serialBuf[111 + STARTCOUNT] << 8) | serialBuf[112 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[3] = ESC_filter((uint32_t)windedupfilterdatas[3], (uint32_t)((serialBuf[121 + STARTCOUNT] << 8) | serialBuf[122 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				motorKERPM[0] = windedupfilterdatas[0] >> 4;
				motorKERPM[1] = windedupfilterdatas[1] >> 4;
				motorKERPM[2] = windedupfilterdatas[2] >> 4;
				motorKERPM[3] = windedupfilterdatas[3] >> 4;
				//Current
				windedupfilterdatas[4] = ESC_filter((uint32_t)windedupfilterdatas[4], (uint32_t)((serialBuf[87 + STARTCOUNT] << 8) | serialBuf[88 + STARTCOUNT]) << 4);
				windedupfilterdatas[5] = ESC_filter((uint32_t)windedupfilterdatas[5], (uint32_t)((serialBuf[97 + STARTCOUNT] << 8) | serialBuf[98 + STARTCOUNT]) << 4);
				windedupfilterdatas[6] = ESC_filter((uint32_t)windedupfilterdatas[6], (uint32_t)((serialBuf[107 + STARTCOUNT] << 8) | serialBuf[108 + STARTCOUNT]) << 4);
				windedupfilterdatas[7] = ESC_filter((uint32_t)windedupfilterdatas[7], (uint32_t)((serialBuf[117 + STARTCOUNT] << 8) | serialBuf[118 + STARTCOUNT]) << 4);
				motorCurrent[0] = windedupfilterdatas[4] >> 4;
				motorCurrent[1] = windedupfilterdatas[5] >> 4;
				motorCurrent[2] = windedupfilterdatas[6] >> 4;
				motorCurrent[3] = windedupfilterdatas[7] >> 4;
				//ESC Temps
				ESCTemps[0] = ((serialBuf[83 + STARTCOUNT] << 8) | serialBuf[84 + STARTCOUNT]);
				ESCTemps[1] = ((serialBuf[93 + STARTCOUNT] << 8) | serialBuf[94 + STARTCOUNT]);
				ESCTemps[2] = ((serialBuf[103 + STARTCOUNT] << 8) | serialBuf[104 + STARTCOUNT]);
				ESCTemps[3] = ((serialBuf[113 + STARTCOUNT] << 8) | serialBuf[114 + STARTCOUNT]);
				//Aux chan vals
				AuxChanVals[0] = ((serialBuf[8 + STARTCOUNT] << 8) | serialBuf[9 + STARTCOUNT]);
				AuxChanVals[1] = ((serialBuf[10 + STARTCOUNT] << 8) | serialBuf[11 + STARTCOUNT]);
				AuxChanVals[2] = ((serialBuf[12 + STARTCOUNT] << 8) | serialBuf[13 + STARTCOUNT]);
				AuxChanVals[3] = ((serialBuf[14 + STARTCOUNT] << 8) | serialBuf[15 + STARTCOUNT]);
				//StickChanVals
				StickChanVals[0] = ((serialBuf[0 + STARTCOUNT] << 8) | serialBuf[1 + STARTCOUNT]);
				StickChanVals[1] = ((serialBuf[2 + STARTCOUNT] << 8) | serialBuf[3 + STARTCOUNT]);
				StickChanVals[2] = ((serialBuf[4 + STARTCOUNT] << 8) | serialBuf[5 + STARTCOUNT]);
				StickChanVals[3] = ((serialBuf[6 + STARTCOUNT] << 8) | serialBuf[7 + STARTCOUNT]);
				//total current
				current = (uint16_t)(motorCurrent[0] + motorCurrent[1] + motorCurrent[2] + motorCurrent[3]) / 10;
			}
		}
	} //end of aquiring telemetry data
}

//===============
//Display the OSD
void DisplayOSD()
{
	//Declaring some vars
	uint8_t lipoVoltPos = print_int16(LipoVoltage, LipoVoltC, 2, 1);


	uint8_t lipoMAHPos = print_int16(LipoMAH, LipoMAHC, 0, 1);

	uint8_t ESCmarginBot = 0;
	uint8_t ESCmarginTop = 0;
	uint8_t TMPmargin = 0;
	uint8_t CurrentMargin = 0;
	uint8_t MarginMiddleY = 6;
	uint8_t MarginMiddleY_Old = 6;

	uint8_t displayRCthrottle = 0;
	uint8_t displayCombCurrent = 0;
	uint8_t displayLipoVoltage = 0;
	uint8_t displayConsumption = 0;
	uint8_t displayKRPM = 0;
	uint8_t displayCurrent = 0;
	uint8_t displayTemperature = 0;
	uint8_t displayTime = 0;
	uint8_t displayPilot = 0;
	uint8_t displayAngle = 0;

	//reduced mode1
	if (Settings.RED_MODE_AUX_CHANNEL != 0)
	{
		if (AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]<-250)
		{
			reducedModeDisplay = 1;
		}
		else if (AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]>250)
		{
			reducedModeDisplay = 2;
		}
		else reducedModeDisplay = 0;
	}

	//when display mode has changed clear the screen
	if (reducedModeDisplay != lastMode)
	{
		lastMode = reducedModeDisplay;
		for (i = 0; i<20; i++)
		{
			OSD.setCursor(0, i);
			OSD.print(clean);
		}
		while (!OSD.notInVSync());
	}

	if (reducedModeDisplay == 0) {
		displayRCthrottle = Settings.DispRCThrottle1;
		displayCombCurrent = Settings.DispCombCurrent1;
		displayLipoVoltage = Settings.DispLipoVoltage1;
		displayConsumption = Settings.DispMaConsumption1;
		displayKRPM = Settings.DispEscKrpm1;
		displayCurrent = Settings.DispEscCurrent1;
		displayTemperature = Settings.DispEscTemp1;
		displayPilot = Settings.DispPilotname1;
		displayTime = Settings.DispTimer1;
		displayAngle = Settings.DispAngle1;
	}
	else if (reducedModeDisplay == 1)
	{
		displayRCthrottle = Settings.DispRCThrottle2;
		displayCombCurrent = Settings.DispCombCurrent2;
		displayLipoVoltage = Settings.DispLipoVoltage2;
		displayConsumption = Settings.DispMaConsumption2;
		displayKRPM = Settings.DispEscKrpm2;
		displayCurrent = Settings.DispEscCurrent2;
		displayTemperature = Settings.DispEscTemp2;
		displayPilot = Settings.DispPilotname2;
		displayTime = Settings.DispTimer2;
		displayAngle = Settings.DispAngle2;
	}
	else if (reducedModeDisplay == 2)
	{
		displayRCthrottle = Settings.DispRCThrottle3;
		displayCombCurrent = Settings.DispCombCurrent3;
		displayLipoVoltage = Settings.DispLipoVoltage3;
		displayConsumption = Settings.DispMaConsumption3;
		displayKRPM = Settings.DispEscKrpm3;
		displayCurrent = Settings.DispEscCurrent3;
		displayTemperature = Settings.DispEscTemp3;
		displayPilot = Settings.DispPilotname3;
		displayTime = Settings.DispTimer3;
		displayAngle = Settings.DispAngle3;
	}

	if (displayRCthrottle)
	{
		OSD.setCursor(0, 0);
		OSD.print(F("THROT:"));
		OSD.print(Throttle);
		ESCmarginTop = 1;
	}

	if (displayCombCurrent)
	{
		OSD.setCursor(-CurrentPos, 0);
		OSD.print(Current);
		ESCmarginTop = 1;
	}

	if (displayPilot)
	{
		OSD.setCursor(10, 0);
		OSD.print(Pilotname);
	}

	if (Settings.stockSettings = 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("STOCK SETT-OPEN MENU"));
	}
	

	if (displayLipoVoltage)
	{
		OSD.setCursor(Settings.marginLastRow, -1);
		if (VoltageAlarm == true)
		{
			OSD.blink();
			OSD.write(SYM_MAIN_BATT);
			OSD.print(LipoVoltC);
			if (VoltageAlarm2nd == true)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("    LIPO VOLTAGE    "));
			}
			OSD.noBlink();
		}
		else
		{
			OSD.write(SYM_MAIN_BATT);
			OSD.print(LipoVoltC);
		}
		ESCmarginBot = 1;
	}

	if (displayTime)
	{
		OSD.setCursor(12, -1);
		print_time(time, Time);
		OSD.write(SYM_FLY_M);
		OSD.print(Time);
	}

	if (displayConsumption)
	{
		OSD.setCursor(-(lipoMAHPos + 1 + Settings.marginLastRow), -1);
		if (LipoMAH>(Settings.Capacity * (float)Settings.Capacity1st) / 100 && Settings.Capacity>0)
		{
			OSD.blink();
			OSD.write(SYM_MAH);
			OSD.print(LipoMAHC);
			if (LipoMAH>(Settings.Capacity * (float)Settings.Capacity2nd) / 100)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("      CAPACITY      "));
			}
			OSD.noBlink();
		}
		else
		{
			OSD.write(SYM_MAH);
			OSD.print(LipoMAHC);
		}
		ESCmarginBot = 1;
	}

	if (displayKRPM)
	{
		OSD.setCursor(0, ESCmarginTop);
		OSD.print(Motor1KERPM);
		OSD.setCursor(-KRPMPoses[1], ESCmarginTop);
		OSD.print(Motor2KERPM);
		OSD.setCursor(-KRPMPoses[2], -(1 + ESCmarginBot));
		OSD.print(Motor3KERPM);
		OSD.setCursor(0, -(1 + ESCmarginBot));
		OSD.print(Motor4KERPM);
		TMPmargin++;
		CurrentMargin++;
	}

	if (displayCurrent)
	{
		OSD.setCursor(0, CurrentMargin + ESCmarginTop);
		OSD.print(Motor1Current);
		OSD.setCursor(-CurrentPoses[1], CurrentMargin + ESCmarginTop);
		OSD.print(Motor2Current);
		OSD.setCursor(-CurrentPoses[2], -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(Motor3Current);
		OSD.setCursor(0, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(Motor4Current);
		TMPmargin++;
	}

	if (displayTemperature)
	{
		OSD.setCursor(0, TMPmargin + ESCmarginTop);
		OSD.print(ESC1Temp);
		OSD.setCursor(-TempPoses[1] - 1, TMPmargin + ESCmarginTop);
		OSD.print(ESC2Temp);
		OSD.setCursor(-TempPoses[2] - 1, -(1 + TMPmargin + ESCmarginBot));
		OSD.print(ESC3Temp);
		OSD.setCursor(0, -(1 + TMPmargin + ESCmarginBot));
		OSD.print(ESC4Temp);
	}

	if (displayAngle)
	{
		OSD.setCursor(0, 6);
		drawAngelIndicator(angley);
	}

	//show the detected cell count upon the first 30 flight-sec if not armed
	if (current == 0 && BatteryCells != 0 && armed == 0 && firstarmed == 0 && firstloop == 255 && time<30000)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		switch (BatteryCells)
		{
		case 1: OSD.print(F("1S BATTERY CONNECTED"));
			break;
		case 2: OSD.print(F("2S BATTERY CONNECTED"));
			break;
		case 3: OSD.print(F("3S BATTERY CONNECTED"));
			break;
		case 4: OSD.print(F("4S BATTERY CONNECTED"));
			break;
		case 5: OSD.print(F("5S BATTERY CONNECTED"));
			break;
		case 6: OSD.print(F("6S BATTERY CONNECTED"));
			break;
		default:OSD.print(F("ERR BAT CELLS: "));
				OSD.print(BatteryCells);
			break;
		}
	}

	if (firstloop<255)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.blink();
		OSD.print(F("WAIT - DON'T ARM: "));
		OSD.noBlink();
		OSD.print(percent);
	}

	//show armed | dissarmed
	if (armed == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("     DISSARMED      "));
		armedstarted = millis();
	}
	else if (armedstarted + 2000>millis())
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.blink();
		OSD.print(F("       ARMED        "));
		OSD.noBlink();
	}
	else if (firstarmed == 0)
	{
		firstarmed == 1;
	}

	if (failsafe>5)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("      FAILSAFE      "));
	}

	if (calibGyroDone>100)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("  CALIBRATING GYRO  "));
	}

	//clearing middle screen below displayed datas
	uint8_t iii = 0;
	while ((MarginMiddleY + iii) <= 10)
	{
		OSD.setCursor(4, MarginMiddleY + iii);
		OSD.print(F("                    "));
		iii++;
	}
}

//======================
//Calculate the OSD data
void CalculateOSD()
{

	//calculating Voltage alarm
	if (firstloop<254 && LipoVoltage>200)
	{
		//generating a delay, because voltage reading is inaccurate in the beginning.
		firstloop++;
		percent = (firstloop * 100) / 255; //for showing percentage in osd
	}
	if (firstloop == 254  && LipoVoltage>200)
	{
		//check the battery cells to display the correct alarm later
		uint16_t tempVoltage = LipoVoltage;
		BatteryCells = 1;
		while (tempVoltage > 440)
		{
			BatteryCells = BatteryCells + 1;
			tempVoltage = tempVoltage - 440;
		}
		firstloop = 255;
	}
	//Voltage Alarm 1 and 2
	if ( (LipoVoltage / BatteryCells)<Settings.LowVoltage1st  && firstloop == 255 && Settings.LowVoltageAllowed == 1)
	{
		
		VoltageAlarm = true;
	}
	if ( (LipoVoltage / BatteryCells)<Settings.LowVoltage2nd && VoltageAlarm == true)
	{
		VoltageAlarm2nd = true;
	}
	//no Voltage Alarm
	if ( (LipoVoltage / BatteryCells)> (Settings.LowVoltage1st + Settings.hysteresis) && VoltageAlarm==true)
	{
		VoltageAlarm = false;
		VoltageAlarm2nd = false;
	}

	if (firstloop<255)
	{
		VoltageAlarm = false;
		VoltageAlarm2nd = false;
	}

	//wait if OSD is not in sync
	while (!OSD.notInVSync());

	//Variable which is set, so only upon the first arming the message with the detected battery is being displayed
	if (armed == 1 && firstarmed == 0)
	{
		firstarmed == 1;
	}

	//Calculate Timer
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
		time = millis() - start_time + total_time;
	}
	armedOld = armed;

	//clearing the RPM and TEMP Arrays and then rewriting the nwe values
	for (i = 0; i<10; i++)
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
	ThrottlePos = print_int16(StickChanVals[0], Throttle, 0, 1);
	Throttle[ThrottlePos++] = '%';
	//
	CurrentPos = print_int16(current, Current, 1, 0);
	Current[CurrentPos++] = 'A';
	Current[CurrentPos++] = 'T';
	//
	KRPMPoses[0] = print_int16(motorKERPM[0], Motor1KERPM, 1, 1);
	Motor1KERPM[KRPMPoses[0]++] = 'K';
	Motor1KERPM[KRPMPoses[0]++] = 'R';
	//
	KRPMPoses[1] = print_int16(motorKERPM[1], Motor2KERPM, 1, 0);
	Motor2KERPM[KRPMPoses[1]++] = 'K';
	Motor2KERPM[KRPMPoses[1]++] = 'R';
	//
	KRPMPoses[2] = print_int16(motorKERPM[2], Motor3KERPM, 1, 0);
	Motor3KERPM[KRPMPoses[2]++] = 'K';
	Motor3KERPM[KRPMPoses[2]++] = 'R';
	//
	KRPMPoses[3] = print_int16(motorKERPM[3], Motor4KERPM, 1, 1);
	Motor4KERPM[KRPMPoses[3]++] = 'K';
	Motor4KERPM[KRPMPoses[3]++] = 'R';
	//
	CurrentPoses[0] = print_int16(motorCurrent[0], Motor1Current, 2, 1);
	Motor1Current[CurrentPoses[0]++] = 'A';
	//
	CurrentPoses[1] = print_int16(motorCurrent[1], Motor2Current, 2, 0);
	Motor2Current[CurrentPoses[1]++] = 'A';
	//
	CurrentPoses[2] = print_int16(motorCurrent[2], Motor3Current, 2, 0);
	Motor3Current[CurrentPoses[2]++] = 'A';
	//
	CurrentPoses[3] = print_int16(motorCurrent[3], Motor4Current, 2, 1);
	Motor4Current[CurrentPoses[3]++] = 'A';
	//
	TempPoses[0] = print_int16(ESCTemps[0], ESC1Temp, 0, 1);
	ESC1Temp[TempPoses[0]++] = '°';
	//
	TempPoses[1] = print_int16(ESCTemps[1], ESC2Temp, 0, 0);
	ESC2Temp[TempPoses[1]++] = '°';
	//
	TempPoses[2] = print_int16(ESCTemps[2], ESC3Temp, 0, 0);
	ESC3Temp[TempPoses[2]++] = '°';
	//
	TempPoses[3] = print_int16(ESCTemps[3], ESC4Temp, 0, 1);
	ESC4Temp[TempPoses[3]++] = '°';
}

void drawAngelIndicator(int8_t Value)
{
	if (Value>18)
		OSD.write(ANGEL_UP);
	else if (Value <= 18 && Value>7)
		OSD.write(ANGEL_LIGHTUP);
	else if (Value <= 7 && Value>1)
		OSD.write(ANGEL_MIDDLEUP);
	else if (Value <= 1 && Value >= -1)
		OSD.write(ANGEL_CLEAR);
	else if (Value<-1 && Value >= -7)
		OSD.write(ANGEL_MIDDLEDOWN);
	else if (Value<-7 && Value >= -18)
		OSD.write(ANGEL_LIGHTDOWN);
	else if (Value<-18)
		OSD.write(ANGEL_DOWN);
	else
		OSD.print(' ');
}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void OSDmakegrey() {
	OSD.clear();
	while (OSD.clearIsBusy()) {}
	OSD.grayBackground();
	for (int makegrey = 0; makegrey < 15; makegrey++)
	{
		OSD.setCursor(0, makegrey);
		OSD.print(F("                             "));
	}
}