
idf_component_register(
	SRCS
		"main.cpp"
		"EspSupervising.cpp"
		"InfoTesting.cpp"
	INCLUDE_DIRS "."
)

if(DEFINED ENV{ESP_WIFI_SSID})
    message(STATUS "WiFi SSID: \"$ENV{ESP_WIFI_SSID}\"")
    add_compile_definitions(CONFIG_INT_ESP_WIFI_SSID=\"$ENV{ESP_WIFI_SSID}\")
endif()

if(DEFINED ENV{ESP_WIFI_PASSWORD})
    add_compile_definitions(CONFIG_INT_ESP_WIFI_PASSWORD=\"$ENV{ESP_WIFI_PASSWORD}\")
endif()

