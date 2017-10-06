
/* 1. Information
//=========================================================================================================================

Fonctions needed for the settings-menu off the OSD.

//=========================================================================================================================
//START OF CODE
*/

// TODO Add the following declarations as non-static into the while if possible
static uint8_t MenuPage = 1;
static uint8_t OldMenuPage = 0;
static uint8_t cursorlineMax = 0;
static uint8_t cursorline = 0;
static uint8_t cursorlineOLD = 0;
static uint8_t pause = 0; //makes a pause of multiples of 50ms

void MenuRight_Main()
{
	boolean exitmenu = false;
	Menuall_start(0);

	while (!exitmenu && KissTelemetrie.armed == 0)
	{

		if (micros() - KissStatus.LastLoopTime > 10000) //limits the speed of the OSD to 20Hz  millis() - LastLoopTimeMenu > 100
		{
			KissStatus.LastLoopTime = micros();

			if (getSerialData(GET_TELEMETRY, true))			//requesting serial data
			{
				//received data is ok
				KissStatus.GetTelemetryErrorCount = 0;
			}
			else
			{
				KissStatus.GetTelemetryErrorCount = +1;
				if (KissStatus.GetTelemetryErrorCount > 3)
					KissConnection = LostConnection;
			}

			//set cursor position
			if (KissTelemetrie.StickChanVals[2] < -800 && cursorline<cursorlineMax)
			{
				cursorline++;
				KissTelemetrie.StickChanVals[2] = 0;
				pause = 5;
			}
			if (KissTelemetrie.StickChanVals[2] > 800 && cursorline>1)
			{
				cursorline--;
				KissTelemetrie.StickChanVals[2] = 0;
				pause = 5;
			}

			//changevalue
			if (KissTelemetrie.StickChanVals[1] > 800)
				value(true);
			if (KissTelemetrie.StickChanVals[1] < -800)
				value(false);

			//changepage
			if (KissTelemetrie.StickChanVals[3] > 800)
			{
				if (MenuPage<7)
				{
					MenuPage++;
					KissTelemetrie.StickChanVals[3] = 0;
					pause = 5;
				}
				else
					exitmenu = true;
			}
			if (KissTelemetrie.StickChanVals[3] < -800)
			{
				if (MenuPage > 1)
				{
					MenuPage--;
					KissTelemetrie.StickChanVals[3] = 0;
					pause = 5;
				}
				else
					exitmenu = true;
			}


			//redraw menu site
			if (MenuPage != OldMenuPage)
			{
				menuprintsite();
				OldMenuPage = MenuPage;
			}

			//redraw value
			menuprintvalue();

			//draw cursor position
			if (cursorline != cursorlineOLD && MenuPage != 7)
			{
				if (cursorline > 8 && MenuPage == 6)
				{
					OSD.setCursor(22, 9);
					OSD.print(F(" "));
					//remove old cursor
					OSD.setCursor(8 + cursorlineOLD, 11);
					OSD.print(F(" "));
					//write new one
					OSD.setCursor(8 + cursorline, 11);
					OSD.write(SYM_CURSOR_UP);
				}
				else
				{
					OSD.setCursor(17, 11);
					OSD.print(F(" "));
					OSD.setCursor(22, cursorlineOLD + 1);
					OSD.print(F(" "));
					OSD.setCursor(22, cursorline + 1);
					OSD.print(F(">"));
				}
				cursorlineOLD = cursorline;
			}

			//pause (multiples of 50ms)
			while (pause >0)
			{
				delay(50);
				pause--;
			}
		}
		//reset wdt
		//wdt_reset();
	}

	MenuAll_Exit(GET_TELEMETRY);

}

