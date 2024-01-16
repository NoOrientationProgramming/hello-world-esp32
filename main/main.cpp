
#include "WlvlSupervising.h"

extern "C" void app_main()
{
	Processing *pApp = WlvlSupervising::create();

	levelLogSet(3);

	while (1)
		pApp->treeTick();
}

