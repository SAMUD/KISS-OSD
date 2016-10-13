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

			Serial.write(0x20); // request telemetrie

			minBytes = 100;
			recBytes = 0;
		
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
					for (i = 2;i<minBytes;i++)
					{
						checksum += serialBuf[i];
					}
					checksum = (uint32_t)checksum / (minBytes - 3);

					if (checksum == serialBuf[recBytes - 1])
					{
						//armed
						armed = ((serialBuf[15 + STARTCOUNT] << 8) | serialBuf[16 + STARTCOUNT]);
						
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
					}
				}
			}

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
			OSD.print("SamuD - Menu - yaw left to exit  ");
			
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




