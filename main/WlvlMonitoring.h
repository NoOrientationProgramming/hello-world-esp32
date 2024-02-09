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

#ifndef WLVL_MONITORING_H
#define WLVL_MONITORING_H

#include <list>

#include "Processing.h"
#include "EspLedPulsing.h"
#include "ThreadPooling.h"
#include "FancyCalculating.h"

class WlvlMonitoring : public Processing
{

public:

	static WlvlMonitoring *create()
	{
		return new (std::nothrow) WlvlMonitoring;
	}

protected:

	WlvlMonitoring();
	virtual ~WlvlMonitoring() {}

private:

	WlvlMonitoring(const WlvlMonitoring &) : Processing("") {}
	WlvlMonitoring &operator=(const WlvlMonitoring &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	/* member variables */
	uint32_t mStartMs;
	uint32_t mFancyDiffMs;
	EspLedPulsing *mpLed;
	ThreadPooling *mpPool;
	std::list<FancyCalculating *> mLstFancy;

	/* static functions */
	static void poolDriverSet(Processing *pDrv, uint16_t idDrv);
	static void cpuBoundProcess(void *arg);

	static void cmdProcAdd(char *pArgs, char *pBuf, char *pBufEnd);
	static void cmdPoolDown(char *pArgs, char *pBuf, char *pBufEnd);
	static void cmdPoolUp(char *pArgs, char *pBuf, char *pBufEnd);

	/* static variables */
	static bool fancyCreateReq;
	static bool fancyDrivenByPool;
	static uint32_t cntFancy;

	static bool poolDownReq;
	static bool poolUpReq;

	/* constants */

};

#endif