//printing the actual menu page
void menuprintsite() {

	OSD.clear();
	while (OSD.clearIsBusy());
	OSD.home();
	cursorline = 1;
	cursorlineOLD = 0;

	switch (MenuPage)
	{
	case 1:
		//VoltageSite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P1/7 VOLTAGE     "));
		OSD.setCursor(0, -1);
		OSD.print(F(" <-YAW-> : PAGE / EXIT       "));
		OSD.videoBackground();
		OSD.setCursor(0, 2);
		OSD.print(F("VOLTAGE 1ST"));
		OSD.setCursor(0, 3);
		OSD.print(F("VOLTAGE 2ND"));
		OSD.setCursor(0, 4);
		OSD.print(F("V-ALARM ON/OFF"));
		OSD.setCursor(0, 5);
		OSD.print(F("VOLTAGE OFFSET"));
		OSD.setCursor(0, 6);
		OSD.print(F("VOLT ALARM HYST/CELL"));
		OSD.setCursor(0, 7);
		OSD.print(F("V/CELL DURING FLIGHT"));
		cursorlineMax = 6;
		break;
	case 2:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P2/7 CAPACITY    "));
		OSD.setCursor(0, -1);
		OSD.print(F(" <-PITCH-> : MOVE UP/DOWN    "));
		OSD.videoBackground();
		OSD.setCursor(0, 2);
		OSD.print(F("CAPACITY IN MAH"));
		OSD.setCursor(0, 3);
		OSD.print(F("1ST WARN AT % USED"));
		OSD.setCursor(0, 4);
		OSD.print(F("2ND WARN AT % USED"));
		OSD.setCursor(0, 6);
		OSD.print(F("1ST WARN AT MAH USED"));
		OSD.setCursor(0, 7);
		OSD.print(F("2ND WARN AT MAH USED"));
		cursorlineMax = 3;
		break;
	case 3:
		//Red1
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P3/7 DISP MODE 1 "));
		OSD.setCursor(0, -1);
		OSD.print(F(" <-ROLL-> : CHANGE VALUE     "));
		printRED();
		break;
	case 4:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P4/7 DISP MODE 2 "));
		OSD.setCursor(0, -1);
		OSD.print(F("SEE PAG6 FOR RED-CHANNEL SELE"));
		printRED();
		break;
	case 5:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P5/7 DISP MODE 3 "));
		OSD.setCursor(0, -1);
		OSD.print(F("                             "));
		printRED();
		break;
	case 6:
		//Other Settings
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P6/7 VARIOUS    "));
		OSD.setCursor(0, -1);
		OSD.print(F("                             "));
		OSD.videoBackground();
		OSD.setCursor(0, 2);
		OSD.print(F("MARGIN LAST ROW"));
		OSD.setCursor(0, 3);
		OSD.print(F("MAGNETPOLECOUNT MOT"));
		OSD.setCursor(0, 4);
		OSD.print(F("ESC FILTER"));
		OSD.setCursor(0, 5);
		OSD.print(F("MODE CHANNEL"));
		OSD.setCursor(0, 6);
		OSD.print(F("SELECT THR / AUX 1-4"));
		OSD.setCursor(0, 7);
		OSD.print(F("SCREEN OFFSET VER"));
		OSD.setCursor(14, 8);
		OSD.print(F("HOR"));
		OSD.setCursor(0, 9);
		OSD.print(F("DISP LINES"));
		OSD.setCursor(0, 10);
		OSD.print(F("NAME"));
		cursorlineMax = 19;
		break;
	case 7:
		//Info
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P7/7 INFO       "));
		OSD.setCursor(0, -1);
		OSD.print(F("                             "));
		OSD.videoBackground();
		OSD.setCursor(0, 2);
		OSD.print(F("FREE RAM"));
		OSD.setCursor(0, 3);
		OSD.print(F("OSD VERSION"));
		OSD.setCursor(0, 4);
		OSD.print(F("MEMORY VERSION"));
		OSD.setCursor(0, 5);
		OSD.print(F("CURR VIDEOSYSTEM"));
		OSD.setCursor(1, 6);
		OSD.print(F("SAVED"));
		cursorlineMax = 0;
		break;

	}
}

