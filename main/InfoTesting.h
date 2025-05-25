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

#ifndef INFO_TESTING_H
#define INFO_TESTING_H

#include "Processing.h"

class InfoTesting : public Processing
{

public:

	static InfoTesting *create()
	{
		return new dNoThrow InfoTesting;
	}

protected:

	virtual ~InfoTesting() {}

private:

	InfoTesting();
	InfoTesting(const InfoTesting &) = delete;
	InfoTesting &operator=(const InfoTesting &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	/* member variables */
	//uint32_t mStartMs;
	uint32_t mItems;

	/* static functions */
	static void cmdDisplay(char *pArgs, char *pBuf, char *pBufEnd);
	static void cmdLogTest(char *pArgs, char *pBuf, char *pBufEnd);

	/* static variables */

	/* constants */

};

#endif

