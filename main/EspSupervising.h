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

#ifndef ESP_SUPERVISING_H
#define ESP_SUPERVISING_H

#include "Processing.h"
#include "EspWifiConnecting.h"

class EspSupervising : public Processing
{

public:

	static EspSupervising *create()
	{
		return new dNoThrow EspSupervising;
	}

protected:

	virtual ~EspSupervising() {}

private:

	EspSupervising();
	EspSupervising(const EspSupervising &) = delete;
	EspSupervising &operator=(const EspSupervising &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	bool servicesStart();
	void processInfo(char *pBuf, char *pBufEnd);

	/* member variables */
	//uint32_t mStartMs;
	EspWifiConnecting *mpWifi;
	uint32_t mCntCycles;

	/* static functions */
	static void cmdLedToggle(char *pArgs, char *pBuf, char *pBufEnd);
	static void cmdReset(char *pArgs, char *pBuf, char *pBufEnd);

	/* static variables */

	/* constants */

};

#endif

