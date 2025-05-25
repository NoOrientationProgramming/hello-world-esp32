
//#include <thread>

//#include "EspSupervising.h"

#include "esp_log.h"

using namespace std;

extern "C" void app_main()
{
	ESP_LOGI("main_task", "Starting main");

	//Processing *pApp = EspSupervising::create();

	//levelLogSet(4);

	while (1)
	{
		//pApp->treeTick();
		//this_thread::sleep_for(chrono::milliseconds(2));
	}
}

