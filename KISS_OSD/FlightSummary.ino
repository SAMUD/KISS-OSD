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
		OSD.print(F("MAX CURRENT"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MaxCurrentTotal, TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition-1, 2);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 2);
		OSD.write(SYM_AMP);

		//Average Current
		float TempCurrent = 0;
		TempCurrent = KissTelemetrie.LipoMAH*36.0;
		TempCurrent = TempCurrent / (KissStatus.time/1000);

		OSD.setCursor(1, 3);
		OSD.print(F("AVG"));
		ClearTempCharConverted();
		TempCharPosition = print_int16((int16_t)TempCurrent, TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition-1, 3);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 3);
		OSD.write(SYM_AMP);

		//Voltage
		OSD.setCursor(0, 4);
		OSD.print(F("MIN VOLTAGE"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MinVoltage / KissStatus.BatteryCells, TempCharConverted, 2, 1);
		OSD.setCursor(-TempCharPosition-1, 4);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 4);
		OSD.write(SYM_VOLT);

		//Power
		OSD.setCursor(0, 5);
		OSD.print(F("MAX POWER"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissStats.MAXWatt, TempCharConverted, 1, 1);
		OSD.setCursor(-TempCharPosition-1, 5);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 5);
		OSD.write(SYM_WATT);

		//Consumption
		OSD.setCursor(0, 6);
		OSD.print(F("CONSUMPTION"));
		ClearTempCharConverted();
		TempCharPosition = print_int16(KissTelemetrie.LipoMAH, TempCharConverted, 0, 1);
		OSD.setCursor(-TempCharPosition-1, 6);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 6);
		OSD.write(SYM_MAH);

		//Flight Time
		OSD.setCursor(0, 7);
		OSD.print(F("FLIGHT TIME"));
		ClearTempCharConverted();
		OSD.setCursor(23, 7);
		print_time(KissStatus.time, TempCharConverted);
		OSD.print(TempCharConverted);
		OSD.setCursor(-1, 7);
		OSD.write(SYM_FLY_M);

		//ESC Data
		OSD.setCursor(0, 9);
		OSD.grayBackground();
		OSD.print(F("ESC DATA - MAX VALUES       "));
		OSD.videoBackground();

		for (uint8_t increment = 0; increment < 4; increment++)
		{
			ClearTempCharConverted();
			TempCharPosition = print_int16(KissStats.MAXESCTemp[increment], TempCharConverted, 0, 1);
			TempCharConverted[TempCharPosition++] = SYM_TEMP_C;
			OSD.setCursor(0, 10+ increment);
			OSD.print(TempCharConverted);

			ClearTempCharConverted();
			TempCharPosition = print_int16(KissStats.MAXmotorCurrent[increment], TempCharConverted, 2, 0);
			TempCharConverted[TempCharPosition++] = SYM_AMP;
			OSD.setCursor(7, 10+ increment);
			OSD.print(TempCharConverted);

			ClearTempCharConverted();
			TempCharPosition = print_int16(KissStats.MAXmotorKERPM[increment], TempCharConverted, 1, 1);
			TempCharConverted[TempCharPosition++] = 'K';
			TempCharConverted[TempCharPosition++] = 'R';
			TempCharConverted[TempCharPosition++] = 'P';
			TempCharConverted[TempCharPosition++] = 'M';
			OSD.setCursor(-TempCharPosition, 10+ increment);
			OSD.print(TempCharConverted);
		}

		KissStatus.lastMode = 4;
	}
}

void FlightSummaryCalculate()
{
	//save the maimum value at each iteration
	KissStats.MaxCurrentTotal  = FlightSummaryMax(KissStats.MaxCurrentTotal, KissTelemetrie.current);
	KissStats.MinVoltage = FlightSummaryMin(KissStats.MinVoltage, KissTelemetrie.LipoVoltage);
	KissStats.MAXWatt = FlightSummaryMax(KissStats.MAXWatt, (KissTelemetrie.current*(KissTelemetrie.LipoVoltage / 10))/10);

	for (i = 0; i < 4; i++)
	{
		KissStats.MAXESCTemp[i] = FlightSummaryMax(KissStats.MAXESCTemp[i], KissTelemetrie.ESCTemps[i]);
		KissStats.MAXmotorCurrent[i] = FlightSummaryMax(KissStats.MAXmotorCurrent[i], KissTelemetrie.motorCurrent[i]);
		KissStats.MAXmotorKERPM[i] = FlightSummaryMax(KissStats.MAXmotorKERPM[i], KissTelemetrie.motorKERPM[i]);
	}
}

/*uint16_t FlightSummaryMaxArr(uint16_t maxV, uint16_t *values, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++)
	{
		if (values[i] > maxV)
		{
			maxV = values[i];
		}
	}
	return maxV;
}*/

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