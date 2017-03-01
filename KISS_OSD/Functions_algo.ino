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

				//Settings.StandbyCurrent is @5V so Settings.StandbyCurrent*5V=mW
				//mW/CurrentVoltage=Current at actual Voltage
				standbyCurrentTotal += (((Settings.StandbyCurrent * 5) / LipoVoltage) / 36000000.0) * (2 * (micros() - LastLoopTime));


				LipoMAH = ((serialBuf[148 + STARTCOUNT] << 8) | serialBuf[149 + STARTCOUNT]);
				LipoMAH += standbyCurrentTotal;
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
				current += (((Settings.StandbyCurrent * 5) / LipoVoltage) / 100);
			}
		}
	} //end of aquiring telemetry data
}

//===============
//Display the OSD
void DisplayOSD()
{
	//Declaring some vars


	uint8_t ESCmarginBot = 0;
	uint8_t ESCmarginTop = 0;
	uint8_t TMPmargin = 0;
	uint8_t CurrentMargin = 0;
	uint8_t MarginMiddleY = 6;
	uint8_t MarginMiddleY_Old = 6;

	//reduced mode selection
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
		OSD.clear();
		while(OSD.clearIsBusy())
		while (!OSD.notInVSync());
	}

	if (reducedModeDisplay == 0 && Settings.DispRCThrottle1 || reducedModeDisplay == 1 && Settings.DispRCThrottle2 || reducedModeDisplay == 2 && Settings.DispRCThrottle3)
	{
		OSD.setCursor(0, 0);
		OSD.write(SYM_THR);
		ClearTempCharConverted();
		TempCharPosition = print_int16(StickChanVals[0] / 10, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_PERC;
		OSD.print(TempCharConverted);
		ESCmarginTop = 1;
	}

	if (reducedModeDisplay == 0 && Settings.DispCombCurrent1 || reducedModeDisplay == 1 && Settings.DispCombCurrent2 || reducedModeDisplay == 2 && Settings.DispCombCurrent3)
	{
		ClearTempCharConverted();
		TempCharPosition = print_int16(current, TempCharConverted, 1, 0);
		TempCharConverted[TempCharPosition++] = 'A';
		TempCharConverted[TempCharPosition++] = 'T';
		OSD.setCursor(-TempCharPosition, 0);
		OSD.print(TempCharConverted);
	}

	if (reducedModeDisplay == 0 && Settings.DispPilotname1 || reducedModeDisplay == 1 && Settings.DispPilotname2 || reducedModeDisplay == 2 && Settings.DispPilotname3)
	{
		OSD.setCursor(10, 0);
		OSD.print(Pilotname);
	}

	if (Settings.stockSettings == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("STOCK SETT-OPEN MENU"));
	}

	if (reducedModeDisplay == 0 && Settings.DispLipoVoltage1 || reducedModeDisplay == 1 && Settings.DispLipoVoltage2 || reducedModeDisplay == 2 && Settings.DispLipoVoltage3)
	{
		if (VoltageAlarm == true)
		{
			OSD.blink();
			if (VoltageAlarm2nd == true)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("    LIPO VOLTAGE    "));
			}
		}
		OSD.setCursor(Settings.marginLastRow, -1);
		OSD.write(SYM_MAIN_BATT);
		ClearTempCharConverted();
		print_int16(LipoVoltage, TempCharConverted, 2, 1);
		OSD.print(TempCharConverted);
		OSD.noBlink();
		ESCmarginBot = 1;
	}

	if (reducedModeDisplay == 0 && Settings.DispTimer1 || reducedModeDisplay == 1 && Settings.DispTimer2 || reducedModeDisplay == 2 && Settings.DispTimer3)
	{
		OSD.setCursor(12, -1);
		ClearTempCharConverted();
		print_time(time, TempCharConverted);
		OSD.write(SYM_FLY_M);
		OSD.print(TempCharConverted);
	}

	if (reducedModeDisplay == 0 && Settings.DispMaConsumption1 || reducedModeDisplay == 1 && Settings.DispMaConsumption2 || reducedModeDisplay == 2 && Settings.DispMaConsumption3)
	{

		if (LipoMAH>(Settings.Capacity * (float)Settings.Capacity1st) / 100 && Settings.Capacity>0)
		{
			OSD.blink();
			if (LipoMAH>(Settings.Capacity * (float)Settings.Capacity2nd) / 100)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("     CAPACITY       "));
			}
		}
		ClearTempCharConverted();
		
		TempCharPosition = print_int16(LipoMAH, TempCharConverted, 0, 1);
		OSD.setCursor(-(TempCharPosition + 1 + Settings.marginLastRow), -1);
		OSD.write(SYM_MAH);
		OSD.print(TempCharConverted);
		OSD.noBlink();
		ESCmarginBot = 1;
	}

	if (reducedModeDisplay == 0 && Settings.DispEscKrpm1 || reducedModeDisplay == 1 && Settings.DispEscKrpm2 || reducedModeDisplay == 2 && Settings.DispEscKrpm3)
	{

		//print motor 1
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorKERPM[0], TempCharConverted, 1, 1);
		OSD.setCursor(0, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorKERPM[1], TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 4
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorKERPM[3], TempCharConverted, 1, 1);
		OSD.setCursor(0, -(1 + ESCmarginBot));
		OSD.print(TempCharConverted);

		//print motor 3
		TempCharPosition = print_int16(motorKERPM[2], TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition, -(1 + ESCmarginBot));
		OSD.print(TempCharConverted);
		ClearTempCharConverted();

		TMPmargin++;
		CurrentMargin++;
	}

	if (reducedModeDisplay == 0 && Settings.DispEscCurrent1 || reducedModeDisplay == 1 && Settings.DispEscCurrent2 || reducedModeDisplay == 2 && Settings.DispEscCurrent3)
	{
		//current 1
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorCurrent[0], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorCurrent[1], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current4
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorCurrent[3], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		//current3
		ClearTempCharConverted();
		TempCharPosition = print_int16(motorCurrent[2], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		TMPmargin++;
	}

	if (reducedModeDisplay == 0 && Settings.DispEscTemp1 || reducedModeDisplay == 1 && Settings.DispEscTemp2 || reducedModeDisplay == 2 && Settings.DispEscTemp3)
	{
		//temp1
		ClearTempCharConverted();
		TempCharPosition = print_int16(ESCTemps[0], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp2
		ClearTempCharConverted();
		TempCharPosition = print_int16(ESCTemps[1], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp4
		ClearTempCharConverted();
		TempCharPosition = print_int16(ESCTemps[3], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, -(1 + TMPmargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		//temp3
		ClearTempCharConverted();
		TempCharPosition = print_int16(ESCTemps[2], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, -(1 + TMPmargin + ESCmarginBot));
		OSD.print(TempCharConverted);

	}

	if (reducedModeDisplay == 0 && Settings.DispAngle1 || reducedModeDisplay == 1 && Settings.DispAngle2 || reducedModeDisplay == 2 && Settings.DispAngle3)
	{
		OSD.setCursor(0, 6);
		drawAngelIndicator(angley);
	}

	//show the detected cell count upon the first 30 flight-sec if not armed
	if (current == 0 && BatteryCells != 0 && armed == 0 && firstarmed == 0 && time<30000)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		if (BatteryCells <1 || BatteryCells>6)
		{
			OSD.print(F("ERR BAT CELLS: "));
		}
		else
		{
			switch (BatteryCells)
			{
			case 1: OSD.print(F("1"));
				break;
			case 2: OSD.print(F("2"));
				break;
			case 3: OSD.print(F("3"));
				break;
			case 4: OSD.print(F("4"));
				break;
			case 5: OSD.print(F("5"));
				break;
			case 6: OSD.print(F("6"));
				break;
			}
			OSD.print(F("S BATTERY CONNECTED"));
		}
	}

	/*if (firstloop<255)
	{
	OSD.setCursor(4, MarginMiddleY);
	MarginMiddleY++;
	OSD.blink();
	OSD.print(F("WAIT - DON'T ARM: "));
	OSD.noBlink();
	OSD.print(percent);
	}*/

	//show armed | DISARMED
	if (armed == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("     DISARMED      "));
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
	/*if (firstloop<254 && LipoVoltage>200)
	{
	//generating a delay, because voltage reading is inaccurate in the beginning.
	firstloop++;
	percent = (firstloop * 100) / 255; //for showing percentage in osd
	}*/
	if (LipoVoltage>200)
	{
		//check the battery cells to display the correct alarm later
		uint16_t tempVoltage = LipoVoltage;
		BatteryCells = 1;
		while (tempVoltage > 440)
		{
			BatteryCells = BatteryCells + 1;
			tempVoltage = tempVoltage - 440;
		}
		//firstloop = 255;
	}
	//Voltage Alarm 1 and 2
	if ((LipoVoltage / BatteryCells)<Settings.LowVoltage1st && Settings.LowVoltageAllowed == 1)
	{

		VoltageAlarm = true;
	}
	if ((LipoVoltage / BatteryCells)<Settings.LowVoltage2nd && VoltageAlarm == true)
	{
		VoltageAlarm2nd = true;
	}
	//no Voltage Alarm
	if ((LipoVoltage / BatteryCells)> (Settings.LowVoltage1st + Settings.hysteresis) && VoltageAlarm == true)
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

void ClearTempCharConverted()
{
	for (uint8_t i = 0; i < 15; i++)
	{
		TempCharConverted[i] = ' ';
	}
}

void WaitForKissFc()
{
	OSD.setCursor(9, 0);
	OSD.print(F("SAMUD OSD"));
	OSD.setCursor(6, 1);
	OSD.print(F("CUSTOM KISS OSD"));
	OSD.setCursor(5, 2);
	OSD.print(F("ENJOY YOUR FLIGHT"));
	OSD.setCursor(0, 13);
	OSD.print(F("USING "));
	if (OSD.videoSystem() == 1)
		OSD.print(F("PAL"));
	else if (OSD.videoSystem() == 2)
		OSD.print(F("NTSC"));
	OSD.setCursor(0, 14);
	OSD.print(F("WAITING FOR KISS FC...  "));
	for (int i = 0; i < 10; i++)
	{
		wdt_reset();
		delay(1000);
	}
	OSD.clear();
}