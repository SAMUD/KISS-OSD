/* 1. Information
//=========================================================================================================================

Fonctions needed for the settings-menu off the OSD.

//=========================================================================================================================
//START OF CODE
*/

void menumain()
{
	static uint32_t LastLoopTimemenu;
	static uint8_t cursorline=0;
	static uint8_t pause=0; //makes a pause of multiples of 50ms
	static boolean exitmenu=false;

	while (!exitmenu)
	{
		if (millis() - LastLoopTimemenu > 100) //limits the speed of the OSD to 10Hz
		{
			LastLoopTime = micros();

			getSerialData();

			//set cursor position
			if (StickChanVals[2] < -500 && cursorline<5)
			{
				cursorline++;
				pause=4;
			}
			if (StickChanVals[2] > 500 && cursorline>0)
			{
				cursorline--;
				pause=4;
			}

			//changevalue
			if (StickChanVals[1] > 500)
			{
				ValuePage1[cursorline]= ValuePage1[cursorline]+5;
			}
			if (StickChanVals[1] < -500)
			{
				ValuePage1[cursorline] = ValuePage1[cursorline] - 5;
			}
			




			OSD.clear();
			OSD.setCursor(0, 0);
			OSD.print("SAMUD MENU - P1/1  ");
			
			//print menu site
			menuprintsite();

			//draw cursor position
			OSD.setCursor(23,cursorline+2);
			OSD.print(">");

			//pause (multiples of 50ms)
			while (pause > 0)
			{
				delay(50);
				pause--;
			}

			if (armed == 1 || ValuePage1[5] > 0 || StickChanVals[3]< -500)
			{
				exitmenu=true;
			}

			





			//exit OSD
			//if(StickChan)
		}
	}
	exitmenu=false;
}

void menuprintsite() {
	for (int i = 0;i < 11;i++)
	{
		OSD.setCursor(1,i+2);
		OSD.print(MenuPage1[i]);
		OSD.setCursor(24,i+2);
		OSD.print(ValuePage1[i]);
	}
}