//print the actual menu-value
void menuprintvalue() {
	switch (MenuPage)
	{
	case 1:
		//VoltageSite
		OSD.setCursor(23, 2);
		ClearTempCharConverted();
		TempCharPosition = print_int16(Settings.LowVoltage1st, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'V';
		OSD.print(TempCharConverted);
		OSD.setCursor(23, 3);
		ClearTempCharConverted();
		TempCharPosition = print_int16(Settings.LowVoltage2nd, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'V';
		OSD.print(TempCharConverted);
		OSD.setCursor(23, 4);
		showONOFF(Settings.LowVoltageAllowed);
		DisplaySpace();
		OSD.setCursor(23, 5);
		OSD.print(Settings.VoltageOffset);
		OSD.print("MV ");
		OSD.setCursor(23, 6);
		OSD.print(Settings.hysteresis);
		OSD.print("MV ");
		OSD.setCursor(23, 7);
		showONOFF(Settings.VoltageDisplayingCell);
		break;
	case 2:
		//CapacitySite
		OSD.setCursor(23, 2);
		OSD.print(Settings.Capacity);
		OSD.write(SYM_MAH);
		DisplaySpace();
		OSD.setCursor(23, 3);
		OSD.print(Settings.Capacity1st);
		DisplaySpace();
		OSD.setCursor(23, 4);
		OSD.print(Settings.Capacity2nd);
		DisplaySpace();
		OSD.setCursor(23, 6);
		OSD.print((Settings.Capacity * (float)Settings.Capacity1st) / 100);
		DisplaySpace();
		//OSD.write(SYM_MAH);
		OSD.setCursor(23, 7);
		OSD.print((Settings.Capacity * (float)Settings.Capacity2nd) / 100);
		//OSD.write(SYM_MAH);
		DisplaySpace();
		break;
	case 3:
		//RED1
		OSD.setCursor(23, 2);
		showONOFF(Settings.DispChannel1);
		OSD.setCursor(23, 3);
		showONOFF(Settings.DispCombCurrent1);
		OSD.setCursor(23, 4);
		showONOFF(Settings.DispLipoVoltage1);
		OSD.setCursor(23, 5);
		showONOFF(Settings.DispMaConsumption1);
		OSD.setCursor(23, 6);
		showONOFF(Settings.DispEscKrpm1);
		OSD.setCursor(23, 7);
		showONOFF(Settings.DispEscCurrent1);
		OSD.setCursor(23, 8);
		showONOFF(Settings.DispEscTemp1);
		OSD.setCursor(23, 9);
		showONOFF(Settings.DispPilotname1);
		OSD.setCursor(23, 10);
		showONOFF(Settings.DispTimer1);
		OSD.setCursor(23, 11);
		showONOFF(Settings.DispAngle1);
		break;
	case 4:
		//RED2
		OSD.setCursor(23, 2);
		showONOFF(Settings.DispChannel2);
		OSD.setCursor(23, 3);
		showONOFF(Settings.DispCombCurrent2);
		OSD.setCursor(23, 4);
		showONOFF(Settings.DispLipoVoltage2);
		OSD.setCursor(23, 5);
		showONOFF(Settings.DispMaConsumption2);
		OSD.setCursor(23, 6);
		showONOFF(Settings.DispEscKrpm2);
		OSD.setCursor(23, 7);
		showONOFF(Settings.DispEscCurrent2);
		OSD.setCursor(23, 8);
		showONOFF(Settings.DispEscTemp2);
		OSD.setCursor(23, 9);
		showONOFF(Settings.DispPilotname2);
		OSD.setCursor(23, 10);
		showONOFF(Settings.DispTimer2);
		OSD.setCursor(23, 11);
		showONOFF(Settings.DispAngle2);
		break;
	case 5:
		//RED1
		OSD.setCursor(23, 2);
		showONOFF(Settings.DispChannel3);
		OSD.setCursor(23, 3);
		showONOFF(Settings.DispCombCurrent3);
		OSD.setCursor(23, 4);
		showONOFF(Settings.DispLipoVoltage3);
		OSD.setCursor(23, 5);
		showONOFF(Settings.DispMaConsumption3);
		OSD.setCursor(23, 6);
		showONOFF(Settings.DispEscKrpm3);
		OSD.setCursor(23, 7);
		showONOFF(Settings.DispEscCurrent3);
		OSD.setCursor(23, 8);
		showONOFF(Settings.DispEscTemp3);
		OSD.setCursor(23, 9);
		showONOFF(Settings.DispPilotname3);
		OSD.setCursor(23, 10);
		showONOFF(Settings.DispTimer3);
		OSD.setCursor(23, 11);
		showONOFF(Settings.DispAngle3);
		break;
	case 6:
		//Other Settings
		OSD.setCursor(23, 2);
		OSD.print(Settings.marginLastRow);
		DisplaySpace();
		OSD.setCursor(23, 3);
		OSD.print(Settings.MAGNETPOLECOUNT);
		DisplaySpace();
		OSD.setCursor(23, 4);
		OSD.print(Settings.ESC_FILTER);
		DisplaySpace();
		OSD.setCursor(23, 5);
		OSD.print(Settings.RED_MODE_AUX_CHANNEL);
		DisplaySpace();
		OSD.setCursor(23, 6);
		OSD.print(Settings.ChannelSelectDisp);
		DisplaySpace();
		OSD.setCursor(23, 7);
		OSD.print(Settings.OffsetX);
		OSD.print(F("PX "));
		OSD.setCursor(23, 8);
		OSD.print(Settings.OffsetY);
		OSD.print(F("PX "));
		OSD.setCursor(23, 9);
		if (Settings.VideoMode == 1)
			OSD.print(13 + Settings.LineAddition);
		else
			OSD.print(13 + Settings.LineAddition);
		OSD.setCursor(17, 10);
		OSD.print(Settings.PilotName);
		break;
	case 7:
		//Info
		OSD.setCursor(23, 2);
		OSD.print(freeRam());
		OSD.setCursor(22, 3);
		OSD.print(OSDVersion);
		OSD.setCursor(23, 4);
		OSD.print(DMemoryVersion);
		OSD.setCursor(23, 5);
		if (OSD.videoSystem() == 1)
			OSD.print(F("PAL"));
		else if (OSD.videoSystem() == 2)
			OSD.print(F("NTSC"));
		OSD.setCursor(23, 6);
		if (Settings.VideoMode == 1)
			OSD.print(F("PAL"));
		else if (OSD.videoSystem() == 2)
			OSD.print(F("NTSC"));
		else
			OSD.print(F("ERR"));
		break;
	}
}

void value(bool addsub)
{
	switch (MenuPage)
	{
	case 1:
		//VoltageSite
		switch (cursorline)
		{
		case 1: changeval(addsub, 300, 420, 2, &Settings.LowVoltage1st);
			if (Settings.LowVoltage1st < Settings.LowVoltage2nd)
				Settings.LowVoltage2nd = Settings.LowVoltage1st;
			break;
		case 2: changeval(addsub, 300, 420, 2, &Settings.LowVoltage2nd);
			if (Settings.LowVoltage2nd > Settings.LowVoltage1st)
				Settings.LowVoltage1st = Settings.LowVoltage2nd;
			break;
		case 3: changeval(addsub, 0, 1, 1, &Settings.LowVoltageAllowed);
			break;
		case 4: changeval(addsub, -100, 100, 1, &Settings.VoltageOffset);
			break;
		case 5: changeval(addsub, 0, 100, 1, &Settings.hysteresis);
			break;
		case 6: changeval(addsub, 0, 1, 1, &Settings.VoltageDisplayingCell);
			break;
		}
		break;
	case 2:
		//CapacitySite
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 9999, 10, &Settings.Capacity);
			break;
		case 2: changeval(addsub, 0, 100, 1, &Settings.Capacity1st);
			if (Settings.Capacity2nd < Settings.Capacity1st)
				Settings.Capacity2nd = Settings.Capacity1st;
			break;
		case 3: changeval(addsub, 0, 100, 1, &Settings.Capacity2nd);
			if (Settings.Capacity2nd < Settings.Capacity1st)
				Settings.Capacity1st = Settings.Capacity2nd;
			break;
		}
		break;
	case 3:
		//RED1
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispChannel1);
			break;
		case 2: changeval(addsub, 0, 1, 1, &Settings.DispCombCurrent1);
			break;
		case 3: changeval(addsub, 0, 1, 1, &Settings.DispLipoVoltage1);
			break;
		case 4: changeval(addsub, 0, 1, 1, &Settings.DispMaConsumption1);
			break;
		case 5: changeval(addsub, 0, 1, 1, &Settings.DispEscKrpm1);
			break;
		case 6: changeval(addsub, 0, 1, 1, &Settings.DispEscCurrent1);
			break;
		case 7: changeval(addsub, 0, 1, 1, &Settings.DispEscTemp1);
			break;
		case 8: changeval(addsub, 0, 1, 1, &Settings.DispPilotname1);
			break;
		case 9: changeval(addsub, 0, 1, 1, &Settings.DispTimer1);
			break;
		case 10: changeval(addsub, 0, 1, 1, &Settings.DispAngle1);
			break;
		}
		break;
	case 4:
		//RED2
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispChannel2);
			break;
		case 2: changeval(addsub, 0, 1, 1, &Settings.DispCombCurrent2);
			break;
		case 3: changeval(addsub, 0, 1, 1, &Settings.DispLipoVoltage2);
			break;
		case 4: changeval(addsub, 0, 1, 1, &Settings.DispMaConsumption2);
			break;
		case 5: changeval(addsub, 0, 1, 1, &Settings.DispEscKrpm2);
			break;
		case 6: changeval(addsub, 0, 1, 1, &Settings.DispEscCurrent2);
			break;
		case 7: changeval(addsub, 0, 1, 1, &Settings.DispEscTemp2);
			break;
		case 8: changeval(addsub, 0, 1, 1, &Settings.DispPilotname2);
			break;
		case 9: changeval(addsub, 0, 1, 1, &Settings.DispTimer2);
			break;
		case 10: changeval(addsub, 0, 1, 1, &Settings.DispAngle2);
			break;
		}
		break;
	case 5:
		//RED3
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispChannel3);
			break;
		case 2: changeval(addsub, 0, 1, 1, &Settings.DispCombCurrent3);
			break;
		case 3: changeval(addsub, 0, 1, 1, &Settings.DispLipoVoltage3);
			break;
		case 4: changeval(addsub, 0, 1, 1, &Settings.DispMaConsumption3);
			break;
		case 5: changeval(addsub, 0, 1, 1, &Settings.DispEscKrpm3);
			break;
		case 6: changeval(addsub, 0, 1, 1, &Settings.DispEscCurrent3);
			break;
		case 7: changeval(addsub, 0, 1, 1, &Settings.DispEscTemp3);
			break;
		case 8: changeval(addsub, 0, 1, 1, &Settings.DispPilotname3);
			break;
		case 9: changeval(addsub, 0, 1, 1, &Settings.DispTimer3);
			break;
		case 10: changeval(addsub, 0, 1, 1, &Settings.DispAngle3);
			break;
		}
		break;
	case 6:
		//Other Settings
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 6, 1, &Settings.marginLastRow);
			break;
		case 2: changeval(addsub, 2, 28, 2, &Settings.MAGNETPOLECOUNT);
			break;
		case 3: changeval(addsub, 0, 100, 1, &Settings.ESC_FILTER);
			break;
		case 4: changeval(addsub, 0, 4, 1, &Settings.RED_MODE_AUX_CHANNEL);
			break;
		case 5: changeval(addsub, 0, 4, 1, &Settings.ChannelSelectDisp);
			break;
		case 6: changeval(addsub, MAX7456_VOS_MIN, MAX7456_VOS_MAX, 1, &Settings.OffsetX);
			OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);
			break;
		case 7: changeval(addsub, MAX7456_HOS_MIN, MAX7456_HOS_MAX, 1, &Settings.OffsetY);
			OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);
			break;
		case 8: changeval(addsub, 0, 2, 1, &Settings.LineAddition);
			break;
		}
		if (cursorline > 8)
			changeChar(addsub, cursorline - 9);
		break;
	case 7:
		//Info
		break;
	}
	pause = 1;
}

