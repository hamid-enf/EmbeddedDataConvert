# Integration Guide for STM32 and ESP32

---

## 1. STM32CubeIDE Integration

1. Copy `include/` and `src/` to your project under `Middlewares/Third_Party/EDC/`.
2. Right click project -> **Properties** -> **C/C++ Build** -> **Settings** -> **MCU GCC Compiler** -> **Include paths**.
3. Add the include path:
   ```text
   "${workspace_loc:/${ProjName}/Middlewares/Third_Party/EDC/include}"
   ```
4. Include `#include "edc.h"` in your application code and compile.

---

## 2. Keil MDK (uVision) Integration

1. Copy files to your project workspace.
2. In Project Tree, create an `EDC` group and add all `src/*.c` files.
3. Open **Options for Target** -> **C/C++** -> **Include Paths** and add the `include/` folder.
4. Select standard **C99** under language settings.

---

## 3. ESP-IDF (ESP32) Integration

Place EDC into `components/edc/` with this `CMakeLists.txt`:
```cmake
idf_component_register(
    SRCS "src/edc_common.c"
         "src/edc_numeric.c"
         "src/edc_string.c"
         "src/edc_unicode.c"
         "src/edc_persian.c"
         "src/edc_gps.c"
         "src/edc_units.c"
         "src/edc_parser.c"
         "src/edc_buffer.c"
    INCLUDE_DIRS "include"
)
```
Build with `idf.py build`.

---

## 4. PlatformIO Integration

In `platformio.ini`:
```ini
[env:stm32f407]
platform = ststm32
board = discovery_f407vg
framework = stm32cube
build_flags =
    -std=c99
    -Iinclude

[env:esp32]
platform = espressif32
board = esp32dev
framework = espidf
build_flags =
    -std=c99
    -Iinclude
```
