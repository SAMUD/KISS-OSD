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
		TempCharPosition = print_int16(KissStats.MAXmotorKERPM, TempCharConverted, 1, 1);
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
	//save the maimum value at each iteration
	KissStats.MaxCurrentTotal  = FlightSummaryMax(KissStats.MaxCurrentTotal, KissTelemetrie.current);
	KissStats.MinVoltage = FlightSummaryMin(KissStats.MinVoltage, KissTelemetrie.LipoVoltage);
	KissStats.MAXWatt = FlightSummaryMax(KissStats.MAXWatt, (KissTelemetrie.current*(KissTelemetrie.LipoVoltage / 10))/10);
	KissStats.MAXESCTemp = FlightSummaryMaxArr(KissStats.MAXESCTemp, KissTelemetrie.ESCTemps, 4);
	KissStats.MAXmotorCurrent = FlightSummaryMaxArr(KissStats.MAXmotorCurrent, KissTelemetrie.motorCurrent, 4);
	KissStats.MAXmotorKERPM = FlightSummaryMaxArr(KissStats.MAXmotorKERPM, KissTelemetrie.motorKERPM, 4);
}

uint16_t FlightSummaryMaxArr(uint16_t maxV, uint16_t *values, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++)
	{
		if (values[i] > maxV)
		{
			maxV = values[i];
		}
	}
	return maxV;
}

int16_t FlightSummaryMax(int16_t maxV, int16_t newVal)
{
	if (newVal > maxV) {
		return newVal;
	}
	return maxV;
}

int16_t FlightSummaryMin(int16_t minV, int16_t newVal)
{
	if (newVal < minV) {
		return newVal;
	}
	return minV;
}