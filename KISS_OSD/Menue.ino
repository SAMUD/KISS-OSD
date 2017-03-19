/* 1. Information
//=========================================================================================================================

Fonctions needed for the settings-menu off the OSD.

//=========================================================================================================================
//START OF CODE
*/
static uint8_t MenuPage = 1;
static uint8_t OldMenuPage = 0;
static uint8_t cursorlineMax = 0;
static uint8_t cursorline = 0;
static uint8_t cursorlineOLD = 0;
static uint8_t pause = 0; //makes a pause of multiples of 50ms

void menumain()
{
	static uint32_t LastLoopTimeMenu;
	static boolean exitmenu = false;
	static uint8_t ii;

	Settings.stockSettings = 1;
	KissData.StickChanVals[3] = 0;

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
	delay(2000);

	while (!exitmenu)
	{

		if (micros() - KissStatus.LastLoopTime > 100000) //limits the speed of the OSD to 20Hz  millis() - LastLoopTimeMenu > 100
		{
			KissStatus.LastLoopTime = micros();

			getSerialData();

			//set cursor position
			if (KissData.StickChanVals[2] < -800 && cursorline<cursorlineMax)
			{
				cursorline++;
				KissData.StickChanVals[2] = 0;
				pause = 5;
			}
			if (KissData.StickChanVals[2] > 800 && cursorline>1)
			{
				cursorline--;
				KissData.StickChanVals[2] = 0;
				pause = 5;
			}

			//changevalue
			if (KissData.StickChanVals[1] > 800)
			{
				value(true);
			}
			if (KissData.StickChanVals[1] < -800)
			{
				value(false);
			}

			//changepage
			if (KissData.StickChanVals[3] > 800)
			{
				if (MenuPage<7)
				{
					MenuPage++;
					KissData.StickChanVals[3] = 0;
					pause = 5;
				}
				else
				{
					exitmenu = true;
					//pause=10;
				}
			}
			if (KissData.StickChanVals[3] < -800)
			{
				if (MenuPage > 1)
				{
					MenuPage--;
					KissData.StickChanVals[3] = 0;
					pause = 5;
				}
				else
				{
					exitmenu = true;
					//pause=10;
				}

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
				OSD.setCursor(23, cursorlineOLD + 1);
				OSD.print(" ");
				OSD.setCursor(23, cursorline + 1);
				OSD.print(">");
				cursorlineOLD = cursorline;
			}

			//pause (multiples of 50ms)
			while (pause > 0)
			{
				delay(50);
				pause--;
			}


			if (KissData.armed == 1)
			{
				exitmenu = true;
			}

		}

		//reset wdt
		wdt_reset();
	}
	exitmenu = false;
	MenuPage = 1;
	OldMenuPage = 0;
	KissData.StickChanVals[3] = 0;
	KissStatus.lastMode = 5;
}

