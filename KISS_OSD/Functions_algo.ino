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
	static uint32_t tmpVoltage = 0;
	static uint32_t voltDev = 0;
	uint8_t minBytes = 100;
	uint8_t recBytes = 0;
	uint8_t exitreceiving = 0;

	Serial.write(0x20); // request telemetrie

	//aquire serial data and write it to normal variables
	while (exitreceiving==0)
	{
		//Running already to long in this loop
		if (micros() - KissStatus.LastLoopTime > 200000)
		{
			KissConnection = LostConnection;
			exitreceiving = 1;
		}

		//Copy all received data into buffer
		while (Serial.available())
			serialBuf[recBytes++] = Serial.read();

		//Check for start byte - if wrong exit 
		if (recBytes == 1 && serialBuf[0] != 5)
		{
			exitreceiving = 1;
			KissConnection = LostConnection;
		}
			

		//Check for transmission length
		if (recBytes == 2)
			minBytes = serialBuf[1] + STARTCOUNT + 1; // got the transmission length

		//All data is here
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
				KissData.armed = ((serialBuf[15 + STARTCOUNT] << 8) | serialBuf[16 + STARTCOUNT]);
				//Lipo Voltage from FC
				KissData.LipoVoltage = ((serialBuf[17 + STARTCOUNT] << 8) | serialBuf[18 + STARTCOUNT]);
				//failsafe
				KissData.failsafe = ((serialBuf[40 + STARTCOUNT] << 8) | (serialBuf[41 + STARTCOUNT])); //42
																							   //calib Done
				KissData.calibGyroDone = ((serialBuf[39 + STARTCOUNT] << 8) | serialBuf[40 + STARTCOUNT]);
				//angle y for displaying in the horizon bar
				KissData.angley = ((serialBuf[33 + STARTCOUNT] << 8) | (serialBuf[34 + STARTCOUNT])) / 100; //35

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
					KissData.LipoVoltage = (tmpVoltage / voltDev);
				KissData.LipoVoltage = KissData.LipoVoltage + Settings.VoltageOffset;

				//capacity

				//Settings.StandbyCurrent is @5V so Settings.StandbyCurrent*5V=mW
				//mW/CurrentVoltage=Current at actual Voltage
				KissData.standbyCurrentTotal += (((Settings.StandbyCurrent * 5) / KissData.LipoVoltage) / 36000000.0) * (2 * (micros() - KissStatus.LastLoopTime));


				KissData.LipoMAH = ((serialBuf[148 + STARTCOUNT] << 8) | serialBuf[149 + STARTCOUNT]);
				KissData.LipoMAH += KissData.standbyCurrentTotal;
				//read Motor Current and other ESC datas
				static uint32_t windedupfilterdatas[8];
				//RPM
				windedupfilterdatas[0] = ESC_filter((uint32_t)windedupfilterdatas[0], (uint32_t)((serialBuf[91 + STARTCOUNT] << 8) | serialBuf[92 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[1] = ESC_filter((uint32_t)windedupfilterdatas[1], (uint32_t)((serialBuf[101 + STARTCOUNT] << 8) | serialBuf[102 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[2] = ESC_filter((uint32_t)windedupfilterdatas[2], (uint32_t)((serialBuf[111 + STARTCOUNT] << 8) | serialBuf[112 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[3] = ESC_filter((uint32_t)windedupfilterdatas[3], (uint32_t)((serialBuf[121 + STARTCOUNT] << 8) | serialBuf[122 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				KissData.motorKERPM[0] = windedupfilterdatas[0] >> 4;
				KissData.motorKERPM[1] = windedupfilterdatas[1] >> 4;
				KissData.motorKERPM[2] = windedupfilterdatas[2] >> 4;
				KissData.motorKERPM[3] = windedupfilterdatas[3] >> 4;
				//Current
				windedupfilterdatas[4] = ESC_filter((uint32_t)windedupfilterdatas[4], (uint32_t)((serialBuf[87 + STARTCOUNT] << 8) | serialBuf[88 + STARTCOUNT]) << 4);
				windedupfilterdatas[5] = ESC_filter((uint32_t)windedupfilterdatas[5], (uint32_t)((serialBuf[97 + STARTCOUNT] << 8) | serialBuf[98 + STARTCOUNT]) << 4);
				windedupfilterdatas[6] = ESC_filter((uint32_t)windedupfilterdatas[6], (uint32_t)((serialBuf[107 + STARTCOUNT] << 8) | serialBuf[108 + STARTCOUNT]) << 4);
				windedupfilterdatas[7] = ESC_filter((uint32_t)windedupfilterdatas[7], (uint32_t)((serialBuf[117 + STARTCOUNT] << 8) | serialBuf[118 + STARTCOUNT]) << 4);
				KissData.motorCurrent[0] = windedupfilterdatas[4] >> 4;
				KissData.motorCurrent[1] = windedupfilterdatas[5] >> 4;
				KissData.motorCurrent[2] = windedupfilterdatas[6] >> 4;
				KissData.motorCurrent[3] = windedupfilterdatas[7] >> 4;
				//ESC Temps
				KissData.ESCTemps[0] = ((serialBuf[83 + STARTCOUNT] << 8) | serialBuf[84 + STARTCOUNT]);
				KissData.ESCTemps[1] = ((serialBuf[93 + STARTCOUNT] << 8) | serialBuf[94 + STARTCOUNT]);
				KissData.ESCTemps[2] = ((serialBuf[103 + STARTCOUNT] << 8) | serialBuf[104 + STARTCOUNT]);
				KissData.ESCTemps[3] = ((serialBuf[113 + STARTCOUNT] << 8) | serialBuf[114 + STARTCOUNT]);
				//Aux chan vals
				KissData.AuxChanVals[0] = ((serialBuf[8 + STARTCOUNT] << 8) | serialBuf[9 + STARTCOUNT]);
				KissData.AuxChanVals[1] = ((serialBuf[10 + STARTCOUNT] << 8) | serialBuf[11 + STARTCOUNT]);
				KissData.AuxChanVals[2] = ((serialBuf[12 + STARTCOUNT] << 8) | serialBuf[13 + STARTCOUNT]);
				KissData.AuxChanVals[3] = ((serialBuf[14 + STARTCOUNT] << 8) | serialBuf[15 + STARTCOUNT]);
				//StickChanVals
				KissData.StickChanVals[0] = ((serialBuf[0 + STARTCOUNT] << 8) | serialBuf[1 + STARTCOUNT]);
				KissData.StickChanVals[1] = ((serialBuf[2 + STARTCOUNT] << 8) | serialBuf[3 + STARTCOUNT]);
				KissData.StickChanVals[2] = ((serialBuf[4 + STARTCOUNT] << 8) | serialBuf[5 + STARTCOUNT]);
				KissData.StickChanVals[3] = ((serialBuf[6 + STARTCOUNT] << 8) | serialBuf[7 + STARTCOUNT]);
				//total current
				KissData.current = (uint16_t)(KissData.motorCurrent[0] + KissData.motorCurrent[1] + KissData.motorCurrent[2] + KissData.motorCurrent[3]) / 10;
				KissData.current += (((Settings.StandbyCurrent * 5) / KissData.LipoVoltage) / 100);
			}
			exitreceiving = 1;

			if(KissConnection == LostConnection || KissConnection == WaitingForConn)
				KissConnection = ConnectionEtablished;
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
		if (KissData.AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]<-250)
		{
			KissStatus.reducedModeDisplay = 1;
		}
		else if (KissData.AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]>250)
		{
			KissStatus.reducedModeDisplay = 2;
		}
		else KissStatus.reducedModeDisplay = 0;
	}

	//when display mode has changed clear the screen
	if (KissStatus.reducedModeDisplay != KissStatus.lastMode)
	{
		KissStatus.lastMode = KissStatus.reducedModeDisplay;
		OSD.clear();
		while(OSD.clearIsBusy())
		while (!OSD.notInVSync());
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispRCThrottle1 || KissStatus.reducedModeDisplay == 1 && Settings.DispRCThrottle2 || KissStatus.reducedModeDisplay == 2 && Settings.DispRCThrottle3)
	{
		OSD.setCursor(0, 0);
		OSD.write(SYM_THR);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.StickChanVals[0] / 10, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_PERC;
		OSD.print(TempCharConverted);
		ESCmarginTop = 1;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispCombCurrent1 || KissStatus.reducedModeDisplay == 1 && Settings.DispCombCurrent2 || KissStatus.reducedModeDisplay == 2 && Settings.DispCombCurrent3)
	{
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.current, TempCharConverted, 1, 0);
		TempCharConverted[TempCharPosition++] = 'A';
		TempCharConverted[TempCharPosition++] = 'T';
		OSD.setCursor(-TempCharPosition, 0);
		OSD.print(TempCharConverted);
	}

	

	if (Settings.stockSettings == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("STOCK SETT-OPEN MENU"));
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispLipoVoltage1 || KissStatus.reducedModeDisplay == 1 && Settings.DispLipoVoltage2 || KissStatus.reducedModeDisplay == 2 && Settings.DispLipoVoltage3)
	{
		if (KissStatus.VoltageAlarm == true)
		{
			OSD.blink();
			if (KissStatus.VoltageAlarm2nd == true)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("    LIPO VOLTAGE    "));
			}
		}
		OSD.setCursor(Settings.marginLastRow, -1);
		OSD.write(SYM_MAIN_BATT);
		ClearTempCharConverted();
		print_int16(KissData.LipoVoltage, TempCharConverted, 2, 1);
		OSD.print(TempCharConverted);
		OSD.noBlink();
		ESCmarginBot = 1;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispTimer1 || KissStatus.reducedModeDisplay == 1 && Settings.DispTimer2 || KissStatus.reducedModeDisplay == 2 && Settings.DispTimer3)
	{
		OSD.setCursor(12, -1);
		ClearTempCharConverted();
		print_time(KissStatus.time, TempCharConverted);
		OSD.write(SYM_FLY_M);
		OSD.print(TempCharConverted);
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispMaConsumption1 || KissStatus.reducedModeDisplay == 1 && Settings.DispMaConsumption2 || KissStatus.reducedModeDisplay == 2 && Settings.DispMaConsumption3)
	{

		if (KissData.LipoMAH>(Settings.Capacity * (float)Settings.Capacity1st) / 100 && Settings.Capacity>0)
		{
			OSD.blink();
			if (KissData.LipoMAH>(Settings.Capacity * (float)Settings.Capacity2nd) / 100)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("     CAPACITY       "));
			}
		}
		ClearTempCharConverted();
		
		TempCharPosition = print_int16(KissData.LipoMAH, TempCharConverted, 0, 1);
		OSD.setCursor(-(TempCharPosition + 1 + Settings.marginLastRow), -1);
		OSD.write(SYM_MAH);
		OSD.print(TempCharConverted);
		OSD.noBlink();
		ESCmarginBot = 1;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispEscKrpm1 || KissStatus.reducedModeDisplay == 1 && Settings.DispEscKrpm2 || KissStatus.reducedModeDisplay == 2 && Settings.DispEscKrpm3)
	{

		//print motor 1
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorKERPM[0], TempCharConverted, 1, 1);
		OSD.setCursor(0, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorKERPM[1], TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorKERPM[3], TempCharConverted, 1, 1);
		OSD.setCursor(0, -(1 + ESCmarginBot));
		OSD.print(TempCharConverted);

		//print motor 3
		TempCharPosition = print_int16(KissData.motorKERPM[2], TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition, -(1 + ESCmarginBot));
		OSD.print(TempCharConverted);
		ClearTempCharConverted();

		TMPmargin++;
		CurrentMargin++;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispEscCurrent1 || KissStatus.reducedModeDisplay == 1 && Settings.DispEscCurrent2 || KissStatus.reducedModeDisplay == 2 && Settings.DispEscCurrent3)
	{
		//current 1
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorCurrent[0], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorCurrent[1], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorCurrent[3], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		//current3
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.motorCurrent[2], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		TMPmargin++;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispEscTemp1 || KissStatus.reducedModeDisplay == 1 && Settings.DispEscTemp2 || KissStatus.reducedModeDisplay == 2 && Settings.DispEscTemp3)
	{
		//temp1
		
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.ESCTemps[0], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.ESCTemps[1], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.ESCTemps[3], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, -(1 + TMPmargin + ESCmarginBot));
		TempCharConverted[7] = '  ';
		OSD.print(TempCharConverted);

		//temp3
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.ESCTemps[2], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, -(1 + TMPmargin + ESCmarginBot));
		TempCharConverted[7] = '  ';
		OSD.print(TempCharConverted);

	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispAngle1 || KissStatus.reducedModeDisplay == 1 && Settings.DispAngle2 || KissStatus.reducedModeDisplay == 2 && Settings.DispAngle3)
	{
		OSD.setCursor(0, 6);
		drawAngelIndicator(KissData.angley);
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispPilotname1 || KissStatus.reducedModeDisplay == 1 && Settings.DispPilotname2 || KissStatus.reducedModeDisplay == 2 && Settings.DispPilotname3)
	{
		OSD.setCursor(10, 0);
		OSD.print(Pilotname);
	}

	//show the detected cell count upon the first 30 flight-sec if not armed
	if (KissData.current == 0 && KissStatus.BatteryCells != 0 && KissData.armed == 0 && KissStatus.firstarmed == 0 && KissStatus.time<30000)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		if (KissStatus.BatteryCells <1 || KissStatus.BatteryCells>6)
		{
			OSD.print(F("ERR BAT CELLS: "));
		}
		else
		{
			switch (KissStatus.BatteryCells)
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



	//show armed | DISARMED
	if (KissData.armed == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("     DISARMED      "));
		KissStatus.armedstarted = millis();
	}
	else if (KissStatus.armedstarted + 2000>millis())
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.blink();
		OSD.print(F("       ARMED        "));
		OSD.noBlink();
	}
	else if (KissStatus.firstarmed == 0)
	{
		KissStatus.firstarmed == 1;
	}

	if (KissData.failsafe>5)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("      FAILSAFE      "));
	}

	if (KissData.calibGyroDone>100)
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
	if (KissData.LipoVoltage>200)
	{
		//check the battery cells to display the correct alarm later
		uint16_t tempVoltage = KissData.LipoVoltage;
		KissStatus.BatteryCells = 1;
		while (tempVoltage > 440)
		{
			KissStatus.BatteryCells = KissStatus.BatteryCells + 1;
			tempVoltage = tempVoltage - 440;
		}
		//firstloop = 255;
	}
	//Voltage Alarm 1 and 2
	if ((KissData.LipoVoltage / KissStatus.BatteryCells)<Settings.LowVoltage1st && Settings.LowVoltageAllowed == 1)
	{

		KissStatus.VoltageAlarm = true;
	}
	if ((KissData.LipoVoltage / KissStatus.BatteryCells)<Settings.LowVoltage2nd && KissStatus.VoltageAlarm == true)
	{
		KissStatus.VoltageAlarm2nd = true;
	}
	//no Voltage Alarm
	if ((KissData.LipoVoltage / KissStatus.BatteryCells)> (Settings.LowVoltage1st + Settings.hysteresis) && KissStatus.VoltageAlarm == true)
	{
		KissStatus.VoltageAlarm = false;
		KissStatus.VoltageAlarm2nd = false;
	}

	//wait if OSD is not in sync
	while (!OSD.notInVSync());

	//Variable which is set, so only upon the first arming the message with the detected battery is being displayed
	if (KissData.armed == 1 && KissStatus.firstarmed == 0)
	{
		KissStatus.firstarmed == 1;
	}

	//Calculate Timer
	// switch disarmed => armed
	if (KissStatus.armedOld == 0 && KissData.armed > 0)
	{
		KissStatus.start_time = millis();
	}
	// switch armed => disarmed
	else if (KissStatus.armedOld > 0 && KissData.armed == 0)
	{
		KissStatus.total_time = KissStatus.total_time + (millis() - KissStatus.start_time);
	}
	else if (KissData.armed > 0)
	{
		KissStatus.time = millis() - KissStatus.start_time + KissStatus.total_time;
	}
	KissStatus.armedOld = KissData.armed;

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
	for (uint8_t i = 0; i < 30; i++)
	{
		TempCharConverted[i] = ' ';
	}
}

