//=========================================
//Read serial data from FC and store in var
bool getSerialData(uint8_t Mode,bool CopyBuffToSett)	//reading serial Data from FC - Mode0:Telemetrie | Mode1:PID Settings
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
		if (millis() - KissStatus.LastLoopTime > 750)
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
			serialBuf[recBytes++] = Serial.read();
			

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
		{
			minBytes = serialBuf[1] + STARTCOUNT + 1; // got the transmission length
			//small error check
			if (minBytes < 150 || minBytes>180)
				minBytes = 63;
		}
			
			
		

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
			else if(Mode==GET_SETTINGS && kissProtocolCRC8(serialBuf, STARTCOUNT, minBytes-1) == serialBuf[recBytes - 1])
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

				if (CopyBuffToSett)
				{
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

					//KissSettings.ACC_Trim[1] = ((serialBuf[24 + STARTCOUNT] << 8) | serialBuf[25 + STARTCOUNT]);
					//KissSettings.ACC_Trim[2] = ((serialBuf[26 + STARTCOUNT] << 8) | serialBuf[27 + STARTCOUNT]);

					KissSettings.RC_Rate[0] = ((serialBuf[28 + STARTCOUNT] << 8) | serialBuf[29 + STARTCOUNT]);
					KissSettings.RC_Rate[1] = ((serialBuf[30 + STARTCOUNT] << 8) | serialBuf[31 + STARTCOUNT]);
					KissSettings.RC_Rate[2] = ((serialBuf[32 + STARTCOUNT] << 8) | serialBuf[33 + STARTCOUNT]);

					KissSettings.Rate[0] = ((serialBuf[34 + STARTCOUNT] << 8) | serialBuf[35 + STARTCOUNT]);
					KissSettings.Rate[1] = ((serialBuf[36 + STARTCOUNT] << 8) | serialBuf[37 + STARTCOUNT]);
					KissSettings.Rate[2] = ((serialBuf[38 + STARTCOUNT] << 8) | serialBuf[39 + STARTCOUNT]);

					KissSettings.RC_Curve[0] = ((serialBuf[40 + STARTCOUNT] << 8) | serialBuf[41 + STARTCOUNT]);
					KissSettings.RC_Curve[1] = ((serialBuf[42 + STARTCOUNT] << 8) | serialBuf[43 + STARTCOUNT]);
					KissSettings.RC_Curve[2] = ((serialBuf[44 + STARTCOUNT] << 8) | serialBuf[45 + STARTCOUNT]);

					KissSettings.MaxAngle = ((serialBuf[77 + STARTCOUNT] << 8) | serialBuf[78 + STARTCOUNT]);

					KissSettings.Version = serialBuf[92 + STARTCOUNT];

					KissSettings.TPA[0] = ((serialBuf[93 + STARTCOUNT] << 8) | serialBuf[94 + STARTCOUNT]);
					KissSettings.TPA[1] = ((serialBuf[95 + STARTCOUNT] << 8) | serialBuf[96 + STARTCOUNT]);
					KissSettings.TPA[2] = ((serialBuf[97 + STARTCOUNT] << 8) | serialBuf[98 + STARTCOUNT]);

					KissSettings.RGB[0] = 100.0 * (serialBuf[122 + STARTCOUNT]/255.0);
					KissSettings.RGB[1] = 100.0 * (serialBuf[123 + STARTCOUNT] / 255.0);
					KissSettings.RGB[2] = 100.0 * (serialBuf[124 + STARTCOUNT] / 255.0);
					KissSettings.VbatAlarm = ((serialBuf[125 + STARTCOUNT] << 8) | serialBuf[126 + STARTCOUNT]);

					KissSettings.LapTimerID = ((serialBuf[135 + STARTCOUNT] << 8) | serialBuf[136 + STARTCOUNT]);

					//KissSettings.NotchRoll.Enabled = serialBuf[138 + STARTCOUNT];
					//KissSettings.NotchRoll.CenterfFreq = ((serialBuf[139 + STARTCOUNT] << 8) | serialBuf[140 + STARTCOUNT]);
					//KissSettings.NotchRoll.CutoffFreq = ((serialBuf[141 + STARTCOUNT] << 8) | serialBuf[142 + STARTCOUNT]);
					//KissSettings.NotchPitch.Enabled = serialBuf[143 + STARTCOUNT];
					//KissSettings.NotchPitch.CenterfFreq = ((serialBuf[144 + STARTCOUNT] << 8) | serialBuf[145 + STARTCOUNT]);
					//KissSettings.NotchPitch.CutoffFreq = ((serialBuf[146 + STARTCOUNT] << 8) | serialBuf[147 + STARTCOUNT]);
					KissSettings.YawFilter = serialBuf[148 + STARTCOUNT];

					KissSettings.CapacityAlarm = ((serialBuf[157 + STARTCOUNT] << 8) | serialBuf[158 + STARTCOUNT]);

					KissSettings.MotorBuzzer = serialBuf[163 + STARTCOUNT];

					//Get VTX Settings
					KissSettings.VTXChannel = serialBuf[120 + STARTCOUNT];
					KissSettings.VTXLowPower = ((serialBuf[150 + STARTCOUNT] << 8) | serialBuf[151 + STARTCOUNT]);
					KissSettings.VTXHighPower = ((serialBuf[152 + STARTCOUNT] << 8) | serialBuf[153 + STARTCOUNT]);

					//Get LPF Filter
					KissSettings.YawLPF = serialBuf[165 + STARTCOUNT];
					KissSettings.DLPF = serialBuf[166 + STARTCOUNT];
					KissSettings.PrLPF = serialBuf[79 + STARTCOUNT];
					KissSettings.AdaptFilter = serialBuf[169 + STARTCOUNT];
				}
				
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

			if (KissConnection == LostConnection)
				KissConnection = ConnectionEtablished;
			return true;
		}
	} //end of aquiring telemetry data
}

