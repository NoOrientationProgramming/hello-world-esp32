
#include <thread>

#include "EspSupervising.h"

#include "esp_log.h"

using namespace std;

extern "C" void app_main()
{
	Processing *pApp = EspSupervising::create();

	levelLogSet(4);

	ESP_LOGI("main_task", "starting main");
	infLog("starting main");

	while (1)
	{
		pApp->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));
	}
}