void WaitForKissFc()
{
	OSD.clear();
	while (OSD.clearIsBusy())
		{}
	OSD.setCursor(9, 0);
	OSD.print(F("SAMUD OSD"));
	OSD.setCursor(6, 1);
	OSD.print(F("CUSTOM KISS OSD"));
	
	
	OSD.setCursor(0, 14);
	if (KissConnection == WaitingForConn)
	{
		OSD.print(F("WAITING FOR KISS FC...  "));
		OSD.setCursor(5, 2);
		OSD.print(F("ENJOY YOUR FLIGHT"));
	}
	else
		OSD.print(F("LOST COMMUNICATION WITH FC...  "));
}

void FlightSummary()
{
	//show stats:   MaxCurrentTotal,	Min Voltage	
	//				ESTTemp,			ESCCurrent		ESCRPM
	//				Time				MAHUsed
	if (KissStatus.lastMode!=4)
	{
		OSD.clear();
		while (OSD.clearIsBusy())
		{
		}
		OSD.setCursor(0, 0);
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - FLIGHT STATISTIC "));
		OSD.videoBackground();

		//Current
		OSD.setCursor(0, 2);
		OSD.print(F("MAX CURRENT:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MaxCurrentTotal, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, 2);
		OSD.print(TempCharConverted);

		//Voltage
		OSD.setCursor(0, 3);
		OSD.print(F("MIN VOLTAGE:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MinVoltage/KissStatus.BatteryCells, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'V';
		OSD.setCursor(-TempCharPosition, 3);
		OSD.print(TempCharConverted);

		//Voltage
		OSD.setCursor(0, 4);
		OSD.print(F("MAX POWER:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXWatt, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'W';
		OSD.setCursor(-TempCharPosition, 4);
		OSD.print(TempCharConverted);

		//Consumption
		OSD.setCursor(0, 5);
		OSD.print(F("CONSUMPTION:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissData.LipoMAH, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = 'M';
		TempCharConverted[TempCharPosition++] = 'A';
		TempCharConverted[TempCharPosition++] = 'H';
		OSD.setCursor(-TempCharPosition, 5);
		OSD.print(TempCharConverted);

		//Flight Time
		OSD.setCursor(0, 6);
		OSD.print(F("FLIGHT TIME:"));
		OSD.setCursor(-2, 5);
		OSD.print(F("00"));
		ClearTempCharConverted();
		OSD.setCursor(24, 6);
		print_time(KissStatus.time, TempCharConverted);
		OSD.print(TempCharConverted);

		//ESC Data
		OSD.setCursor(0, 9);
		OSD.grayBackground();
		OSD.print(F("ESC DATA - MAX VALUES       "));
		OSD.videoBackground();
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXESCTemp, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, 10);
		OSD.print(TempCharConverted);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXmotorCurrent, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(10, 10);
		OSD.print(TempCharConverted);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXmotorCurrent, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'K';
		TempCharConverted[TempCharPosition++] = 'R';
		TempCharConverted[TempCharPosition++] = 'P';
		TempCharConverted[TempCharPosition++] = 'M';
		OSD.setCursor(-TempCharPosition, 9);
		OSD.print(TempCharConverted);

		KissStatus.lastMode = 4;
	}
	

	

}

void FlightSummaryCalculate()
{
	FlightSummaryCalculateFnc(&KissStats.MaxCurrentTotal, &KissData.current,1);
	if(KissData.LipoVoltage>100)
		FlightSummaryCalculateFnc(&KissStats.MinVoltage, &KissData.LipoVoltage, 0);
	//FlightSummaryCalculateFnc(&KissStats.MAXESCTemp[0], &KissData.LipoVoltage, 0);

	if (KissData.current*KissData.LipoVoltage > KissStats.MAXWatt)
		KissStats.MAXWatt = KissData.current*KissData.LipoVoltage;
	if (KissData.ESCTemps[0] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissData.ESCTemps[0];
	if (KissData.ESCTemps[1] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissData.ESCTemps[1];
	if (KissData.ESCTemps[2] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissData.ESCTemps[2];
	if (KissData.ESCTemps[3] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissData.ESCTemps[3];

	if (KissData.motorCurrent[0] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissData.motorCurrent[0];
	if (KissData.motorCurrent[1] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissData.motorCurrent[1];
	if (KissData.motorCurrent[2] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissData.motorCurrent[2];
	if (KissData.motorCurrent[3] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissData.motorCurrent[3];

	if (KissData.motorKERPM[0] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissData.motorKERPM[0];
	if (KissData.motorKERPM[1] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissData.motorKERPM[1];
	if (KissData.motorKERPM[2] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissData.motorKERPM[2];
	if (KissData.motorKERPM[3] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissData.motorKERPM[3];

	//show stats:

}

void FlightSummaryCalculateFnc(uint16_t *Stat,uint16_t *Value,uint8_t bigger )
{
	if ((bigger == 1 && *Value > *Stat) || bigger == 0 && *Value < *Stat)
		*Stat = *Value;
}
void FlightSummaryCalculateFnc(int16_t *Stat, int16_t *Value, uint8_t bigger)
{
	if ((bigger == 1 && *Value > *Stat) || bigger == 0 && *Value < *Stat)
		*Stat = *Value;
}