void changeval(bool addsub, int16_t min_value, int16_t max_value, uint16_t increment, uint16_t *variable)
{
	if (addsub && *variable<max_value)     // 
	{
		*variable = *variable + increment;
	}
	if (!addsub && *variable>min_value)	// && *variable>min_value
	{
		*variable = *variable - increment;
	}
}

void changeval(bool addsub, int32_t min_value, int32_t max_value, uint16_t increment, uint8_t *variable)
{
	if (addsub && *variable<max_value)     // && *variable<max_value
	{
		*variable = *variable + increment;
	}
	if (!addsub && *variable>min_value)	// 
	{
		*variable = *variable - increment;
	}
}

void changeval(bool addsub, int32_t min_value, int32_t max_value, uint16_t increment, int8_t *variable)
{
	if (addsub && *variable<max_value)     // && *variable<max_value
	{
		*variable = *variable + increment;
	}
	if (!addsub && *variable>min_value)	// 
	{
		*variable = *variable - increment;
	}
}

void changeval(bool addsub, int32_t min_value, int32_t max_value, uint16_t increment, float *variable)
{
	if (addsub && *variable<max_value)     // && *variable<max_value
	{
		*variable = *variable + increment;
	}
	if (!addsub && *variable>min_value)	// 
	{
		*variable = *variable - increment;
	}
}

