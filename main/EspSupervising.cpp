/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 25.05.2025

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

#include "EspSupervising.h"
#include "SystemDebugging.h"
#include "InfoTesting.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StWifiStart) \
		gen(StWifiConnectedWait) \
		gen(StMainStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#ifndef CONFIG_INT_ESP_WIFI_SSID
#define CONFIG_INT_ESP_WIFI_SSID		"ssid"
#define CONFIG_INT_ESP_WIFI_PASSWORD	"password"
#endif

EspSupervising::EspSupervising()
	: Processing("EspSupervising")
	//, mStartMs(0)
	, mpWifi(NULL)
	, mCntCycles(0)
{
	mState = StStart;
}

/* member functions */

Success EspSupervising::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
	bool ok;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mState = StWifiStart;

		break;
	case StWifiStart:

		mpWifi = EspWifiConnecting::create();
		if (!mpWifi)
			return procErrLog(-1, "could not create process");

		mpWifi->ssidSet(CONFIG_INT_ESP_WIFI_SSID);
		mpWifi->passwordSet(CONFIG_INT_ESP_WIFI_PASSWORD);

		start(mpWifi);

		procInfLog("waiting for wifi");

		mState = StWifiConnectedWait;

		break;
	case StWifiConnectedWait:

		if (!EspWifiConnecting::isOk())
			break;

		procInfLog("wifi connected");

		mState = StMainStart;

		break;
	case StMainStart:

		ok = servicesStart();
		if (!ok)
			return procErrLog(-1, "could not start services");

		cmdReg("led",
			cmdLedToggle,
			"", "Toggle LED",
			"LEDs");

		cmdReg("reset",
			cmdReset,
			"", "Reset the ESP32 microcontroller",
			"Testing");

		mState = StMain;

		break;
	case StMain:

		++mCntCycles;

		break;
	default:
		break;
	}

	return Pending;
}

bool EspSupervising::servicesStart()
{
	SystemDebugging *pDbg;
	Processing *pProc;

	pDbg = SystemDebugging::create(this);
	if (!pDbg)
	{
		procWrnLog("could not create process");
		return false;
	}

	pDbg->procTreeDisplaySet(false);
	start(pDbg);

	pProc = InfoTesting::create();
	if (!pProc)
	{
		procWrnLog("could not create process");
		return false;
	}

	start(pProc);

	return true;
}

void EspSupervising::processInfo(char *pBuf, char *pBufEnd)
{
	dInfo("Hello World ESP32!\n");
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	dInfo("Cycles\t\t%lu\n", mCntCycles);
}

/* static functions */

void EspSupervising::cmdLedToggle(char *pArgs, char *pBuf, char *pBufEnd)
{
	dInfo("LED toggled");
	infLog("LED toggled");
}

void EspSupervising::cmdReset(char *pArgs, char *pBuf, char *pBufEnd)
{
	dInfo("ESP32 reset");
}

