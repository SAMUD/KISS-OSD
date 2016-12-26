
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
	Settings.CapacityThreshold = DCapacityThreshold;
	Settings.CapacityThreshold2ndStage = DCapacityThreshold2ndStage;
	Settings.ESC_FILTER = DESC_FILTER;
	Settings.hysteresis = Dhysteresis;
	Settings.LowVoltage3s = DLowVoltage3s;
	Settings.LowVoltage4s = DLowVoltage4s;
	Settings.MAGNETPOLECOUNT = DMAGNETPOLECOUNT;
	Settings.marginLastRow = DmarginLastRow;
	Settings.MemoryVersion = DMemoryVersion;
	Settings.MinimalCellVoltage2nd = DMinimalCellVoltage2nd;
	Settings.RED_MODE_AUX_CHANNEL = DRED_MODE_AUX_CHAN;
	Settings.SeparationVoltage3s4s = DSeparationVoltage3s4s;

	EEPROMsave();

}

void EEPROMsave()
{
	EEPROM.updateBlock(configAdress, Settings);
}