void showONOFF(uint8_t val)
{
	if (val == 0)
		OSD.print(F("OFF"));
	else
	{
		OSD.print(F("ON"));
		DisplaySpace();
	}
}

void printRED()
{
	OSD.videoBackground();
	OSD.setCursor(0, 2);
	OSD.print(F("THR / AUX 1-4"));
	OSD.setCursor(0, 3);
	OSD.print(F("TOTAL CURRENT"));
	OSD.setCursor(0, 4);
	OSD.print(F("LIPO VOLTAGE"));
	OSD.setCursor(0, 5);
	OSD.print(F("CONSUMPTION MAH"));
	OSD.setCursor(0, 6);
	OSD.print(F("ESC KRPM *1000"));
	OSD.setCursor(0, 7);
	OSD.print(F("ESC CURRENT"));
	OSD.setCursor(0, 8);
	OSD.print(F("ESC TEMPERATURE"));
	OSD.setCursor(0, 9);
	OSD.print(F("PILOTNAME"));
	OSD.setCursor(0, 10);
	OSD.print(F("TIMER"));
	OSD.setCursor(0, 11);
	OSD.print(F("ANGLE PITCH"));
	cursorlineMax = 10;
}

bool Menuall_start(uint8_t GetSettings)
{
	OSDmakegrey();
	OSD.setCursor(9, 3);
	OSD.print(F("SAMUD OSD"));
	OSD.setCursor(6, 4);
	OSD.print(F("CUSTOM KISS OSD"));
	OSD.setCursor(1, 8);
	OSD.print(F("MORE INFORMATION AND WIKI:"));
	OSD.setCursor(1, 9);
	OSD.print(F("GITHUB.COM/SAMUD/KISS-OSD"));
	OSD.setCursor(7, 10);
	OSD.print(F("OR ON FACEBOOK"));
	OSD.videoBackground();

	//delay(500);

	if (GetSettings == GET_SETTINGS)
	{
		//aquire settings from FC
		i = 0;
		while (!getSerialData(GET_SETTINGS, true))
		{
			delay(500);
			i++;
			if (i > 10)								//Unable to re-aquire data
			{
				KissConnection = LostConnection;
				KissStatus.SerialErrorReason = 4;
				return false;
			}
		}
		getSerialData(GET_SETTINGS, true);						//get the Settings from the FC
#ifdef DEBUG
		Debug_Fnc("AQUR SETT");
#endif // DEBUG	
	}
	else
	{
		Settings.stockSettings = 1;				//byte saving that I entered the Menu
	}


	KissTelemetrie.StickChanVals[3] = 0;	//reset the current Stick to avoid changing Menu page diectly upon entering

	delay(500);									//delay for showing the message
	OSD.clear();
	while (OSD.clearIsBusy());
}

