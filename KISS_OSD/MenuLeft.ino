/* 1. Information
//=========================================================================================================================

Fonctions needed for the settings-menu off the OSD.

//=========================================================================================================================
//START OF CODE
*/

static uint8_t MenuPageLeft = 1;
static uint8_t OldMenuPageLeft = 0;
static uint8_t CursorlineMaxLeft = 0;
static uint8_t CursorlineLeft = 0;
static uint8_t CursorlineOldLeft = 0;
static uint8_t PauseLeft = 0; //makes a PauseLeft of multiples of 50ms

void MenuLeft_Main()
{
	boolean exitmenu = false;
	Menuall_start(GET_SETTINGS);

	while (!exitmenu)
	{
		if (micros() - KissStatus.LastLoopTime > 100000) //limits the speed of the OSD to 20Hz  millis() - LastLoopTimeMenu > 100
		{
			KissStatus.LastLoopTime = micros();
			getSerialData(GET_TELEMETRY);

			//set cursor position
			if (KissTelemetrie.StickChanVals[2] < -800 && CursorlineLeft<CursorlineMaxLeft)
			{
				CursorlineLeft++;
				KissTelemetrie.StickChanVals[2] = 0;
				PauseLeft = 5;
			}
			if (KissTelemetrie.StickChanVals[2] > 800 && CursorlineLeft>1)
			{
				CursorlineLeft--;
				KissTelemetrie.StickChanVals[2] = 0;
				PauseLeft = 5;
			}

			//changevalue
			if (KissTelemetrie.StickChanVals[1] > 800)
			{
				MenuLeft_Valie(true);
			}
			if (KissTelemetrie.StickChanVals[1] < -800)
			{
				MenuLeft_Valie(false);
			}

			//changepage
			if (KissTelemetrie.StickChanVals[3] > 800)
			{
				if (MenuPageLeft<1)
				{
					MenuPageLeft++;
					KissTelemetrie.StickChanVals[3] = 0;
					PauseLeft = 5;
				}
				else
					exitmenu = true;
			}
			if (KissTelemetrie.StickChanVals[3] < -800)
			{
				if (MenuPageLeft > 1)
				{
					MenuPageLeft--;
					KissTelemetrie.StickChanVals[3] = 0;
					PauseLeft = 5;
				}
				else
					exitmenu = true;
			}


			//redraw menu site
			if (MenuPageLeft != OldMenuPageLeft)
			{
				MenuLeft_PrintSite();
				OldMenuPageLeft = MenuPageLeft;
			}

			//redraw MenuLeft_Valie
			MenuLeft_PrintValue();

			//draw cursor position
			if (CursorlineLeft != CursorlineOldLeft && MenuPageLeft != 7)
			{
				OSD.setCursor(23, CursorlineOldLeft + 1);
				OSD.print(" ");
				OSD.setCursor(23, CursorlineLeft + 1);
				OSD.print(">");
				CursorlineOldLeft = CursorlineLeft;
			}

			//PauseLeft (multiples of 50ms)
			while (PauseLeft > 0)
			{
				delay(50);
				PauseLeft--;
			}

			if (KissTelemetrie.armed == 1)
				exitmenu = true;
		}
		//reset wdt
		//wdt_reset();
	}

	MenuAll_Exit(GET_SETTINGS);

}

//printing the actual menu page
void MenuLeft_PrintSite() {

	OSD.clear();
	while (OSD.clearIsBusy());
	OSD.home();
	CursorlineLeft = 1;
	CursorlineOldLeft = 0;

	switch (MenuPageLeft)
	{
	case 1:
		//PID
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P1/8 PID     "));
		OSD.setCursor(0, 14);
		OSD.print(F(" <-YAW-> : PAGE / EXIT       "));
		OSD.videoBackground();
		OSD.setCursor(1, 2);
		OSD.print(F("ROLL  P"));
		OSD.setCursor(7, 3);
		OSD.print(F("I"));
		OSD.setCursor(7, 4);
		OSD.print(F("D"));
		OSD.setCursor(1, 5);
		OSD.print(F("PITCH P"));
		OSD.setCursor(7, 6);
		OSD.print(F("I"));
		OSD.setCursor(7, 7);
		OSD.print(F("D"));
		OSD.setCursor(1, 8);
		OSD.print(F("YAW   P"));
		OSD.setCursor(7, 9);
		OSD.print(F("I"));
		CursorlineMaxLeft = 8;
		break;
	}
}

//print the actual menu-value
void MenuLeft_PrintValue() {
	switch (MenuPageLeft)
	{
	case 1:
		//PID
		OSD.setCursor(24, 2);
		OSD.print(((float)KissSettings.PID_P[0]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 3);
		OSD.print(((float)KissSettings.PID_I[0]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 4);
		OSD.print(((float)KissSettings.PID_D[0]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 5);
		OSD.print(((float)KissSettings.PID_P[1]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 6);
		OSD.print(((float)KissSettings.PID_I[1]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 7);
		OSD.print(((float)KissSettings.PID_D[1]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 8);
		OSD.print(((float)KissSettings.PID_P[2]) / 1000);
		OSD.print(" ");
		OSD.setCursor(24, 9);
		OSD.print(((float)KissSettings.PID_I[2]) / 1000);
		OSD.print(" ");
		break;	
	}
}

void MenuLeft_Valie(bool addsub)
{
	switch (MenuPageLeft)
	{
	case 1:
		//PID
		switch (CursorlineLeft)
		{
		case 1: changeval(addsub, 0, 67000, 5, &KissSettings.PID_P[0]);
			break;
		case 2: changeval(addsub, 0, 67000, 5, &KissSettings.PID_I[0]);
			break;
		case 3: changeval(addsub, 0, 67000, 5, &KissSettings.PID_D[0]);
			break;
		case 4: changeval(addsub, 0, 67000, 5, &KissSettings.PID_P[1]);
			break;
		case 5: changeval(addsub, 0, 67000, 5, &KissSettings.PID_I[1]);
			break;
		case 6: changeval(addsub, 0, 67000, 5, &KissSettings.PID_D[1]);
			break;
		case 7: changeval(addsub, 0, 67000, 5, &KissSettings.PID_P[2]);
			break;
		case 8: changeval(addsub, 0, 67000, 5, &KissSettings.PID_I[2]);
			break;
		}
		break;
	}
	PauseLeft = 1;
}













