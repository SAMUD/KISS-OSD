//===============
//Display the OSD
void DisplayOSD_Main()
{
	//Declaring some vars
	uint8_t ESCmarginBot = 0;
	uint8_t ESCmarginTop = 0;
	uint8_t TMPmargin = 0;
	uint8_t CurrentMargin = 0;
	uint8_t MarginMiddleY = 4;
	uint8_t MarginMiddleY_Old = 6;

	//reduced mode selection
	if (Settings.RED_MODE_AUX_CHANNEL != 0)
	{
		if (KissTelemetrie.AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]<-333)
			KissStatus.reducedModeDisplay = 1;
		else if (KissTelemetrie.AuxChanVals[Settings.RED_MODE_AUX_CHANNEL - 1]>333)
			KissStatus.reducedModeDisplay = 2;
		else
			KissStatus.reducedModeDisplay = 0;
	}

	//when display mode has changed clear the screen
	if (KissStatus.reducedModeDisplay != KissStatus.lastMode)
	{
		KissStatus.lastMode = KissStatus.reducedModeDisplay;
		OSD.clear();
		while (OSD.clearIsBusy() || !OSD.notInVSync());
	}

	//*******************************
	//*start displaying the OSD-data*
	//*******************************

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispChannel1 || KissStatus.reducedModeDisplay == 1 && Settings.DispChannel2 || KissStatus.reducedModeDisplay == 2 && Settings.DispChannel3)
	{
		OSD.home();
		ClearTempCharConverted();
		if (Settings.ChannelSelectDisp == 0)		//selector to show either Throttle or Aux 1-4
		{
			//we want to show Throttle
			OSD.write(SYM_THR);
			TempCharPosition = print_int16(KissTelemetrie.StickChanVals[0] / 10, TempCharConverted, 0, 1);
		}
		else
		{
			//we want to show Aux1-4
			OSD.write(SYM_RSSI);	//at the moment we only support RSSI with a value from 0 to 100%. db will be added later
			TempCharPosition = print_int16( ((KissTelemetrie.AuxChanVals[Settings.ChannelSelectDisp-1]/10)+100)/2, TempCharConverted, 0, 1);

		}
		TempCharConverted[TempCharPosition++] = SYM_PERC;
		OSD.print(TempCharConverted);
		ESCmarginTop = 1;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispCombCurrent1 || KissStatus.reducedModeDisplay == 1 && Settings.DispCombCurrent2 || KissStatus.reducedModeDisplay == 2 && Settings.DispCombCurrent3)
	{
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.current, TempCharConverted, 1, 0);
		TempCharConverted[TempCharPosition++] = 'A';
		TempCharConverted[TempCharPosition++] = 'T';
		OSD.setCursor(-TempCharPosition, 0);
		OSD.print(TempCharConverted);
		ESCmarginTop = 1;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispLipoVoltage1 || KissStatus.reducedModeDisplay == 1 && Settings.DispLipoVoltage2 || KissStatus.reducedModeDisplay == 2 && Settings.DispLipoVoltage3)
	{
		//if the quad is disarmed it will show the total tension and the per cell in a 2sec intervall
		//calculation is done in the main calculation fnc
		ClearTempCharConverted();
		if (KissStatus.VoltageAlarm > 0)
			OSD.blink();

		if (KissStatus.VoltageDisplayingCell == true )
		{
			//show the cell Voltage
			if (KissTelemetrie.LipoVoltage / KissStatus.BatteryCells<410 && KissStatus.time<1000)
				OSD.blink();
			print_int16(KissTelemetrie.LipoVoltage / KissStatus.BatteryCells, TempCharConverted, 2, 1);
		}
		else
			//show the normal Voltage
			print_int16(KissTelemetrie.LipoVoltage, TempCharConverted, 2, 1);
		OSD.setCursor(Settings.marginLastRow, -1);
		OSD.write(SYM_MAIN_BATT);
		OSD.print(TempCharConverted);
		ESCmarginBot = 1;

		//2nd stage Voltage alarm
		if (KissStatus.VoltageAlarm == 2)
		{
			OSD.setCursor(4, MarginMiddleY);
			MarginMiddleY++;
			OSD.print(F("    LIPO VOLTAGE    "));
		}
		OSD.noBlink();
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
		if (KissTelemetrie.LipoMAH>(Settings.Capacity * (float)Settings.Capacity1st) / 100 && Settings.Capacity>0)
		{
			OSD.blink();
			if (KissTelemetrie.LipoMAH>(Settings.Capacity * (float)Settings.Capacity2nd) / 100)
			{
				OSD.setCursor(4, MarginMiddleY);
				MarginMiddleY++;
				OSD.print(F("     CAPACITY       "));
			}
		}
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.LipoMAH, TempCharConverted, 0, 1);
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
		TempCharPosition = print_int16(KissTelemetrie.motorKERPM[0], TempCharConverted, 1, 1);
		OSD.setCursor(0, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.motorKERPM[1], TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition, ESCmarginTop);
		OSD.print(TempCharConverted);

		//print motor 4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.motorKERPM[3], TempCharConverted, 1, 1);
		OSD.setCursor(0, -(1 + ESCmarginBot));
		OSD.print(TempCharConverted);

		//print motor 3
		TempCharPosition = print_int16(KissTelemetrie.motorKERPM[2], TempCharConverted, 1, 1);
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
		TempCharPosition = print_int16(KissTelemetrie.motorCurrent[0], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current 2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.motorCurrent[1], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, CurrentMargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//current4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.motorCurrent[3], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(0, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		//current3
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.motorCurrent[2], TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, -(1 + CurrentMargin + ESCmarginBot));
		OSD.print(TempCharConverted);

		TMPmargin++;
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispEscTemp1 || KissStatus.reducedModeDisplay == 1 && Settings.DispEscTemp2 || KissStatus.reducedModeDisplay == 2 && Settings.DispEscTemp3)
	{
		//temp1

		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.ESCTemps[0], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp2
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.ESCTemps[1], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, TMPmargin + ESCmarginTop);
		OSD.print(TempCharConverted);

		//temp4
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.ESCTemps[3], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, -(1 + TMPmargin + ESCmarginBot));
		TempCharConverted[7] = '  ';
		OSD.print(TempCharConverted);

		//temp3
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.ESCTemps[2], TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(-TempCharPosition, -(1 + TMPmargin + ESCmarginBot));
		TempCharConverted[7] = '  ';
		OSD.print(TempCharConverted);

	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispAngle1 || KissStatus.reducedModeDisplay == 1 && Settings.DispAngle2 || KissStatus.reducedModeDisplay == 2 && Settings.DispAngle3)
	{
		OSD.setCursor(0, 5);
		drawAngelIndicator(KissTelemetrie.angley);
	}

	if (KissStatus.reducedModeDisplay == 0 && Settings.DispPilotname1 || KissStatus.reducedModeDisplay == 1 && Settings.DispPilotname2 || KissStatus.reducedModeDisplay == 2 && Settings.DispPilotname3)
	{
		OSD.setCursor(9, 0);
		for (i = 0; i < 11; i++)
			OSD.write(Settings.PilotName[i]);
	}


	//**********************************
	//*start displaying the Info-center*
	//**********************************

	//show the detected cell count upon the first 30 flight-sec if not armed
	if (KissStatus.BatteryCells != 0 && KissTelemetrie.armed == 0 && KissStatus.time<30000)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		if (KissStatus.BatteryCells>6)
			OSD.print(F("ERR BAT CELLS: "));
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

	if (Settings.stockSettings == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("STOCK SETT-OPEN MENU"));
	}

	//show armed | DISARMED
	if (KissTelemetrie.armed == 0)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("      DISARMED     "));
	}
	else if (KissStatus.start_time + 2000>millis())
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.blink();
		OSD.print(F("       ARMED        "));
		OSD.noBlink();
	}

	if (KissTelemetrie.failsafe>5)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("      FAILSAFE      "));
	}

	if (KissTelemetrie.calibGyroDone>100)
	{
		OSD.setCursor(4, MarginMiddleY);
		MarginMiddleY++;
		OSD.print(F("  CALIBRATING GYRO  "));
	}

	//clearing middle screen below Info Center
	i = 0;
	while ((MarginMiddleY + i) <= 8)
	{
		OSD.setCursor(4, MarginMiddleY + i);
		OSD.print(F("                    "));
		i++;
	}
}

