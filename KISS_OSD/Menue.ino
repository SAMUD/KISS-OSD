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
	static uint8_t i;
	static uint8_t ii;


	OSDmakegrey();
	OSD.setCursor(6, 3);
	OSD.print(F("SAMUD - KISS OSD"));
	OSD.setCursor(1, 5);
	OSD.print(F("MORE INFORMATION AND WIKI: "));
	OSD.setCursor(1, 6);
	OSD.print(F("GITHUB.COM/SAMUD/KISS-OSD  "));
	OSD.videoBackground();
	delay(2000);




	while (!exitmenu)
	{

		if (micros() - LastLoopTime > 10000) //limits the speed of the OSD to 10Hz  millis() - LastLoopTimeMenu > 100
		{
			LastLoopTime = micros();

			getSerialData();

			//set cursor position
			if (StickChanVals[2] < -500 && cursorline<cursorlineMax)
			{
				cursorline++;
				pause = 10;
			}
			if (StickChanVals[2] > 500 && cursorline>1)
			{
				cursorline--;
				pause = 10;
			}

			//changevalue
			if (StickChanVals[1] > 500)
			{
				value(true);
			}
			if (StickChanVals[1] < -500)
			{
				value(false);
			}

			//changepage
			if (StickChanVals[3] > 500)
			{
				if (MenuPage<4)
				{
					MenuPage++;
					pause = 10;
				}
				else
				{
					exitmenu = true;
					//pause=10;
				}
			}
			if (StickChanVals[3] < -500)
			{
				if (MenuPage > 1)
				{
					MenuPage--;
					pause = 10;
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
			if (cursorline != cursorlineOLD && MenuPage != 4)
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


			if (armed == 1)
			{
				exitmenu = true;
			}

		}
	}
	exitmenu = false;
	MenuPage = 1;
	OldMenuPage = 0;
	StickChanVals[3] = 0;
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
		OSD.print(F("SAMUD OSD - P1/4 VOLTAGE     "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-YAW-> : PAGE / EXIT       "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("VOLTAGE ALARM 3S:"));
		OSD.setCursor(1, 3);
		OSD.print(F("VOLTAGE ALARM 4S:"));
		OSD.setCursor(1, 4);
		OSD.print(F("MIN PER CELL VOLT:"));
		OSD.setCursor(1, 5);
		OSD.print(F("VOLTAGE OFFSET:"));
		OSD.setCursor(1, 6);
		OSD.print(F("VOLT ALARM HYST:"));
		OSD.setCursor(1, 7);
		OSD.print(F("DIFF VOLT 3S 4S:"));
		cursorlineMax = 6;
		break;
	case 2:
		//CapacitySite
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P2/4 CAPACITY    "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-PITCH-> : MOVE UP/DOWN    "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("CAPACITY 1ST WARN:"));
		OSD.setCursor(1, 3);
		OSD.print(F("CAPACITY 2ND WARN:"));
		cursorlineMax = 2;
		break;
	case 3:
		//Other Settings
		OSD.grayBackground();
		OSD.print(F("SAMUD MENU - P3/4 VARIOUS    "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-ROLL-> : CHANGE VALUE     "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("MARGIN LAST ROW:"));
		OSD.setCursor(1, 3);
		OSD.print(F("MAGNETPOLECOUNT MOT:"));
		OSD.setCursor(1, 4);
		OSD.print(F("ESC FILTER:"));
		OSD.setCursor(1, 5);
		OSD.print(F("RED MODE AUX CHANNEL:"));
		cursorlineMax = 4;
		break;
	case 4:
		//Info
		OSD.grayBackground();
		OSD.print(F("SAMUD MENU - P4/4 INFO       "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("FREE RAM:"));
		OSD.setCursor(1, 3);
		OSD.print(F("OSD VERSION:"));
		OSD.setCursor(1, 4);
		OSD.print(F("MEMORY VERSION:"));
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
		OSD.print(Settings.LowVoltage3s);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(Settings.LowVoltage4s);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		OSD.print(Settings.MinimalCellVoltage2nd);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		OSD.print(Settings.VoltageOffset);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		OSD.print(Settings.hysteresis);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		OSD.print(Settings.SeparationVoltage3s4s);
		OSD.print(" ");
		break;
	case 2:
		//CapacitySite
		OSD.setCursor(24, 2);
		OSD.print(Settings.CapacityThreshold);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(Settings.CapacityThreshold2ndStage);
		OSD.print(" ");
		break;
	case 3:
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
		break;
	case 4:
		//Info
		OSD.setCursor(24, 2);
		OSD.print(freeRam());
		OSD.setCursor(22, 3);
		OSD.print(OSDVersion);
		OSD.setCursor(22, 4);
		OSD.print(DMemoryVersion);
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
		case 1: changeval(addsub, 990, 1260, 10, &Settings.LowVoltage3s);
			break;
		case 2: changeval(addsub, 1320, 1680, 10, &Settings.LowVoltage4s);
			break;
		case 3: changeval(addsub, 300, 420, 5, &Settings.MinimalCellVoltage2nd);
			break;
		case 4: changeval(addsub, -100, 100, 1, &Settings.VoltageOffset);
			break;
		case 5: changeval(addsub, 0, 100, 5, &Settings.hysteresis);
			break;
		case 6: changeval(addsub, 990, 1680, 10, &Settings.SeparationVoltage3s4s);
			break;
		}
		break;
	case 2:
		//CapacitySite
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 9999, 10, &Settings.CapacityThreshold);
			break;
		case 2: changeval(addsub, 0, 9999, 10, &Settings.CapacityThreshold2ndStage);
			break;
		}
		break;
	case 3:
		//Other Settings
		switch (cursorline)
		{
		case 1: changeval(addsub, 0, 6, 1, &Settings.marginLastRow);
			break;
		case 2: changeval(addsub, 2, 20, 1, &Settings.MAGNETPOLECOUNT);
			break;
		case 3: changeval(addsub, 0, 20, 1, &Settings.ESC_FILTER);
			break;
		case 4: changeval(addsub, -100, 100, 1, &Settings.VoltageOffset);
			break;
		case 5: changeval(addsub, 0, 4, 1, &Settings.RED_MODE_AUX_CHANNEL);
			break;
		}
		break;
	case 4:
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