bool setSerialData()
{
	//Clear OSD and show a saving message
	OSD.clear();
	while (OSD.clearIsBusy());
	OSD.home();
	OSD.print(F("SENDING TO FC..."));

	//Re-aquire settings from FC
	i = 0;
	while (!getSerialData(GET_SETTINGS,false))
	{
		delay(100);
		i++;
		if (i > 10)								//Unable to re-aquire data
		{
			KissConnection = LostConnection;
			return false;		
		}	
	}
		

	//save the changed settings in the serialBuf-Array
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
	serialBuf[STARTCOUNT + 13] = (byte)(KissSettings.PID_D[0] & 0x00FF);
	serialBuf[STARTCOUNT + 14] = (byte)((KissSettings.PID_D[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 15] = (byte)(KissSettings.PID_D[1] & 0x00FF);
	serialBuf[STARTCOUNT + 16] = (byte)((KissSettings.PID_D[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 17] = (byte)(KissSettings.PID_D[2] & 0x00FF);
	serialBuf[STARTCOUNT + 18] = (byte)((KissSettings.PID_A[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 19] = (byte)(KissSettings.PID_A[0] & 0x00FF);
	serialBuf[STARTCOUNT + 20] = (byte)((KissSettings.PID_A[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 21] = (byte)(KissSettings.PID_A[1] & 0x00FF);
	serialBuf[STARTCOUNT + 22] = (byte)((KissSettings.PID_A[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 23] = (byte)(KissSettings.PID_A[2] & 0x00FF);
	//serialBuf[STARTCOUNT + 24] = (byte)((KissSettings.ACC_Trim[0] & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 25] = (byte)(KissSettings.ACC_Trim[0] & 0x00FF);
	//serialBuf[STARTCOUNT + 26] = (byte)((KissSettings.ACC_Trim[1] & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 27] = (byte)(KissSettings.ACC_Trim[1] & 0x00FF);
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

	serialBuf[STARTCOUNT + 77] = (byte)((KissSettings.MaxAngle & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 78] = (byte)(KissSettings.MaxAngle & 0x00FF);
	serialBuf[STARTCOUNT + 79] = KissSettings.PrLPF;

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

	//and here we could save again our Settings
	serialBuf[STARTCOUNT + 80] = (byte)((KissSettings.TPA[0] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 81] = (byte)(KissSettings.TPA[0] & 0x00FF);
	serialBuf[STARTCOUNT + 82] = (byte)((KissSettings.TPA[1] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 83] = (byte)(KissSettings.TPA[1] & 0x00FF);
	serialBuf[STARTCOUNT + 84] = (byte)((KissSettings.TPA[2] & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 85] = (byte)(KissSettings.TPA[2] & 0x00FF);

	serialBuf[STARTCOUNT + 112] = (KissSettings.RGB[0] * 255.0)/100;
	serialBuf[STARTCOUNT + 113] = (KissSettings.RGB[1] * 255.0) / 100;
	serialBuf[STARTCOUNT + 114] = (KissSettings.RGB[2] * 255.0) / 100;
	serialBuf[STARTCOUNT + 115] = (byte)((KissSettings.VbatAlarm & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 116] = (byte)(KissSettings.VbatAlarm & 0x00FF);

	serialBuf[STARTCOUNT + 125] = (byte)((KissSettings.LapTimerID & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 126] = (byte)(KissSettings.LapTimerID & 0x00FF);

	//serialBuf[STARTCOUNT + 128] = KissSettings.NotchRoll.Enabled;
	//serialBuf[STARTCOUNT + 129] = (byte)((KissSettings.NotchRoll.CenterfFreq & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 130] = (byte)(KissSettings.NotchRoll.CenterfFreq & 0x00FF);
	//serialBuf[STARTCOUNT + 131] = (byte)((KissSettings.NotchRoll.CutoffFreq & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 132] = (byte)(KissSettings.NotchRoll.CutoffFreq & 0x00FF);
	//serialBuf[STARTCOUNT + 133] = KissSettings.NotchPitch.Enabled;
	//serialBuf[STARTCOUNT + 134] = (byte)((KissSettings.NotchPitch.CenterfFreq & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 135] = (byte)(KissSettings.NotchPitch.CenterfFreq & 0x00FF);
	//serialBuf[STARTCOUNT + 136] = (byte)((KissSettings.NotchPitch.CutoffFreq & 0xFF00) >> 8);
	//serialBuf[STARTCOUNT + 137] = (byte)(KissSettings.NotchPitch.CutoffFreq & 0x00FF);
	serialBuf[STARTCOUNT + 138] = (byte)(KissSettings.YawFilter);

	serialBuf[STARTCOUNT + 147] = (byte)((KissSettings.CapacityAlarm& 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 148] = (byte)(KissSettings.CapacityAlarm & 0x00FF);

	serialBuf[STARTCOUNT + 152] = KissSettings.MotorBuzzer;

	//set VTX settings
	serialBuf[STARTCOUNT + 110] = KissSettings.VTXChannel;
	serialBuf[STARTCOUNT + 140] = (byte)((KissSettings.VTXLowPower & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 141] = (byte)(KissSettings.VTXLowPower & 0x00FF);
	serialBuf[STARTCOUNT + 142] = (byte)((KissSettings.VTXHighPower & 0xFF00) >> 8);
	serialBuf[STARTCOUNT + 143] = (byte)(KissSettings.VTXHighPower & 0x00FF);

	//LPF Filtering
	serialBuf[STARTCOUNT + 154] = KissSettings.YawLPF;
	serialBuf[STARTCOUNT + 155] = KissSettings.DLPF;

	//Adapt Filter
	serialBuf[STARTCOUNT + 158] = KissSettings.AdaptFilter;


	//calculate Checksum
	//new method
	serialBuf[KissSettings.minBytesSettings - 1] = kissProtocolCRC8(serialBuf, STARTCOUNT, KissSettings.minBytesSettings - 1);

	i = 0;
	uint8_t serialBuf2[5] = { 0 };

	Serial.write(SET_SETTINGS);
	//delay(2);

	//Alternative Mode:
	for (i = 0; i<KissSettings.minBytesSettings; i++)
		Serial.write(serialBuf[i]);

	/*Serial.write(serialBuf, sizeof(serialBuf));
	Serial.flush();*/

	/*delay(50);
	while (Serial.available() && i<5)
	{
		serialBuf2[i] = Serial.read();
		i++;
	}



	/*OSD.setCursor(0, 0);
	OSD.print(serialBuf2[0]);
	OSD.setCursor(0, 1);
	OSD.print(serialBuf2[1]);
	OSD.setCursor(0, 2);
	OSD.print(serialBuf2[2]);
	OSD.setCursor(0, 3);
	OSD.print(serialBuf2[3]);
	OSD.setCursor(0, 4);
	OSD.print(serialBuf[KissSettings.minBytesSettings - 1]);
	while (1)
	{
	}*/

	/*if (serialBuf2[0] == 5 && serialBuf2[1] == 1 && serialBuf2[2] == 6)
		return true;
	else
	{
		//resending the data a second time
		Serial.write(SET_SETTINGS);
		for (i = 0; i<KissSettings.minBytesSettings; i++)
			Serial.write(serialBuf[i]);
		delay(50);
		while (Serial.available() && i<5)
		{
			serialBuf2[i] = Serial.read();
			i++;
		}
		if (serialBuf2[0] == 5 && serialBuf2[1] == 1 && serialBuf2[2] == 6)
			return true;
		else
			return false;

	}*/

	return true;
}


//new CRC-Method for the Kiss FC
uint8_t kissProtocolCRC8(const uint8_t *data, uint8_t startIndex, uint8_t stopIndex)
{
	uint8_t crc = 0;
	for (i = startIndex; i < stopIndex; i++)
	{
		crc ^= data[i];
		for (uint8_t j = 0; j < 8; j++)
		{
			if ((crc & 0x80) != 0)
				crc = (uint8_t)((crc << 1) ^ 0xD5);
			else
				crc <<= 1;
		}
	}
	return crc;
}