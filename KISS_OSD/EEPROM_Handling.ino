
void EEPROMinit()
{
	EEPROM.setMemPool(32, EEPROMSizeATmega328);
	configAdress = EEPROM.getAddress(sizeof(StoreStruct));
	memValid = EEPROMloadConfig();

	if (!memValid)
	{
		OSD.blink();
		OSDmakegrey();
		OSD.setCursor(3, 6);
		OSD.print(F("WARNING: MEMORY INVALID"));
		OSD.setCursor(3, 7);
		OSD.print(F("LOADING DEFAULT CONFIG"));
		EEPROMsaveDefaultValues();
		OSD.noBlink();
		OSD.videoBackground();
		delay(6000);
		OSD.clear();
		while (OSD.clearIsBusy()) {}

	}
}

bool EEPROMloadConfig()
{
	EEPROM.readBlock(configAdress, Settings);
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

	Settings.stockSettings = 0;

	EEPROMsave();

}

void EEPROMsave()
{
	EEPROM.updateBlock(configAdress, Settings);
}