/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 16.01.2024

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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "WlvlMonitoring.h"
#include "SystemCommanding.h"
#include "FancyCalculating.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \
		gen(StTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

bool WlvlMonitoring::fancyCreateReq = false;
int32_t WlvlMonitoring::idDriverFancy = 0;

WlvlMonitoring::WlvlMonitoring()
	: Processing("WlvlMonitoring")
	, mStartMs(0)
	, mpLed(NULL)
	, mpPool(NULL)
{
	mState = StStart;
}

/* member functions */

Success WlvlMonitoring::process()
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

		procInfLog("StStart");

		cmdReg(
			"procAdd",
			&WlvlMonitoring::cmdProcAdd,
			"",
			"",
			"Add dummy process");

		mpLed = EspLedPulsing::create();
		if (!mpLed)
			return procErrLog(-1, "could not create process");

		mpLed->pinSet(GPIO_NUM_2);
		mpLed->paramSet(50, 200, 1, 800);
		//mpLed->paramSet(50, 200, 2, 600); // <-- Error condition 2

		mpLed->procTreeDisplaySet(false);
		start(mpLed);

		mpPool = ThreadPooling::create();
		if (!mpPool)
			return procErrLog(-1, "could not create process");
#if 1
		mpPool->workerCntSet(2);
		mpPool->driverCreateFctSet(poolDriverCreate);
#else
		mpPool->workerCntSet(1);
#endif
		start(mpPool);

		mState = StMain;

		break;
	case StMain:

		fancyCalculationsCreate();

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

void WlvlMonitoring::fancyCalculationsCreate()
{
	if (!fancyCreateReq)
		return;
	fancyCreateReq = false;

	procInfLog("creating fancy process");

	FancyCalculating *pFancy;

	pFancy = FancyCalculating::create();
	if (!pFancy)
	{
		procErrLog(-1, "could not create process");
		return;
	}

	start(pFancy, DrivenByExternalDriver);
	whenFinishedRepel(pFancy);

	ThreadPooling::procAdd(pFancy, idDriverFancy);
}

/*
 * Literature
 * - https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html
 */
void WlvlMonitoring::poolDriverCreate(Processing *pProc, uint16_t idProc)
{
	const char *pName = !idProc ? "Primary" : "Secondary";

	xTaskCreatePinnedToCore(
		(TaskFunction_t)poolWorkerDrive,
		pName,
		4096,
		pProc,
		1,
		NULL,
		idProc);
}

void WlvlMonitoring::poolWorkerDrive(Processing *pChild)
{
	infLog("entered pool worker driver: %p", pChild);

	while (1)
	{
		pChild->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));

		if (pChild->progress())
			continue;

		undrivenSet(pChild);
		break;
	}
}

void WlvlMonitoring::cmdProcAdd(char *pArgs, char *pBuf, char *pBufEnd)
{
	if (pArgs and pArgs[0])
		idDriverFancy = strtol(pArgs, NULL, 10);
	else
		idDriverFancy = -1;

	infLog("requesting fancy process");
	fancyCreateReq = true;

	dInfo("requested fancy process on driver %ld\n", idDriverFancy);
}

void WlvlMonitoring::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