#ifdef DEBUG
	void Debug_Fnc(char Message[28])
	{
		static uint8_t line = 0;

		line++;
		if (line > 10)
			line = 0;

		OSD.grayBackground();
		OSD.setCursor(0, line);
		OSD.print(Message);
		delay(100);
		OSD.videoBackground();

	}
#endif

void DisplayLPF(uint8_t Value)
{
	switch (Value)
	{
	case 0: OSD.print(F("OFF "));
		break;
	case 1: OSD.print(F("HIGH  "));
		break;
	case 2: OSD.print(F("MED HI"));
		break;
	case 3: OSD.print(F("MEDIUM"));
		break;
	case 4: OSD.print(F("MED LO"));
		break;
	case 5: OSD.print(F("LOW   "));
		break;
	case 6: OSD.print(F("LOWEST"));
		break;

	}
}


void DisplaySpace()
{
	OSD.print(F(" "));
}

void DisplayVTXChannel()
{
	/*Channel A from 0 to 7
	Channel B from 8 to 15
	Channel E from 16 to 23
	FatShark  from 24 to 31
	Race      from 32 to 39*/

	if (KissSettings.VTXChannel < 8)
	{
		OSD.print(F("A"));
		OSD.print(KissSettings.VTXChannel+1);
	}
		
	else if (KissSettings.VTXChannel < 16)
	{
		OSD.print(F("B"));
		OSD.print(KissSettings.VTXChannel - 7);
	}
	else if (KissSettings.VTXChannel < 24)
	{
		OSD.print(F("E"));
		OSD.print(KissSettings.VTXChannel - 15);
	}
	else if (KissSettings.VTXChannel < 32)
	{
		OSD.print(F("FS"));
		OSD.print(KissSettings.VTXChannel - 23);
	}
	else
	{
		OSD.print(F("RACE"));
		OSD.print(KissSettings.VTXChannel - 31);
	}

	DisplaySpace();
	DisplaySpace();

}

