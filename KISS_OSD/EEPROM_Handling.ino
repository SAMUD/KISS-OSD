
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

	EEPROMsave();

}

void EEPROMsave()
{
	EEPROM.updateBlock(configAdress, Settings);
}