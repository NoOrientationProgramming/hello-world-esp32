/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 02.02.2024

  Copyright (C) 2024-now Authors and www.dsp-crowd.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "FancyCalculating.h"
#include "LibTime.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 0
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

FancyCalculating::FancyCalculating()
	: Processing("FancyCalculating")
	, mStartMs(0)
	, mTimeCalcMs(50)
	, mCntCalc(0)
	, mCntCalcMax(10)
{
	mState = StStart;
}

/* member functions */

void FancyCalculating::paramSet(uint32_t timeCalcMs, uint32_t cntCalcMax)
{
	mTimeCalcMs = timeCalcMs;
	mCntCalcMax = cntCalcMax;
}

Success FancyCalculating::process()
{
	uint32_t curTimeMs = millis();
	uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mState = StMain;

		break;
	case StMain:

		if (mCntCalc >= mCntCalcMax)
			return Positive;
		++mCntCalc;

		mStartMs = millis();

		while (1)
		{
			curTimeMs = millis();
			diffMs = curTimeMs - mStartMs;

			if (diffMs >= mTimeCalcMs)
				break;
		}

		break;
	default:
		break;
	}

	return Pending;
}

void FancyCalculating::processInfo(char *pBuf, char *pBufEnd)
{
#if 0
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	pBuf += progressStr(pBuf, pBufEnd, mCntCalc, mCntCalcMax);
}

/* static functions */

