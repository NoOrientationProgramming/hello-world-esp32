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

#include "WlvlSupervising.h"
#include "SystemDebugging.h"
#include "WlvlMonitoring.h"

#include "WifiConnectingInt.h" // <-- Delete this line

#ifndef CONFIG_INT_ESP_WIFI_SSID
#define CONFIG_INT_ESP_WIFI_SSID		"ssid"
#define CONFIG_INT_ESP_WIFI_PASSWORD	"password"
#endif

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StWifiStart) \
		gen(StWifiConnectedWait) \
		gen(StMainStart) \
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

WlvlSupervising::WlvlSupervising()
	: Processing("WlvlSupervising")
	, mStartMs(0)
	, mpWifi(NULL)
{
	mState = StStart;
}

/* member functions */

Success WlvlSupervising::process()
{
	Processing *pProc;
	SystemDebugging *pDbg;
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
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

		if (!mpWifi->connected())
			break;

		procInfLog("wifi connected");

		mState = StMainStart;

		break;
	case StMainStart:

		pProc = WlvlMonitoring::create();
		if (!pProc)
			return procErrLog(-1, "could not create process");

		start(pProc);

		pDbg = SystemDebugging::create(this);
		if (!pDbg)
			return procErrLog(-1, "could not create process");

		pDbg->procTreeDisplaySet(false);
		start(pDbg);

		mState = StMain;

		break;
	case StMain:

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

void WlvlSupervising::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

