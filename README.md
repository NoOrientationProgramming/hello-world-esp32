
## Clone repository
```
git clone https://github.com/NoOrientationProgramming/hello-world-esp32.git --recursive
```

## Enter directory
```
cd hello-world-esp32/
```

## Set WiFi variables

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

## Load toolchain

Must be installed already, see [ESP-IDF Setup](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)

```
. ../esp-idf/export.sh
```

## Build project
```
idf.py build
```

## Flash ESP
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
telnet <ip> 3001
```

### Command Interface
```
telnet <ip> 3002
```

