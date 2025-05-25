/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 28.04.2025

  Copyright (C) 2025, Johannes Natter

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

#include "InfoTesting.h"
#include "SystemDebugging.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

const uint32_t cItemsMax = 400000;

bool logTestReq = false;
char buf[32];

InfoTesting::InfoTesting()
	: Processing("InfoTesting")
	//, mStartMs(0)
	, mItems(0)
{
	mState = StStart;
}

/* member functions */

Success InfoTesting::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		cmdReg("disp",
			cmdDisplay,
			"di", "Display UTF-8 in process tree",
			"Testing");

		cmdReg("你好",
			cmdDisplay,
			"", "Display UTF-8 in process tree",
			"Testing");

		cmdReg("logTest",
			cmdLogTest,
			"", "Test full-/halfduplex logging",
			"Testing");

		buf[0] = 0;

		mState = StMain;

		break;
	case StMain:

		mItems += 1;
		if (mItems > cItemsMax)
			mItems = 0;

		if (!logTestReq)
			break;
		logTestReq = false;

		procInfLog("Test: Info process");

		break;
	default:
		break;
	}

	return Pending;
}

void InfoTesting::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t%s\n", ProcStateString[mState]);
#endif
	dInfo("Display\t\t'%s'\n", buf);
	dInfo("Items processed\n");
	pBuf += progressStr(pBuf, pBufEnd, mItems, cItemsMax);
	dInfo("\n");
}

/* static functions */

void InfoTesting::cmdDisplay(char *pArgs, char *pBuf, char *pBufEnd)
{
	dInfo("Writing '%s' to buffer", pArgs);
	snprintf(buf, sizeof(buf), "%s", pArgs);
}

void InfoTesting::cmdLogTest(char *pArgs, char *pBuf, char *pBufEnd)
{
	errLog(-1, "Test: Error");
	wrnLog("Test: Warning");
	infLog("Test: Info");
	dbgLog("Test: Debug");

	logTestReq = true;

	dInfo("Log testing done");
}

