//=========================================
//Read serial data from FC and store in var
bool getSerialData(uint8_t Mode)	//reading serial Data from FC - Mode0:Telemetrie | Mode1:PID Settings
{
	static uint32_t tmpVoltage = 0;
	static uint8_t voltDev = 0;
	static uint32_t windedupfilterdatas[8];
	uint8_t minBytes = 63;
	uint8_t recBytes = 0;
	uint8_t exitreceiving = 0;

	serialBuf[0] = 0;
	serialBuf[1] = 0;

	
	Serial.write(Mode);	//request data from FC
	
	//aquire serial data and write it to normal variables
	while (exitreceiving == 0)
	{
		//Running already to long in this loop
		if (micros() - KissStatus.LastLoopTime > 750000)
		{
			KissConnection = LostConnection;
			exitreceiving = 1;
			#ifdef DEBUG
			Debug_Fnc("TO LONG WAIT");
			OSD.setCursor(0, 10);
			OSD.print(recBytes);
			OSD.setCursor(0, 11);
			OSD.print(minBytes);
			OSD.setCursor(0, 12);
			OSD.print((uint8_t)serialBuf[1]);
			#endif
		}

		//Copy all received data into buffer
		while (Serial.available())
		{
			serialBuf[recBytes++] = Serial.read();
		}
			

		//Check for start byte - if wrong exit 
		if (recBytes == 1 && serialBuf[0] != 5)
		{
			exitreceiving = 1;
			KissConnection = LostConnection;
			#ifdef DEBUG
			Debug_Fnc("START BYTE ERR");
			#endif // DEBUG
		}

		//Check for transmission length
		if (recBytes == 2)
			minBytes = serialBuf[1] + STARTCOUNT + 1; // got the transmission length
			
		

		//All data is here
		if (recBytes == minBytes)
		{
			uint32_t checksum = 0;
			
			for (i = 2; i<minBytes; i++)
				checksum += serialBuf[i];
			
			checksum = (uint32_t)checksum / (minBytes - 3);

			if (checksum == serialBuf[recBytes - 1] &&  Mode == GET_TELEMETRY)
			{
				#ifdef DEBUG
				OSD.setCursor(0, 10);
				OSD.print(recBytes);
				OSD.setCursor(0, 11);
				OSD.print(minBytes);
				OSD.setCursor(0, 12);
				OSD.print((uint8_t)serialBuf[1]);
				#endif

				KissTelemetrie.armed = ((serialBuf[15 + STARTCOUNT] << 8) | serialBuf[16 + STARTCOUNT]);
				KissTelemetrie.LipoVoltage = ((serialBuf[17 + STARTCOUNT] << 8) | serialBuf[18 + STARTCOUNT]);
				KissTelemetrie.failsafe = ((serialBuf[40 + STARTCOUNT] << 8) | (serialBuf[41 + STARTCOUNT])); 
				KissTelemetrie.calibGyroDone = ((serialBuf[39 + STARTCOUNT] << 8) | serialBuf[40 + STARTCOUNT]);
				KissTelemetrie.angley = ((serialBuf[33 + STARTCOUNT] << 8) | (serialBuf[34 + STARTCOUNT])) / 100; //35

				tmpVoltage = 0;
				voltDev = 0;
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
					KissTelemetrie.LipoVoltage = (tmpVoltage / voltDev);
				KissTelemetrie.LipoVoltage = KissTelemetrie.LipoVoltage + Settings.VoltageOffset;

				//capacity
				KissTelemetrie.LipoMAH = ((serialBuf[148 + STARTCOUNT] << 8) | serialBuf[149 + STARTCOUNT]);
				//read Motor Current and other ESC datas
				
				//RPM
				// TODO Try to remove the filter and see if it is working without
				windedupfilterdatas[0] = ESC_filter((uint32_t)windedupfilterdatas[0], (uint32_t)((serialBuf[91 + STARTCOUNT] << 8) | serialBuf[92 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[1] = ESC_filter((uint32_t)windedupfilterdatas[1], (uint32_t)((serialBuf[101 + STARTCOUNT] << 8) | serialBuf[102 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[2] = ESC_filter((uint32_t)windedupfilterdatas[2], (uint32_t)((serialBuf[111 + STARTCOUNT] << 8) | serialBuf[112 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				windedupfilterdatas[3] = ESC_filter((uint32_t)windedupfilterdatas[3], (uint32_t)((serialBuf[121 + STARTCOUNT] << 8) | serialBuf[122 + STARTCOUNT]) / (Settings.MAGNETPOLECOUNT / 2) << 4);
				KissTelemetrie.motorKERPM[0] = windedupfilterdatas[0] >> 4;
				KissTelemetrie.motorKERPM[1] = windedupfilterdatas[1] >> 4;
				KissTelemetrie.motorKERPM[2] = windedupfilterdatas[2] >> 4;
				KissTelemetrie.motorKERPM[3] = windedupfilterdatas[3] >> 4;
				//Current
				windedupfilterdatas[4] = ESC_filter((uint32_t)windedupfilterdatas[4], (uint32_t)((serialBuf[87 + STARTCOUNT] << 8) | serialBuf[88 + STARTCOUNT]) << 4);
				windedupfilterdatas[5] = ESC_filter((uint32_t)windedupfilterdatas[5], (uint32_t)((serialBuf[97 + STARTCOUNT] << 8) | serialBuf[98 + STARTCOUNT]) << 4);
				windedupfilterdatas[6] = ESC_filter((uint32_t)windedupfilterdatas[6], (uint32_t)((serialBuf[107 + STARTCOUNT] << 8) | serialBuf[108 + STARTCOUNT]) << 4);
				windedupfilterdatas[7] = ESC_filter((uint32_t)windedupfilterdatas[7], (uint32_t)((serialBuf[117 + STARTCOUNT] << 8) | serialBuf[118 + STARTCOUNT]) << 4);
				KissTelemetrie.motorCurrent[0] = windedupfilterdatas[4] >> 4;
				KissTelemetrie.motorCurrent[1] = windedupfilterdatas[5] >> 4;
				KissTelemetrie.motorCurrent[2] = windedupfilterdatas[6] >> 4;
				KissTelemetrie.motorCurrent[3] = windedupfilterdatas[7] >> 4;
				//ESC Temps
				KissTelemetrie.ESCTemps[0] = ((serialBuf[83 + STARTCOUNT] << 8) | serialBuf[84 + STARTCOUNT]);
				KissTelemetrie.ESCTemps[1] = ((serialBuf[93 + STARTCOUNT] << 8) | serialBuf[94 + STARTCOUNT]);
				KissTelemetrie.ESCTemps[2] = ((serialBuf[103 + STARTCOUNT] << 8) | serialBuf[104 + STARTCOUNT]);
				KissTelemetrie.ESCTemps[3] = ((serialBuf[113 + STARTCOUNT] << 8) | serialBuf[114 + STARTCOUNT]);
				//Aux chan vals
				KissTelemetrie.AuxChanVals[0] = ((serialBuf[8 + STARTCOUNT] << 8) | serialBuf[9 + STARTCOUNT]);
				KissTelemetrie.AuxChanVals[1] = ((serialBuf[10 + STARTCOUNT] << 8) | serialBuf[11 + STARTCOUNT]);
				KissTelemetrie.AuxChanVals[2] = ((serialBuf[12 + STARTCOUNT] << 8) | serialBuf[13 + STARTCOUNT]);
				KissTelemetrie.AuxChanVals[3] = ((serialBuf[14 + STARTCOUNT] << 8) | serialBuf[15 + STARTCOUNT]);
				//StickChanVals
				KissTelemetrie.StickChanVals[0] = ((serialBuf[0 + STARTCOUNT] << 8) | serialBuf[1 + STARTCOUNT]);
				KissTelemetrie.StickChanVals[1] = ((serialBuf[2 + STARTCOUNT] << 8) | serialBuf[3 + STARTCOUNT]);
				KissTelemetrie.StickChanVals[2] = ((serialBuf[4 + STARTCOUNT] << 8) | serialBuf[5 + STARTCOUNT]);
				KissTelemetrie.StickChanVals[3] = ((serialBuf[6 + STARTCOUNT] << 8) | serialBuf[7 + STARTCOUNT]);
				
			}
			else if(/*checksum == serialBuf[recBytes - 1] &&*/ Mode==GET_SETTINGS) 
			{
				#ifdef DEBUG
				OSD.setCursor(0, 10);
				OSD.print(recBytes);
				OSD.setCursor(0, 11);
				OSD.print(minBytes);
				OSD.setCursor(0, 12);
				OSD.print((uint8_t)serialBuf[1]);
				Debug_Fnc("COPY SETTINGS");
				#endif

				KissSettings.minBytesSettings = minBytes;
				
				//Copy data into this static array, because we need to send this back with only the changed values
				//memcpy(serialBuf, serialBuf, recBytes);
				#ifdef DEBUG
				Debug_Fnc("MEMCOPY");
				#endif

				KissSettings.PID_P[0] = ((serialBuf[0 + STARTCOUNT] << 8) | serialBuf[1 + STARTCOUNT]);
				KissSettings.PID_P[1] = ((serialBuf[2 + STARTCOUNT] << 8) | serialBuf[3 + STARTCOUNT]);
				KissSettings.PID_P[2] = ((serialBuf[4 + STARTCOUNT] << 8) | serialBuf[5 + STARTCOUNT]);

				KissSettings.PID_I[0] = ((serialBuf[6 + STARTCOUNT] << 8) | serialBuf[7 + STARTCOUNT]);
				KissSettings.PID_I[1] = ((serialBuf[8 + STARTCOUNT] << 8) | serialBuf[9 + STARTCOUNT]);
				KissSettings.PID_I[2] = ((serialBuf[10 + STARTCOUNT] << 8) | serialBuf[11 + STARTCOUNT]);

				KissSettings.PID_D[0] = ((serialBuf[12 + STARTCOUNT] << 8) | serialBuf[13 + STARTCOUNT]);
				KissSettings.PID_D[1] = ((serialBuf[14 + STARTCOUNT] << 8) | serialBuf[15 + STARTCOUNT]);
				KissSettings.PID_D[2] = ((serialBuf[16 + STARTCOUNT] << 8) | serialBuf[17 + STARTCOUNT]);

				KissSettings.PID_A[0] = ((serialBuf[18 + STARTCOUNT] << 8) | serialBuf[19 + STARTCOUNT]);
				KissSettings.PID_A[1] = ((serialBuf[20 + STARTCOUNT] << 8) | serialBuf[21 + STARTCOUNT]);
				KissSettings.PID_A[2] = ((serialBuf[22 + STARTCOUNT] << 8) | serialBuf[23 + STARTCOUNT]);

				KissSettings.ACC_Trim[1] = ((serialBuf[24 + STARTCOUNT] << 8) | serialBuf[25 + STARTCOUNT]);
				KissSettings.ACC_Trim[2] = ((serialBuf[26 + STARTCOUNT] << 8) | serialBuf[27 + STARTCOUNT]);

				KissSettings.RC_Rate[0] = ((serialBuf[28 + STARTCOUNT] << 8) | serialBuf[29 + STARTCOUNT]);
				KissSettings.RC_Rate[1] = ((serialBuf[30 + STARTCOUNT] << 8) | serialBuf[31 + STARTCOUNT]);
				KissSettings.RC_Rate[2] = ((serialBuf[32 + STARTCOUNT] << 8) | serialBuf[33 + STARTCOUNT]);

				KissSettings.Rate[0] = ((serialBuf[34 + STARTCOUNT] << 8) | serialBuf[35 + STARTCOUNT]);
				KissSettings.Rate[1] = ((serialBuf[36 + STARTCOUNT] << 8) | serialBuf[37 + STARTCOUNT]);
				KissSettings.Rate[2] = ((serialBuf[38 + STARTCOUNT] << 8) | serialBuf[39 + STARTCOUNT]);

				KissSettings.RC_Curve[0] = ((serialBuf[40 + STARTCOUNT] << 8) | serialBuf[41 + STARTCOUNT]);
				KissSettings.RC_Curve[1] = ((serialBuf[42 + STARTCOUNT] << 8) | serialBuf[43 + STARTCOUNT]);
				KissSettings.RC_Curve[2] = ((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]);
			}
			#ifdef DEBUG
			else if (Mode == GET_SETTINGS)
			{
				Debug_Fnc("SETT ERR");
				return false;
			}
				
			else if (Mode == GET_TELEMETRY)
				Debug_Fnc("TELM ERR");
			#endif // DEBUG

			

			if (KissConnection == LostConnection || KissConnection == WaitingForConn)
				KissConnection = ConnectionEtablished;
			return true;
		}
	} //end of aquiring telemetry data
}

void setSerialData()
{
	OSD.clear();
	while (OSD.clearIsBusy());
	OSD.home();
	OSD.print(F("SENDING TO FC..."));
	while (!getSerialData(GET_SETTINGS))
		delay(500);

	
	//save the new values in the serialBuf-Array
	serialBuf[STARTCOUNT + 0] = (byte)((KissSettings.PID_P[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 1] = (byte)(KissSettings.PID_P[0] & 0x00FF);
	serialBuf[STARTCOUNT + 2] = (byte)((KissSettings.PID_P[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 3] = (byte)(KissSettings.PID_P[1] & 0x00FF);
	serialBuf[STARTCOUNT + 4] = (byte)((KissSettings.PID_P[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 5] = (byte)(KissSettings.PID_P[2] & 0x00FF);
	serialBuf[STARTCOUNT + 6] = (byte)((KissSettings.PID_I[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 7] = (byte)(KissSettings.PID_I[0] & 0x00FF);
	serialBuf[STARTCOUNT + 8] = (byte)((KissSettings.PID_I[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 9] = (byte)(KissSettings.PID_I[1] & 0x00FF);
	serialBuf[STARTCOUNT + 10] = (byte)((KissSettings.PID_I[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 11] = (byte)(KissSettings.PID_I[2] & 0x00FF);
	serialBuf[STARTCOUNT + 12] = (byte)((KissSettings.PID_D[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 13] = (byte)(KissSettings.PID_I[0] & 0x00FF);
	serialBuf[STARTCOUNT + 14] = (byte)((KissSettings.PID_I[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 15] = (byte)(KissSettings.PID_I[1] & 0x00FF);
	serialBuf[STARTCOUNT + 16] = (byte)((KissSettings.PID_I[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 17] = (byte)(KissSettings.PID_I[2] & 0x00FF);

	serialBuf[STARTCOUNT + 28] = (byte)((KissSettings.RC_Rate[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 29] = (byte)(KissSettings.RC_Rate[0] & 0x00FF);
	serialBuf[STARTCOUNT + 30] = (byte)((KissSettings.RC_Rate[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 31] = (byte)(KissSettings.RC_Rate[1] & 0x00FF);
	serialBuf[STARTCOUNT + 32] = (byte)((KissSettings.RC_Rate[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 33] = (byte)(KissSettings.RC_Rate[2] & 0x00FF);

	serialBuf[STARTCOUNT + 34] = (byte)((KissSettings.Rate[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 35] = (byte)(KissSettings.Rate[0] & 0x00FF);
	serialBuf[STARTCOUNT + 36] = (byte)((KissSettings.Rate[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 37] = (byte)(KissSettings.Rate[1] & 0x00FF);
	serialBuf[STARTCOUNT + 38] = (byte)((KissSettings.Rate[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 39] = (byte)(KissSettings.Rate[2] & 0x00FF);

	serialBuf[STARTCOUNT + 40] = (byte)((KissSettings.RC_Curve[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 41] = (byte)(KissSettings.RC_Curve[0] & 0x00FF);
	serialBuf[STARTCOUNT + 42] = (byte)((KissSettings.RC_Curve[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 43] = (byte)(KissSettings.RC_Curve[1] & 0x00FF);
	serialBuf[STARTCOUNT + 44] = (byte)((KissSettings.RC_Curve[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 45] = (byte)(KissSettings.RC_Curve[2] & 0x00FF);

	//now we need to shift other settings forward, because we do not want to send the serial number to the FC
	for (i = 93; i < 101; i++)
	{
		serialBuf[STARTCOUNT + i - 13] = serialBuf[STARTCOUNT + i]; // skipping serial number
	}
	for (i = 0; i<4; i++)
	{
		serialBuf[STARTCOUNT + 88 + i] = 0; //controller activation
	}
	serialBuf[STARTCOUNT + 92] = serialBuf[STARTCOUNT + 101]; // data.setUint8(92, obj.BoardRotation, 0); obj.BoardRotation = data.getUint8(101);
	for (i = 103; i < (256 - STARTCOUNT); i++)
	{
		serialBuf[STARTCOUNT + i - 10] = serialBuf[STARTCOUNT + i]; // skipped obj.isActive = data.getUint8(102);
	}
	
	//calculate Checksum
	double checksum = 0.0;
	double dataCount = 0.0;	// TODO: remove dataCount --> use i-3 in the for-loop below
	for (i = 2; i<KissSettings.minBytesSettings; i++)
	{
		checksum += serialBuf[i];
		dataCount++;
	}
	checksum = checksum / dataCount;
	serialBuf[KissSettings.minBytesSettings - 1] = floor(checksum);

	OSD.setCursor(0, 10);
	OSD.print(KissSettings.minBytesSettings);
	OSD.setCursor(0, 11);
	OSD.print(serialBuf[1]);
	//serialBuf[1] = KissSettings.minBytesSettings - STARTCOUNT - 1;
	
	Serial.write(SET_SETTINGS);
	Serial.flush();
	//delay(25);
	
	//Serial.write(serialBuf, sizeof(serialBuf));

	//Alternative Mode:
	for (i = 0; i<KissSettings.minBytesSettings; i++)
	{
		
		
		Serial.write(serialBuf[i]);
		//Serial.flush();
		//delay(1);
	}

	delay(250);

}
