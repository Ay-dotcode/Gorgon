#include "../Time.h"
#include <Windows.h>

namespace Gorgon { namespace Time {

	unsigned long GetTime() {
		return timeGetTime();
	}

	Date GetDate() {
		time_t rawtime;
		struct tm *timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		Date ret;
		ret.Year 	   =(int)timeinfo->tm_year+1900;
		ret.Month	   =Date::MonthType(timeinfo->tm_mon+1);
		ret.Day		   =(int)timeinfo->tm_mday;
		ret.Hour 	   =(int)timeinfo->tm_hour;
		ret.Minute	   =(int)timeinfo->tm_min;
		ret.Second	   =(int)timeinfo->tm_sec;
		ret.Millisecond=0;
		ret.Weekday	   =Date::WeekdayType(timeinfo->tm_wday);
		ret.Timezone   =Date::LocalTimezone();

		return ret;
	}

} }
