
void EEPROMinit()
{
	EEPROM.setMemPool(32, EEPROMSizeATmega328);
	KissStatus.configAdress = EEPROM.getAddress(sizeof(StoreStruct));
	KissStatus.memValid = EEPROMloadConfig();

	if (!KissStatus.memValid)
		EEPROMsaveDefaultValues();
}

bool EEPROMloadConfig()
{
	EEPROM.readBlock(KissStatus.configAdress, Settings);
	return (Settings.MemoryVersion == DMemoryVersion);
}

void EEPROMsaveDefaultValues()
{
	Settings.Capacity = DCapacity;
	Settings.Capacity1st = DCapacity1st;
	Settings.Capacity2nd = DCapacity2nd;
	Settings.ESC_FILTER = DESC_FILTER;
	Settings.hysteresis = Dhysteresis;
	Settings.LowVoltage1st = DLowVoltage1st;
	Settings.LowVoltage2nd = DLowVoltage2nd;
	Settings.LowVoltageAllowed = DLowVoltageAllowed
	Settings.MAGNETPOLECOUNT = DMAGNETPOLECOUNT;
	Settings.marginLastRow = DmarginLastRow;
	Settings.MemoryVersion = DMemoryVersion;
	Settings.RED_MODE_AUX_CHANNEL = DRED_MODE_AUX_CHAN;

	Settings.DispRCThrottle3 = RED3_DISPLAY_RC_THROTTLE;
	Settings.DispCombCurrent3 = RED3_DISPLAY_COMB_CURRENT;
	Settings.DispLipoVoltage3 = RED3_DISPLAY_LIPO_VOLTAGE;
	Settings.DispMaConsumption3 = RED3_DISPLAY_MA_CONSUMPTION;
	Settings.DispEscKrpm3 = RED3_DISPLAY_ESC_KRPM;
	Settings.DispEscCurrent3 = RED3_DISPLAY_ESC_CURRENT;
	Settings.DispEscTemp3 = RED3_DISPLAY_ESC_TEMPERATURE;
	Settings.DispPilotname3 = RED3_DISPLAY_PILOTNAME;
	Settings.DispTimer3 = RED3_DISPLAY_PILOTNAME;
	Settings.DispAngle3 = RED3_DISPLAY_PILOTNAME;

	Settings.DispRCThrottle2 = RED2_DISPLAY_RC_THROTTLE;
	Settings.DispCombCurrent2 = RED2_DISPLAY_COMB_CURRENT;
	Settings.DispLipoVoltage2 = RED2_DISPLAY_LIPO_VOLTAGE;
	Settings.DispMaConsumption2 = RED2_DISPLAY_MA_CONSUMPTION;
	Settings.DispEscKrpm2 = RED2_DISPLAY_ESC_KRPM;
	Settings.DispEscCurrent2 = RED2_DISPLAY_ESC_CURRENT;
	Settings.DispEscTemp2 = RED2_DISPLAY_ESC_TEMPERATURE;
	Settings.DispPilotname2 = RED2_DISPLAY_PILOTNAME;
	Settings.DispTimer2 = RED2_DISPLAY_PILOTNAME;
	Settings.DispAngle2 = RED2_DISPLAY_PILOTNAME;

	Settings.DispRCThrottle1 = RED_DISPLAY_RC_THROTTLE;
	Settings.DispCombCurrent1 = RED_DISPLAY_COMB_CURRENT;
	Settings.DispLipoVoltage1 = RED_DISPLAY_LIPO_VOLTAGE;
	Settings.DispMaConsumption1 = RED_DISPLAY_MA_CONSUMPTION;
	Settings.DispEscKrpm1 = RED_DISPLAY_ESC_KRPM;
	Settings.DispEscCurrent1 = RED_DISPLAY_ESC_CURRENT;
	Settings.DispEscTemp1 = RED_DISPLAY_ESC_TEMPERATURE;
	Settings.DispPilotname1 = RED_DISPLAY_PILOTNAME;
	Settings.DispTimer1 = RED_DISPLAY_PILOTNAME;
	Settings.DispAngle1 = RED_DISPLAY_PILOTNAME;

	Settings.OffsetX = DOffsetX;
	Settings.OffsetY = DOffsetY;

	Settings.StandbyCurrent = DStandbyCurrent;

	Settings.PilotName[0] = DPilotName[0];
	Settings.PilotName[1] = DPilotName[1];
	Settings.PilotName[2] = DPilotName[2];
	Settings.PilotName[3] = DPilotName[3];
	Settings.PilotName[4] = DPilotName[4];
	Settings.PilotName[5] = DPilotName[5];
	Settings.PilotName[6] = DPilotName[6];
	Settings.PilotName[7] = DPilotName[7];
	Settings.PilotName[8] = Settings.PilotName[9] = Settings.PilotName[10] = Settings.PilotName[11] = DPilotName[8];


	Settings.stockSettings = 0;

	EEPROMsave();

}

void EEPROMsave()
{
	#ifdef DEBUG
	Debug_Fnc("EEPROM SAVE");
	#endif
	EEPROM.updateBlock(KissStatus.configAdress, Settings);
}