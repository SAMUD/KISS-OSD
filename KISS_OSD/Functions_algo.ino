/* 1. Information
//=========================================================================================================================

Fonctions needed for various things in the OSD.

//=========================================================================================================================
//START OF CODE
*/

//=====================
// fonction print_int16
uint8_t print_int16(int16_t p_int, char *str, uint8_t dec, uint8_t AlignLeft)
  {
  	uint16_t useVal  = p_int;
  	uint8_t pre      = ' ';
  	if(p_int < 0)
    {
  		useVal = p_int*-1;
  		pre = '-';
  	}
  	uint8_t aciidig[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
  	uint8_t i = 0;
    uint8_t digits[6] = {0,0,0,0,0,0};
  	while(useVal >= 10000){digits[0]++; useVal-=10000;}
  	while(useVal >= 1000){digits[1]++; useVal-=1000;}
  	while(useVal >= 100){digits[2]++; useVal-=100;}
  	while(useVal >= 10){digits[3]++; useVal-=10;}
  	digits[4] = useVal;
    char result[6] = {' ',' ',' ',' ',' ','0'};
  	uint8_t signdone = 0;
  	for(i = 0; i < 6;i++)
    {
  		if(i == 5 && signdone == 0) continue;
  		else if(aciidig[digits[i]] != '0' && signdone == 0)
      {
  			result[i] = pre;
  			signdone = 1;
  		}
      else if(signdone)
        result[i] = aciidig[digits[i-1]];
  	}
          uint8_t CharPos = 0;
          for(i = 0; i < 6;i++)
          {
            if(result[i] != ' ' || (AlignLeft == 0 || (i > 5-dec))) str[CharPos++] = result[i];
            if(dec != 0 && i == 5-dec) str[CharPos++] = '.';
            if(dec != 0 && i > 5-dec && str[CharPos-1] == ' ') str[CharPos-1] = '0';
          }
          return CharPos;
}


//===========
// ESC-Filter
uint32_t ESC_filter(uint32_t oldVal, uint32_t newVal)
{
  return (uint32_t)((uint32_t)((uint32_t)((uint32_t)oldVal*ESC_FILTER)+(uint32_t)newVal))/(ESC_FILTER+1);
}

//========================
//Convert time in a string
void print_time(unsigned long time, char *time_str)
{
    uint16_t seconds = time / 1000;
    uint8_t mills = time % 1000;
    uint8_t minutes = 0;
    if (seconds >= 60)
    {
      minutes = seconds/60;
    }
    else
    {
      minutes = 0;
    }
    seconds = seconds % 60; // reste
    static char time_sec[6];
    static char time_mil[6];
    uint8_t i = 0;
    uint8_t time_pos = print_int16(minutes, time_str,0,1);
    time_str[time_pos++] = 'm';

    uint8_t sec_pos = print_int16(seconds, time_sec,0,1);
    for (i=0; i<sec_pos; i++)
    {
      time_str[time_pos++] = time_sec[i];
    }
    //time_str[time_pos++] = '.';

    //uint8_t mil_pos = print_int16(mills, time_mil,0,1);
    //time_str[time_pos++] = time_mil[0];
    for (i=time_pos; i<9; i++)
    {
      time_str[time_pos++] = ' ';
    }
}