void menuprintsite() {

	OSD.clear();
	delay(100);
	OSD.setCursor(0, 0);
	cursorline = 1;
	cursorlineOLD = 0;

	switch (MenuPage)
	{
	case 1:
		//VoltageSite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P1/7 VOLTAGE     "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-YAW-> : PAGE / EXIT       "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("VOLTAGE 1ST V/CELL:"));
		OSD.setCursor(1, 3);
		OSD.print(F("VOLTAGE 2ND V/CELL:"));
		OSD.setCursor(1, 4);
		OSD.print(F("V-ALARM ON/OFF:"));
		OSD.setCursor(1, 5);
		OSD.print(F("VOLTAGE OFFSET:"));
		OSD.setCursor(1, 6);
		OSD.print(F("VOLT ALARM HYST/CELL:"));
		cursorlineMax = 5;
		break;
	case 2:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P2/7 CAPACITY    "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-PITCH-> : MOVE UP/DOWN    "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("CAPACITY IN MAH"));
		OSD.setCursor(1, 3);
		OSD.print(F("1ST WARN AT % USED:"));
		OSD.setCursor(1, 4);
		OSD.print(F("2ND WARN AT % USED:"));
		OSD.setCursor(1, 5);
		OSD.print(F("STB CURRENT @5V:"));
		OSD.setCursor(1, 7);
		OSD.print(F("1ST WARN AT MAH USED:"));
		OSD.setCursor(1, 8);
		OSD.print(F("2ND WARN AT MAH USED:"));
		OSD.setCursor(1, 9);
		OSD.print(F("STB CURRENT FROM BAT:"));
		cursorlineMax = 4;
		break;
	case 3:
		//Red1
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P3/7 RED MODE 1  "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-ROLL-> : CHANGE VALUE     "));
		printRED();
		break;
	case 4:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P4/7 RED MODE 2  "));
		OSD.setCursor(0, 14);
		OSD.print(F("SEE PAG6 FOR RED-CHANNEL SELE"));
		printRED();
		break;
	case 5:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P5/7 RED MODE 3  "));
		OSD.setCursor(0, 14);
		OSD.print(F("                             "));
		printRED();
		break;
	case 6:
		//Other Settings
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P6/7 VARIOUS    "));
		OSD.setCursor(0, 14);
		OSD.print(F("                             "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("MARGIN LAST ROW:"));
		OSD.setCursor(1, 3);
		OSD.print(F("MAGNETPOLECOUNT MOT:"));
		OSD.setCursor(1, 4);
		OSD.print(F("ESC FILTER:"));
		OSD.setCursor(1, 5);
		OSD.print(F("RED MODE AUX CHANNEL:"));
		OSD.setCursor(1, 6);
		OSD.print(F("SCREEN OFFSET ->RIGHT:"));
		OSD.setCursor(15, 7);
		OSD.print(F("->DOWN:"));
		cursorlineMax = 6;
		break;
	case 7:
		//Info
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P7/7 INFO       "));
		OSD.setCursor(0, 14);
		OSD.print(F("                             "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("FREE RAM:"));
		OSD.setCursor(1, 3);
		OSD.print(F("OSD VERSION:"));
		OSD.setCursor(1, 4);
		OSD.print(F("MEMORY VERSION:"));
		OSD.setCursor(1, 5);
		OSD.print(F("VIDEOSYSTEM:"));
		cursorlineMax = 0;
		break;
	default:
		OSD.print(MenuPage);
		break;
	}

}

