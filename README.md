[![Standard](https://img.shields.io/badge/standard-C%2B%2B11-blue.svg?style=plastic&logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
![GitHub](https://img.shields.io/github/license/NoOrientationProgramming/hello-world-stm32?style=plastic&color=blue)

## ESP32 Hello World

...

## How to use

### Clone repo with
```
git clone https://github.com/NoOrientationProgramming/hello-world-esp32.git --recursive
```

Enter directory
```
cd hello-world-esp32/
```

### Set WiFi variables

On Windows
```
set ESP_WIFI_SSID=MySSID
set ESP_WIFI_PASSWORD=MyPassword
```

On Linux
```
export ESP_WIFI_SSID=MySSID
export ESP_WIFI_PASSWORD=MyPassword
```

### Load toolchain

Must be installed already, see [ESP-IDF Setup](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)

```
. ../esp-idf/export.sh
```

### Build project
```
idf.py build
```

### Flash to device
```
idf.py flash
```

## Connect to debugging channels

For windows: Use putty as telnet client

### Process Tree Viewer
```
telnet <ip> 3000
```

### Process Log
```
telnet <ip> 3002
```

### Command Interface
```
telnet <ip> 3004
```

