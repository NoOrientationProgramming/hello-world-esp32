
#include <thread>

#include "WlvlSupervising.h"

using namespace std;

extern "C" void app_main()
{
	Processing *pApp = WlvlSupervising::create();

	levelLogSet(5);

	while (1)
	{
		pApp->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));
	}
}

