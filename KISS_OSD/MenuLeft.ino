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
	while (!exitmenu && KissTelemetrie.armed == 0)
	{
		if (micros() - KissStatus.LastLoopTime > 100000) //limits the speed of the OSD to 20Hz  millis() - LastLoopTimeMenu > 100
		{
			KissStatus.LastLoopTime = micros();
			getSerialData(GET_TELEMETRY,true);

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
				MenuLeft_Valie(true);

			if (KissTelemetrie.StickChanVals[1] < -800)
				MenuLeft_Valie(false);

			//changepage
			if (KissTelemetrie.StickChanVals[3] > 800)
			{
				if (MenuPageLeft<3)
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
			//there are not only normal rows here. so we need to place the cursor marker freely on the screen
			MenuLeft_Marker(false, CursorlineOldLeft, MenuPageLeft);		//clear the marker
			MenuLeft_Marker(true, CursorlineLeft, MenuPageLeft);			//set the new marker
			CursorlineOldLeft = CursorlineLeft;

			//PauseLeft (multiples of 50ms)
			while (PauseLeft >0)
			{
				delay(50);
				PauseLeft--;
			}
		}
		//reset wdt
		//wdt_reset();
	}

	if(KissTelemetrie.armed ==0)
		//this is a normal exit. Saving settings
		MenuAll_Exit(GET_SETTINGS);
	else
		//wxiting because copter was armed. No saving allowed
		MenuAll_Exit(GET_TELEMETRY);

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
		OSD.print(F("SAMUD OSD - P1/3 PID        "));
		OSD.setCursor(0, -1);
		OSD.print(F(" <-YAW-> : PAGE / EXIT      "));
		OSD.setCursor(9, 2);
		OSD.print(F("P   "));
		OSD.setCursor(16, 2);
		OSD.print(F("I   "));
		OSD.setCursor(24, 2);
		OSD.print(F("D   "));
		OSD.setCursor(8, 7);
		OSD.print(F("RC-RAT"));
		OSD.setCursor(16, 7);
		OSD.print(F("RATE"));
		OSD.setCursor(24, 7);
		OSD.print(F("EXPO"));
		OSD.videoBackground();

		OSD.setCursor(0, 3);
		OSD.print(F("ROLL"));
		OSD.setCursor(0, 4);
		OSD.print(F("PITCH"));
		OSD.setCursor(0, 5);
		OSD.print(F("YAW"));
		OSD.setCursor(0, 8);
		OSD.print(F("ROLL"));
		OSD.setCursor(0, 9);
		OSD.print(F("PITCH"));
		OSD.setCursor(0, 10);
		OSD.print(F("YAW"));
		CursorlineMaxLeft = 18;
		break;
	case 2:
		//PID
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P2/3 LEVEL+TPA  "));
		OSD.setCursor(0, -1);
		OSD.print(F(" <-PITCH-> : MOVE UP/DOWN   "));
		OSD.setCursor(9, 2);
		OSD.print(F("P   "));
		OSD.setCursor(16, 2);
		OSD.print(F("I   "));
		OSD.setCursor(24, 2);
		OSD.print(F("D   "));
		OSD.setCursor(16, 7);
		OSD.print(F("PITCH"));
		OSD.setCursor(24, 7);
		OSD.print(F("ROLL"));
		OSD.videoBackground();

		OSD.setCursor(0, 3);
		OSD.print(F("TPA"));
		OSD.setCursor(0, 4);
		OSD.print(F("LEVEL"));
		OSD.setCursor(1, 5);
		OSD.print(F("MAX ANGLE"));
		OSD.setCursor(0, 8);
		OSD.print(F("ACC-TRIM"));
		OSD.setCursor(0, 9);
		OSD.print(F("NOTCH FILTER"));
		OSD.setCursor(1, 10);
		OSD.print(F("CENTER FREQ"));
		OSD.setCursor(1, 11);
		OSD.print(F("CUTOFF FREQ"));
		OSD.setCursor(0, 12);
		OSD.print(F("YAW-FILTER"));
		CursorlineMaxLeft = 16;
		break;
	case 3:
		//PID
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - P3/3 VARIOUS    "));
		OSD.setCursor(0, -1);
		OSD.print(F("EXIT = SAVING SETTINGS      "));
		OSD.setCursor(9, 2);
		OSD.print(F("RED"));
		OSD.setCursor(15, 2);
		OSD.print(F("GREEN"));
		OSD.setCursor(23, 2);
		OSD.print(F("BLUE"));
		OSD.videoBackground();

		OSD.setCursor(0, 3);
		OSD.print(F("LED"));
		OSD.setCursor(0, 5);
		OSD.print(F("VBAT ALARM"));
		OSD.setCursor(0, 6);
		OSD.print(F("MAH ALARM"));
		OSD.setCursor(0, 7);
		OSD.print(F("LAP TRANSPONDER ID"));
		OSD.setCursor(0, 9);
		OSD.print(F("VTX CHANNEL"));
		OSD.setCursor(1, 10);
		OSD.print(F("LOW  POWER MW"));
		OSD.setCursor(1, 11);
		OSD.print(F("HIGH POWER MW"));
		CursorlineMaxLeft = 9;
		break;
	}
}