void menuprintvalue() {


	switch (MenuPage)
	{
	case 1:
		//VoltageSite
		OSD.setCursor(24, 2);
		OSD.print(Settings.LowVoltage1st);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(Settings.LowVoltage2nd);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		showONOFF(Settings.LowVoltageAllowed);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		OSD.print(Settings.VoltageOffset);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		OSD.print(Settings.hysteresis);
		OSD.print(" ");
		break;
	case 2:
		//CapacitySite
		OSD.setCursor(24, 2);
		OSD.print(Settings.Capacity);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(Settings.Capacity1st);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		OSD.print(Settings.Capacity2nd);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		OSD.print(Settings.StandbyCurrent);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		OSD.print((Settings.Capacity * (float)Settings.Capacity1st)/100);
		OSD.print(" ");
		OSD.setCursor(24, 8);
		OSD.print((Settings.Capacity * (float)Settings.Capacity2nd) / 100);
		OSD.print(" ");
		OSD.setCursor(24, 9);
		OSD.print((Settings.StandbyCurrent * 5) / KissData.LipoVoltage);
		OSD.print(" ");
		
		break;
	case 3:
		//RED1
		OSD.setCursor(24, 2);
		showONOFF(Settings.DispRCThrottle1);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		showONOFF(Settings.DispCombCurrent1);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		showONOFF(Settings.DispLipoVoltage1);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		showONOFF(Settings.DispMaConsumption1);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		showONOFF(Settings.DispEscKrpm1);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		showONOFF(Settings.DispEscCurrent1);
		OSD.print(" ");
		OSD.setCursor(24, 8);
		showONOFF(Settings.DispEscTemp1);
		OSD.print(" ");
		OSD.setCursor(24, 9);
		showONOFF(Settings.DispPilotname1);
		OSD.print(" ");
		OSD.setCursor(24, 10);
		showONOFF(Settings.DispTimer1);
		OSD.print(" ");
		OSD.setCursor(24, 11);
		showONOFF(Settings.DispAngle1);
		OSD.print(" ");
		break;
	case 4:
		//RED2
		OSD.setCursor(24, 2);
		showONOFF(Settings.DispRCThrottle2);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		showONOFF(Settings.DispCombCurrent2);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		showONOFF(Settings.DispLipoVoltage2);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		showONOFF(Settings.DispMaConsumption2);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		showONOFF(Settings.DispEscKrpm2);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		showONOFF(Settings.DispEscCurrent2);
		OSD.print(" ");
		OSD.setCursor(24, 8);
		showONOFF(Settings.DispEscTemp2);
		OSD.print(" ");
		OSD.setCursor(24, 9);
		showONOFF(Settings.DispPilotname2);
		OSD.print(" ");
		OSD.setCursor(24, 10);
		showONOFF(Settings.DispTimer2);
		OSD.print(" ");
		OSD.setCursor(24, 11);
		showONOFF(Settings.DispAngle2);
		OSD.print(" ");
		break;
	case 5:
		//RED1
		OSD.setCursor(24, 2);
		showONOFF(Settings.DispRCThrottle3);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		showONOFF(Settings.DispCombCurrent3);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		showONOFF(Settings.DispLipoVoltage3);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		showONOFF(Settings.DispMaConsumption3);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		showONOFF(Settings.DispEscKrpm3);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		showONOFF(Settings.DispEscCurrent3);
		OSD.print(" ");
		OSD.setCursor(24, 8);
		showONOFF(Settings.DispEscTemp3);
		OSD.print(" ");
		OSD.setCursor(24, 9);
		showONOFF(Settings.DispPilotname3);
		OSD.print(" ");
		OSD.setCursor(24, 10);
		showONOFF(Settings.DispTimer3);
		OSD.print(" ");
		OSD.setCursor(24, 11);
		showONOFF(Settings.DispAngle3);
		OSD.print(" ");
		break;
	case 6:
		//Other Settings
		OSD.setCursor(24, 2);
		OSD.print(Settings.marginLastRow);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(Settings.MAGNETPOLECOUNT);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		OSD.print(Settings.ESC_FILTER);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		OSD.print(Settings.RED_MODE_AUX_CHANNEL);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		OSD.print(Settings.OffsetX);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		OSD.print(Settings.OffsetY);
		OSD.print(" ");
		break;
	case 7:
		//Info
		OSD.setCursor(24, 2);
		OSD.print(freeRam());
		OSD.setCursor(22, 3);
		OSD.print(OSDVersion);
		OSD.setCursor(22, 4);
		OSD.print(DMemoryVersion);
		OSD.setCursor(22, 5);
		if(OSD.videoSystem() == 1)
			OSD.print(F("PAL"));
		else if (OSD.videoSystem() == 2)
			OSD.print(F("NTSC"));
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
		case 4: changeval(addsub, 0, 9999, 5, &Settings.StandbyCurrent);
		}
		break;
	case 3:
		//RED1
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispRCThrottle1);
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
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispRCThrottle2);
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
		case 1: changeval(addsub, 0, 1, 1, &Settings.DispRCThrottle3);
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
		case 2: changeval(addsub, 2, 20, 1, &Settings.MAGNETPOLECOUNT);
			break;
		case 3: changeval(addsub, 0, 20, 1, &Settings.ESC_FILTER);
			break;
		case 4: changeval(addsub, 0, 4, 1, &Settings.RED_MODE_AUX_CHANNEL);
			break;
		case 5: changeval(addsub, -10, 10, 1, &Settings.OffsetX);
			OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);
			break;
		case 6: changeval(addsub, -10, 10, 1, &Settings.OffsetY);
			OSD.setTextOffset(Settings.OffsetX, Settings.OffsetY);
			break;
		}
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

void changeval(bool addsub, int16_t min_value, int16_t max_value, uint16_t increment, uint8_t *variable)
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

void changeval(bool addsub, int16_t min_value, int16_t max_value, uint16_t increment, int8_t *variable)
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

void changeval(bool addsub, int16_t min_value, int16_t max_value, uint16_t increment, float *variable)
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
		OSD.print("OFF");
	else
		OSD.print("ON");
}

void printRED()
{
	OSD.videoBackground();
	OSD.setCursor(1, 2);
	OSD.print(F("RC THROTTLE:"));
	OSD.setCursor(1, 3);
	OSD.print(F("COMBINED CURRENT:"));
	OSD.setCursor(1, 4);
	OSD.print(F("LIPO VOLTAGE:"));
	OSD.setCursor(1, 5);
	OSD.print(F("CONSUMPTION MA:"));
	OSD.setCursor(1, 6);
	OSD.print(F("ESC KRPM *1000:"));
	OSD.setCursor(1, 7);
	OSD.print(F("ESC CURRENT:"));
	OSD.setCursor(1, 8);
	OSD.print(F("ESC TEMPERATURE:"));
	OSD.setCursor(1, 9);
	OSD.print(F("PILOTNAME:"));
	OSD.setCursor(1, 10);
	OSD.print(F("TIMER:"));
	OSD.setCursor(1, 11);
	OSD.print(F("ANGLE PITCH:"));
	cursorlineMax = 10;
}
