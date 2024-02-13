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
#include <esp_log.h>
#include <mutex>

#include "WlvlMonitoring.h"
#include "SystemCommanding.h"
#include "LibTime.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#define dForEach_FancyState(gen) \
		gen(StFancyIdle) \
		gen(StFancyStart) \
		gen(StFancyDoneWait) \

#define dGenFancyStateEnum(s) s,
dProcessStateEnum(FancyState);

#define dForEach_PoolState(gen) \
		gen(StPoolIdle) \
		gen(StPoolStart) \
		gen(StPoolDownWait) \

#define dGenPoolStateEnum(s) s,
dProcessStateEnum(PoolState);

#if 0
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#define dGenFancyStateString(s) #s,
dProcessStateStr(FancyState);
#define dGenPoolStateString(s) #s,
dProcessStateStr(PoolState);
#endif

using namespace std;

#define LOG_LVL	0

bool WlvlMonitoring::fancyCreateReq = false;
bool WlvlMonitoring::fancyDrivenByPool = false;
uint32_t WlvlMonitoring::cntFancy = 1;

bool WlvlMonitoring::poolDownReq = false;
bool WlvlMonitoring::poolUpReq = false;

Processing *pDrvs[2];
static mutex mtxDrv;

WlvlMonitoring::WlvlMonitoring()
	: Processing("WlvlMonitoring")
	, mStateFancy(StFancyIdle)
	, mStatePool(StPoolIdle)
	, mStartMs(0)
	, mDiffMs(0)
	, mLastTimeMs(0)
	, mFancyDiffMs(0)
	, mpLed(NULL)
	, mpPool(NULL)
{
	mState = StStart;
}

/* member functions */

Success WlvlMonitoring::process()
{
	uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
	TaskHandle_t pTask;
	UBaseType_t prio;
#if 0
	dStateTrace;
#endif
	mDiffMs = curTimeMs - mLastTimeMs;
	mLastTimeMs = curTimeMs;

	switch (mState)
	{
	case StStart:

		procInfLog("Starting main process");

		mpLed = EspLedPulsing::create();
		if (!mpLed)
			return procErrLog(-1, "could not create process");

		mpLed->pinSet(GPIO_NUM_2);
		mpLed->paramSet(50, 200, 1, 800);
		//mpLed->paramSet(50, 200, 2, 600); // <-- Error condition 2

		mpLed->procTreeDisplaySet(false);
		start(mpLed);

		{
			lock_guard<mutex> lock(mtxDrv);
			pDrvs[0] = NULL;
			pDrvs[1] = NULL;
		}

		pTask = xTaskGetCurrentTaskHandle();
		prio = uxTaskPriorityGet(pTask);

		procDbgLog(LOG_LVL, "Priority of main process is %u", prio);

		for (uint8_t i = 0; i < 2; ++i)
		{
			xTaskCreatePinnedToCore(
				cpuBoundProcess,				// function
				!i ? "Primary" : "Secondary",		// name
				4096,		// stack
				pDrvs + i,	// parameter
				prio,		// priority
				NULL,		// handle
				i);			// core ID
		}

		cmdReg(
			"procAdd",
			&WlvlMonitoring::cmdProcAdd,
			"", "",
			"Add dummy process");

		cmdReg(
			"poolDown",
			&WlvlMonitoring::cmdPoolDown,
			"", "",
			"Shutdown thread pool");

		cmdReg(
			"poolUp",
			&WlvlMonitoring::cmdPoolUp,
			"", "",
			"Start thread pool");

		poolUpReq = true;

		mState = StMain;

		break;
	case StMain:

		fancyProcess();
		poolProcess();

		break;
	default:
		break;
	}

	return Pending;
}

void WlvlMonitoring::fancyProcess()
{
	uint32_t curTimeMs = millis();
	uint32_t diffMs = curTimeMs - mStartMs;
	Success success;
	list<FancyCalculating *>::iterator iter;

	switch (mStateFancy)
	{
	case StFancyIdle:

		if (fancyCreateReq)
		{
			fancyCreateReq = false;

			mStateFancy = StFancyStart;
			break;
		}

		break;
	case StFancyStart:

		procDbgLog(LOG_LVL, "creating fancy process");

		for (uint32_t i = 0; i < cntFancy; ++i)
		{
			FancyCalculating *pFancy;

			pFancy = FancyCalculating::create();
			if (!pFancy)
			{
				procErrLog(-1, "could not create process");
				break;
			}

			pFancy->paramSet(100, 40);
			mLstFancy.push_back(pFancy);

			if (!fancyDrivenByPool)
			{
				start(pFancy);
				continue;
			}

			start(pFancy, DrivenByExternalDriver);
			ThreadPooling::procAdd(pFancy);
		}

		mStartMs = millis();
		mStateFancy = StFancyDoneWait;

		break;
	case StFancyDoneWait:

		mFancyDiffMs = diffMs;

		iter = mLstFancy.begin();
		while (iter != mLstFancy.end())
		{
			FancyCalculating *pFancy = *iter;

			success = pFancy->success();
			if (success == Pending)
				return;

			repel(pFancy);
			pFancy = NULL;

			iter = mLstFancy.erase(iter);
		}

		mStateFancy = StFancyIdle;

		break;
	default:
		break;
	}
}

