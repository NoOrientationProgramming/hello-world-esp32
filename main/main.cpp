
#include <thread>

#include <esp_log.h>
#include <esp_system.h>

#include "EspSupervising.h"

using namespace std;

void taskSupervising(void *pvParameters)
{
	(void)pvParameters;

	Processing *pApp = EspSupervising::create();

	levelLogSet(4);

	ESP_LOGI("main_task", "starting main");
	infLog("starting main");

	while (1)
	{
		pApp->treeTick();

		vTaskDelay(pdMS_TO_TICKS(10));

		if (pApp->progress())
			continue;

		break;
	}

	esp_restart();
}

/*
 * Literature
 * - https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32s2/api-reference/system/freertos.html
 */
extern "C" void app_main()
{
	xTaskCreate(taskSupervising, "EspSupervising()", 4096, NULL, 5, NULL);
	vTaskDelay(portMAX_DELAY);
}