//print the actual menu-value
void MenuLeft_PrintValue() {
	switch (MenuPageLeft)
	{
	case 1:
		//PID
		OSD.setCursor(9, 3);
		OSD.print(((float)KissSettings.PID_P[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 3);
		ClearTempCharConverted();
		print_int16(KissSettings.PID_I[0], TempCharConverted, 3, 1);
		OSD.print(TempCharConverted);
		OSD.setCursor(24, 3);
		OSD.print(((float)KissSettings.PID_D[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(9, 4);
		OSD.print(((float)KissSettings.PID_P[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 4);
		ClearTempCharConverted();
		print_int16(KissSettings.PID_I[1], TempCharConverted, 3, 1);
		OSD.print(TempCharConverted);
		OSD.setCursor(24, 4);
		OSD.print(((float)KissSettings.PID_D[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(9, 5);
		OSD.print(((float)KissSettings.PID_P[2]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 5);
		ClearTempCharConverted();
		print_int16(KissSettings.PID_I[2], TempCharConverted, 3, 1);
		OSD.print(TempCharConverted);
		OSD.setCursor(24, 5);
		OSD.print(((float)KissSettings.PID_D[2]) / 1000);
		DisplaySpace();

		OSD.setCursor(9, 8);
		OSD.print(((float)KissSettings.RC_Rate[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 8);
		OSD.print(((float)KissSettings.Rate[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 8);
		OSD.print(((float)KissSettings.RC_Curve[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(9, 9);
		OSD.print(((float)KissSettings.RC_Rate[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 9);
		OSD.print(((float)KissSettings.Rate[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 9);
		OSD.print(((float)KissSettings.RC_Curve[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(9, 10);
		OSD.print(((float)KissSettings.RC_Rate[2]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 10);
		OSD.print(((float)KissSettings.Rate[2]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 10);
		OSD.print(((float)KissSettings.RC_Curve[2]) / 10);
		DisplaySpace();
		break;
	case 2:
		//PID
		OSD.setCursor(9, 3);
		OSD.print(((float)KissSettings.TPA[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 3);
		OSD.print(((float)KissSettings.TPA[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 3);
		OSD.print(((float)KissSettings.TPA[2]) / 1000);
		OSD.setCursor(9, 4);
		OSD.print(((float)KissSettings.PID_A[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 4);
		OSD.print(((float)KissSettings.PID_A[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 4);
		OSD.print(((float)KissSettings.PID_A[2]) / 1000);
		OSD.setCursor(24, 5);
		OSD.print(KissSettings.MaxAngle/14.3);
		OSD.setCursor(26, 5);
		OSD.print(" °");
		OSD.setCursor(16, 8);
		OSD.print(((float)KissSettings.ACC_Trim[0]) / 1000);
		DisplaySpace();
		OSD.setCursor(24, 8);
		OSD.print(((float)KissSettings.ACC_Trim[1]) / 1000);
		DisplaySpace();
		OSD.setCursor(16, 9);
		showONOFF(KissSettings.NotchPitch.Enabled);
		OSD.setCursor(24, 9);
		showONOFF(KissSettings.NotchRoll.Enabled);
		OSD.setCursor(16, 10);
		OSD.print(KissSettings.NotchPitch.CenterfFreq);
		OSD.print("HZ");
		DisplaySpace();
		OSD.setCursor(24, 10);
		OSD.print(KissSettings.NotchRoll.CenterfFreq);
		OSD.print("HZ");
		OSD.setCursor(16, 11);
		OSD.print(KissSettings.NotchPitch.CutoffFreq);
		OSD.print("HZ");
		DisplaySpace();
		OSD.setCursor(24, 11);
		OSD.print(KissSettings.NotchRoll.CutoffFreq);
		OSD.print("HZ");
		OSD.setCursor(24, 12);
		OSD.print(KissSettings.YawFilter);
		DisplaySpace();
		break;
	case 3:
		//PID
		OSD.setCursor(9, 3);
		OSD.print(KissSettings.RGB[0]);
		DisplaySpace();
		OSD.setCursor(15, 3);
		OSD.print(KissSettings.RGB[1]);
		DisplaySpace();
		OSD.setCursor(23, 3);
		OSD.print(KissSettings.RGB[2]);
		DisplaySpace();
		OSD.setCursor(23, 5);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissSettings.VbatAlarm, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'V';
		OSD.print(TempCharConverted);
		OSD.setCursor(23, 6);
		OSD.print(KissSettings.CapacityAlarm);
		OSD.write(SYM_MAH);
		DisplaySpace();
		OSD.setCursor(23, 7);
		OSD.print(KissSettings.LapTimerID);
		DisplaySpace();
		OSD.setCursor(23, 9);
		OSD.print(KissSettings.VTXChannel);
		DisplaySpace();
		OSD.setCursor(23, 10);
		OSD.print(KissSettings.VTXLowPower);
		DisplaySpace();
		OSD.setCursor(23, 11);
		OSD.print(KissSettings.VTXHighPower);
		DisplaySpace();
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
		case 1: changeval(addsub, 50, 65000, 50, &KissSettings.PID_P[0]);
			break;
		case 2: changeval(addsub, 10, 65000, 1, &KissSettings.PID_I[0]);
			break;
		case 3: changeval(addsub, 100, 65000, 100, &KissSettings.PID_D[0]);
			break;
		case 4: changeval(addsub, 50, 65000, 50, &KissSettings.PID_P[1]);
			break;
		case 5: changeval(addsub, 10, 65000, 1, &KissSettings.PID_I[1]);
			break;
		case 6: changeval(addsub, 100, 65000, 100, &KissSettings.PID_D[1]);
			break;
		case 7: changeval(addsub, 50, 65000, 50, &KissSettings.PID_P[2]);
			break;
		case 8: changeval(addsub, 10, 65000, 1, &KissSettings.PID_I[2]);
			break;
		case 9: changeval(addsub, 100, 65000, 100, &KissSettings.PID_D[2]);
			break;

		case 10: changeval(addsub, 10, 2000, 10, &KissSettings.RC_Rate[0]);
			break;
		case 11: changeval(addsub, 10, 2000, 10, &KissSettings.Rate[0]);
			break;
		case 12: changeval(addsub, 10, 2000, 10, &KissSettings.RC_Curve[0]);
			break;
		case 13: changeval(addsub, 10, 2000, 10, &KissSettings.RC_Rate[1]);
			break;
		case 14: changeval(addsub, 10, 2000, 10, &KissSettings.Rate[1]);
			break;
		case 15: changeval(addsub, 10, 2000, 10, &KissSettings.RC_Curve[1]);
			break;
		case 16: changeval(addsub, 100, 65000, 100, &KissSettings.RC_Rate[2]);
			break;
		case 17: changeval(addsub, 100, 65000, 100, &KissSettings.Rate[2]);
			break;
		case 18: changeval(addsub, 100, 65000, 100, &KissSettings.RC_Curve[2]);
			break;
		}
		break;
	case 2:
		//PID
		switch (CursorlineLeft)
		{
		case 1: changeval(addsub, 10, 65000, 10, &KissSettings.TPA[0]);
			break;
		case 2: changeval(addsub, 10, 65000, 10, &KissSettings.TPA[1]);
			break;
		case 3: changeval(addsub, 10, 65000, 10, &KissSettings.TPA[2]);
			break;
		case 4: changeval(addsub, 100, 65000, 100, &KissSettings.PID_A[0]);
			break;
		case 5: changeval(addsub, 100, 65000, 100, &KissSettings.PID_A[1]);
			break;
		case 6: changeval(addsub, 100, 65000, 100, &KissSettings.PID_A[2]);
			break;
		case 7: changeval(addsub, 15, 2574, 14, &KissSettings.MaxAngle);
			break;
		case 8: changeval(addsub, -30000,30000, 100, &KissSettings.ACC_Trim[0]);
			break;
		case 9: changeval(addsub, -30000, 30000, 100, &KissSettings.ACC_Trim[1]);
			break;
		case 10: changeval(addsub, 0, 1, 1, &KissSettings.NotchPitch.Enabled);
			break;
		case 11: changeval(addsub, 0, 1, 1, &KissSettings.NotchRoll.Enabled);
			break;
		case 12: changeval(addsub, 4, 486, 5, &KissSettings.NotchPitch.CenterfFreq);
			break;
		case 13: changeval(addsub, 4, 486, 5, &KissSettings.NotchRoll.CenterfFreq);
			break;
		case 14: changeval(addsub, 4, 486, 5, &KissSettings.NotchPitch.CutoffFreq);
			break;
		case 15: changeval(addsub, 4, 486, 5, &KissSettings.NotchRoll.CutoffFreq);
			break;
		case 16: changeval(addsub, 0, 250, 1, &KissSettings.YawFilter);
			break;
		}
		break;
	case 3:
		//PID
		switch (CursorlineLeft)
		{
		case 1: changeval(addsub, 0, 100, 1, &KissSettings.RGB[0]);
			break;
		case 2: changeval(addsub, 0, 100, 1, &KissSettings.RGB[1]);
			break;
		case 3: changeval(addsub, 0, 100, 1, &KissSettings.RGB[2]);
			break;
		case 4: changeval(addsub, 2, 26000, 2, &KissSettings.VbatAlarm);
			break;
		case 5: changeval(addsub, 10, 65000, 10, &KissSettings.CapacityAlarm);
			break;
		case 6: changeval(addsub, 0, 63, 1, &KissSettings.LapTimerID);
			break;
		case 7: changeval(addsub, 5645, 5945, 1, &KissSettings.VTXChannel); //TODO: not sure if this will work like this. Perhaps the Lookup table is needed
			break;
		case 8: changeval(addsub, 25, 800, 5, &KissSettings.VTXLowPower);
			break;
		case 9: changeval(addsub, 25, 800, 5, &KissSettings.VTXHighPower);
			break;
		}
		break;
	}
	PauseLeft = 1;
}

void MenuLeft_Marker(bool addMarker, uint8_t MarkerLine, uint8_t CurrentPage)
{
	switch (CurrentPage)
	{
	case 1:
		//PID-Page
		if (MarkerLine > 9)
			i = 2;
		else
			i = 0;
		while (MarkerLine > 3)
		{
			MarkerLine = MarkerLine - 3;
			i++;
		}
		switch (MarkerLine)
		{
		case 1:
			OSD.setCursor(8, 3 + i);
			break;
		case 2:
			OSD.setCursor(15, 3 + i);
				break;
		case 3:
			OSD.setCursor(23, 3 + i);
				break;
		}
		break;
	case 2:
		//TPA-Level Page
		i = 0;
		if (MarkerLine < 7)
		{
			if (MarkerLine > 3)
				i = 1;
			MarkerLine = MarkerLine - (i * 3);
			switch (MarkerLine)
			{
			case 1:
				OSD.setCursor(8, 3 + i);
				break;
			case 2:
				OSD.setCursor(15, 3 + i);
				break;
			case 3:
				OSD.setCursor(23, 3 + i);
				break;
			}
		}
		else if (MarkerLine == 7)
			OSD.setCursor(23, 5);
		else if (MarkerLine < 16)
		{
			if (MarkerLine > 13)
				i = 3;
			else if (MarkerLine > 11)
				i = 2;
			else if (MarkerLine > 9)
				i = 1;
			MarkerLine = MarkerLine - (i * 2);
			switch (MarkerLine - 7)
			{
			case 1:
				OSD.setCursor(15, 8 + i);
				break;
			case 2:
				OSD.setCursor(23, 8 + i);
				break;
			}
		}
		else
			OSD.setCursor(23, 12);
		break;
	case 3:
		//Various
		if (MarkerLine < 4)
		{
			switch (MarkerLine)
			{
			case 1:
				OSD.setCursor(8, 3);
				break;
			case 2:
				OSD.setCursor(14, 3);
				break;
			case 3:
				OSD.setCursor(22, 3);
				break;
			}
		}
		else if (MarkerLine == 4)
			OSD.setCursor(22, 5);
		else if (MarkerLine == 5)
			OSD.setCursor(22, 6);
		else
			OSD.setCursor(22, 7);
		break;
	}

	if (addMarker)
		OSD.print(F(">"));
	else
		OSD.print(F(" "));
}