/*void MenuAll_Exit(uint8_t GetSettings)
{
MenuPage = 1;
MenuPageLeft = 1;
OldMenuPage = 0;
OldMenuPageLeft = 0;
KissTelemetrie.StickChanVals[3] = 0;
KissStatus.lastMode = 5;
OSD.clear();
if (GetSettings == GET_SETTINGS)
{
KissStatus.SavingSettingsRetry = 10;
while (setSerialData() == false && KissStatus.SavingSettingsRetry > 0)
{
KissStatus.SavingSettingsRetry--;
delay(750);
Serial.end();
delay(750);
Serial.begin(115200);
delay(750);
}
if(KissStatus.SavingSettingsRetry<1)
OSD.print(F("FAILED"));
else
OSD.print(F("OK"));

//We need to reboot the serial connection, because sometimes it gets stuck here.
//Serial.end();
//delay(750);
//Serial.begin(115200);
//delay(250);
}
else
EEPROMsave();

}*/

void MenuAll_Exit(uint8_t GetSettings)
{
	MenuPage = 1;
	MenuPageLeft = 1;
	OldMenuPage = 0;
	OldMenuPageLeft = 0;
	KissTelemetrie.StickChanVals[3] = 0;
	KissStatus.lastMode = KissStatus.reducedModeDisplay;
	OSD.clear();
	delay(25);
	KissStatus.MenuBlockiterations = 60;
	if (GetSettings == GET_SETTINGS)
	{
		if (setSerialData())
			OSD.print(F("FINISHED"));
		else
			OSD.print(F("UNKNWN ANSW. FAIL"));

		//We need to reboot the serial connection, because sometimes it gets stuck here.
		Serial.end();
		delay(750);
		Serial.begin(115200);
		delay(250);
		OSD.clear();
	}
	else
		EEPROMsave();

}

void changeChar(bool addsub, uint8_t position)
{

	if (addsub)
		Settings.PilotName[position] = Settings.PilotName[position] + 1;
	else
		Settings.PilotName[position] = Settings.PilotName[position] - 1;
}