void WlvlMonitoring::poolProcess()
{
	switch (mStatePool)
	{
	case StPoolIdle:

		if (poolDownReq)
		{
			poolDownReq = false;

			if (!mpPool)
				break;

			cancel(mpPool);

			procDbgLog(LOG_LVL, "waiting for pool to be shut down");

			mStatePool = StPoolDownWait;
			break;
		}

		if (poolUpReq)
		{
			poolUpReq = false;

			if (mpPool)
				break;

			mStatePool = StPoolStart;
			break;
		}

		break;
	case StPoolStart:

		mpPool = ThreadPooling::create();
		if (!mpPool)
		{
			mStatePool = StPoolIdle;
			procErrLog(-1, "could not create process");
			return;
		}
#if 1
		mpPool->workerCntSet(2);
		mpPool->driverCreateFctSet(poolDriverSet);
#else
		mpPool->workerCntSet(1);
#endif
		start(mpPool);

		mStatePool = StPoolIdle;

		break;
	case StPoolDownWait:

		if (!mpPool->shutdownDone())
			break;

		procDbgLog(LOG_LVL, "pool shutdown finished");

		repel(mpPool);
		mpPool = NULL;

		mStatePool = StPoolIdle;

		break;
	default:
		break;
	}

}

/*
 * Literature
 * - https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html
 */
void WlvlMonitoring::poolDriverSet(Processing *pDrv, uint16_t idDrv)
{
	lock_guard<mutex> lock(mtxDrv);
	pDrvs[idDrv] = pDrv;
}

void WlvlMonitoring::cpuBoundProcess(void *arg)
{
	Processing *pDrv;
	uint32_t id = arg == pDrvs ? 0 : 1;

	dbgLog(LOG_LVL, "entered CPU%lu bound process", id);

	while (1)
	{
		{
			lock_guard<mutex> lock(mtxDrv);
			pDrv = pDrvs[id];
		}

		if (!pDrv)
		{
			this_thread::sleep_for(chrono::milliseconds(2));
			continue;
		}

		dbgLog(LOG_LVL, "driver for CPU%lu set", id);

		while (1)
		{
			pDrv->treeTick();
			this_thread::sleep_for(chrono::milliseconds(2));

			if (pDrv->progress())
				continue;

			undrivenSet(pDrv);
			break;
		}

		{
			lock_guard<mutex> lock(mtxDrv);
			pDrvs[id] = NULL;
		}

		dbgLog(LOG_LVL, "driver for CPU%lu unset", id);
	}

	wrnLog("finished CPU%lu bound process", id);
}

void WlvlMonitoring::cmdProcAdd(char *pArgs, char *pBuf, char *pBufEnd)
{
	cntFancy = 1;
	fancyDrivenByPool = false;

	if (*pArgs)
		cntFancy = strtol(pArgs, NULL, 10);

	if (cntFancy > 20)
	{
		infLog("max 20 tasks allowed");
		cntFancy = 20;
	}

	pArgs = strchr(pArgs, ' ');
	if (pArgs)
		fancyDrivenByPool = strtol(pArgs, NULL, 10);

	dbgLog(LOG_LVL, "requesting fancy process");
	fancyCreateReq = true;

	dInfo("Count: %lu, Driven by %s\n", cntFancy, fancyDrivenByPool ? "thread pool" : "parent");
}

void WlvlMonitoring::cmdPoolDown(char *pArgs, char *pBuf, char *pBufEnd)
{
	dbgLog(LOG_LVL, "requesting thread pool shutdown");
	poolDownReq = true;

	dInfo("thread pool shutdown requested");
}

void WlvlMonitoring::cmdPoolUp(char *pArgs, char *pBuf, char *pBufEnd)
{
	dbgLog(LOG_LVL, "requesting thread pool start");
	poolUpReq = true;

	dInfo("thread pool start requested");
}

void WlvlMonitoring::processInfo(char *pBuf, char *pBufEnd)
{
#if 0
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
	dInfo("State Fancy\t\t\t%s\n", FancyStateString[mStateFancy]);
	dInfo("State Pool\t\t\t%s\n", PoolStateString[mStatePool]);
#endif
	dInfo("Loop duration\t\t%lums\n", mDiffMs);
	dInfo("Fancy duration\t\t%lums\n", mFancyDiffMs);
}

/* static functions */

