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

#include <esp_err.h>
#include <nvs_flash.h>

#include "EspSupervising.h"
#include "InfoTesting.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StNvsInit) \
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
	, mpDbg(NULL)
	, mCntCycles(0)
{
	mState = StStart;
}

/* member functions */

/*
 * Literature
 * - https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/storage/nvs_flash.html
 */
Success EspSupervising::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
	esp_err_t res;
	bool ok;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mState = StNvsInit;

		break;
	case StNvsInit:

		res = nvs_flash_init();
		if (res == ESP_ERR_NVS_NO_FREE_PAGES || res == ESP_ERR_NVS_NEW_VERSION_FOUND)
		{
			res = nvs_flash_erase();
			if (res != ESP_OK)
				return procErrLog(-1, "could not erase NVS: %s (0x%04x)",
									esp_err_to_name(res), res);

			res = nvs_flash_init();
		}

		if (res != ESP_OK)
			return procErrLog(-1, "could not init NVS: %s (0x%04x)",
								esp_err_to_name(res), res);

		mState = StWifiStart;

		break;
	case StWifiStart:

		mpWifi = EspWifiConnecting::create();
		if (!mpWifi)
			return procErrLog(-1, "could not create process");

		mpWifi->ssidSet(CONFIG_INT_ESP_WIFI_SSID);
		mpWifi->passwordSet(CONFIG_INT_ESP_WIFI_PASSWORD);

		start(mpWifi);

		procInfLog("waiting for WiFi");

		mState = StWifiConnectedWait;

		break;
	case StWifiConnectedWait:

		if (!EspWifiConnecting::isOk())
			break;

		procInfLog("WiFi connected");

		mState = StMainStart;

		break;
	case StMainStart:

		ok = servicesStart();
		if (!ok)
			procWrnLog("could not start services");

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
	mpDbg = SystemDebugging::create(this);
	if (!mpDbg)
	{
		procWrnLog("could not create process");
		return false;
	}

	mpDbg->procTreeDisplaySet(false);
	start(mpDbg);

	Processing *pProc;

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