void DisplayRecover()
{
	OSD.setCursor(0, 2);
	OSD.grayBackground();
	OSD.print(F("RESTORE BATTERY WITH"));
	DisplaySpace();
	ClearTempCharConverted();
	TempCharPosition = print_int16(Settings.SavedCurrBat.BatteryMAH, TempCharConverted, 0, 1);
	OSD.write(SYM_MAH);
	OSD.print(TempCharConverted);
	DisplaySpace();
	DisplaySpace();
	DisplaySpace();
	DisplaySpace();
	OSD.videoBackground();
	

	OSD.setCursor(0, 4);
	OSD.print(F("YES: PITCH+"));
	OSD.setCursor(0, 5);
	OSD.print(F("NO:  PITCH- OR START FLYING"));

	if (KissTelemetrie.StickChanVals[2] > 800)
	{
		//YES
		KissStatus.addMAH = Settings.SavedCurrBat.BatteryMAH;
		KissStatus.total_time = Settings.SavedCurrBat.total_time;

		KissStats = Settings.SavedCurrBat.SavedStats;
	}
	else if (KissTelemetrie.StickChanVals[2] < -800)
	{
		//NO
		KissStatus.total_time = 1;
	}
	KissStatus.lastMode = 10;	//clear the Display when showing normal stuff


}
