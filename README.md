
## Clone repository
```
git clone https://github.com/NoOrientationProgramming/esp32-hello-world.git
```

## Enter directory
```
cd esp32-hello-world/
```

## Initialize git submodules
```
git submodule update --init --recursive
```

## Load toolchain

Must be installed already, see [ESP-IDF Setup](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)

```
. ../esp-idf/export.sh
```

## Remove internal header include

Delete this line in WlvlSupervising.cpp:
```
#include "WifiConnectingInt.h" // <-- Delete this line
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

### Process Tree Viewer
```
nc <ip> 3000
```

### Process Log
```
nc <ip> 3001
```

### Command Interface
```
nc <ip> 3002
```

