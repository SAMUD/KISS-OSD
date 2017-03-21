void FlightSummary()
{
	//showed stats:   MaxCurrentTotal,	Min Voltage	
	//				  ESTTemp,			ESCCurrent		ESCRPM
	//				  Time				MAHUsed			Watt
	
	if (KissStatus.lastMode != 4)
	{
		//only do this one time. Ten not needed anymore, because nothing changes
		OSD.clear();
		while (OSD.clearIsBusy());

		OSD.home();
		OSD.grayBackground();
		OSD.print(F("SAMUD OSD - FLIGHT STATISTIC "));
		OSD.videoBackground();

		//Current
		OSD.setCursor(0, 2);
		OSD.print(F("MAX CURRENT:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MaxCurrentTotal, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(-TempCharPosition, 2);
		OSD.print(TempCharConverted);

		//Voltage
		OSD.setCursor(0, 3);
		OSD.print(F("MIN VOLTAGE:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MinVoltage / KissStatus.BatteryCells, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'V';
		OSD.setCursor(-TempCharPosition, 3);
		OSD.print(TempCharConverted);

		//Power
		OSD.setCursor(0, 4);
		OSD.print(F("MAX POWER:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXWatt, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'W';
		OSD.setCursor(-TempCharPosition, 4);
		OSD.print(TempCharConverted);

		//Consumption
		OSD.setCursor(0, 5);
		OSD.print(F("CONSUMPTION:"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.LipoMAH, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = 'M';
		TempCharConverted[TempCharPosition++] = 'A';
		TempCharConverted[TempCharPosition++] = 'H';
		OSD.setCursor(-TempCharPosition, 5);
		OSD.print(TempCharConverted);

		//Flight Time
		OSD.setCursor(0, 6);
		OSD.print(F("FLIGHT TIME:"));
		OSD.setCursor(-2, 5);
		OSD.print(F("00"));
		ClearTempCharConverted();
		OSD.setCursor(24, 6);
		print_time(KissStatus.time, TempCharConverted);
		OSD.print(TempCharConverted);

		//ESC Data
		OSD.setCursor(0, 9);
		OSD.grayBackground();
		OSD.print(F("ESC DATA - MAX VALUES       "));
		OSD.videoBackground();
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXESCTemp, TempCharConverted, 0, 1);
		TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
		OSD.setCursor(0, 10);
		OSD.print(TempCharConverted);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXmotorCurrent, TempCharConverted, 2, 1);
		TempCharConverted[TempCharPosition++] = 'A';
		OSD.setCursor(10, 10);
		OSD.print(TempCharConverted);
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXmotorCurrent, TempCharConverted, 1, 1);
		TempCharConverted[TempCharPosition++] = 'K';
		TempCharConverted[TempCharPosition++] = 'R';
		TempCharConverted[TempCharPosition++] = 'P';
		TempCharConverted[TempCharPosition++] = 'M';
		OSD.setCursor(-TempCharPosition, 10);
		OSD.print(TempCharConverted);

		KissStatus.lastMode = 4;
	}
}

void FlightSummaryCalculate()
{
	
	//TODO Do this instead with if's with calling the fnc
	FlightSummaryCalculateFnc(&KissStats.MaxCurrentTotal, &KissTelemetrie.current, 1);
	if (KissTelemetrie.LipoVoltage>100)
		FlightSummaryCalculateFnc(&KissStats.MinVoltage, &KissTelemetrie.LipoVoltage, 0);
	//FlightSummaryCalculateFnc(&KissStats.MAXESCTemp[0], &KissTelemetrie.LipoVoltage, 0);

	if (KissTelemetrie.current*KissTelemetrie.LipoVoltage > KissStats.MAXWatt)
		KissStats.MAXWatt = KissTelemetrie.current*(KissTelemetrie.LipoVoltage / 10);
	if (KissTelemetrie.ESCTemps[0] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissTelemetrie.ESCTemps[0];
	if (KissTelemetrie.ESCTemps[1] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissTelemetrie.ESCTemps[1];
	if (KissTelemetrie.ESCTemps[2] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissTelemetrie.ESCTemps[2];
	if (KissTelemetrie.ESCTemps[3] > KissStats.MAXESCTemp)
		KissStats.MAXESCTemp = KissTelemetrie.ESCTemps[3];

	if (KissTelemetrie.motorCurrent[0] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissTelemetrie.motorCurrent[0];
	if (KissTelemetrie.motorCurrent[1] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissTelemetrie.motorCurrent[1];
	if (KissTelemetrie.motorCurrent[2] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissTelemetrie.motorCurrent[2];
	if (KissTelemetrie.motorCurrent[3] > KissStats.MAXmotorCurrent)
		KissStats.MAXmotorCurrent = KissTelemetrie.motorCurrent[3];

	if (KissTelemetrie.motorKERPM[0] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissTelemetrie.motorKERPM[0];
	if (KissTelemetrie.motorKERPM[1] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissTelemetrie.motorKERPM[1];
	if (KissTelemetrie.motorKERPM[2] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissTelemetrie.motorKERPM[2];
	if (KissTelemetrie.motorKERPM[3] > KissStats.MAXmotorKERPM)
		KissStats.MAXmotorKERPM = KissTelemetrie.motorKERPM[3];
}

void FlightSummaryCalculateFnc(uint16_t *Stat, uint16_t *Value, uint8_t bigger)
{
	if ((bigger == 1 && *Value > *Stat) || bigger == 0 && *Value < *Stat)
		*Stat = *Value;
}

void FlightSummaryCalculateFnc(int16_t *Stat, int16_t *Value, uint8_t bigger)
{
	if ((bigger == 1 && *Value > *Stat) || bigger == 0 && *Value < *Stat)
		*Stat = *Value;
}
