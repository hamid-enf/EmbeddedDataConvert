# راهنمای نصب و راه‌اندازی در محیط‌های STM32 و ESP32

این راهنما مراحل گام‌به‌گام اضافه کردن کتابخانه **EmbeddedDataConvert (EDC)** به انواع IDEها و زنجیره‌های ابزار (Toolchains) رایج را آموزش می‌دهد.

---

## ۱. نصب در STM32CubeIDE

1. پوشه‌های `include/` و `src/` کتابخانه را در ساختار پروژه خود (مثلاً زیرشاخه `Middlewares/Third_Party/EDC/` یا `Core/`) کپی کنید.
2. در STM32CubeIDE، روی نام پروژه راست‌کلیک کرده و گزینه **Properties** را انتخاب کنید.
3. به مسیر **C/C++ Build -> Settings -> Tool Settings -> MCU GCC Compiler -> Include paths** بروید.
4. مسیر پوشه `include` کتابخانه را اضافه کنید:
   ```text
   "${workspace_loc:/${ProjName}/Middlewares/Third_Party/EDC/include}"
   ```
5. پروژه را Build کنید.

---

## ۲. نصب در Keil MDK (uVision)

1. فایل‌های پروژه را در پوشه پروژه کپی کنید.
2. در پنجره **Project Explorer**، یک گروه جدید به نام `EDC` ایجاد کنید و فایل‌های `src/*.c` را به آن بیفزایید.
3. به پنجره **Options for Target -> C/C++ -> Include Paths** بروید و مسیر پوشه `include` را اضافه کنید.
4. در صورت استفاده از کامپایلر Arm Compiler 6 (AC6)، استاندارد C99 را فعال کنید.

---

## ۳. نصب در ESP-IDF (ESP32)

1. پوشه کتابخانه را در دایرکتوری `components/edc` در ریشه پروژه ESP-IDF خود قرار دهید.
2. یک فایل `components/edc/CMakeLists.txt` با محتوای زیر بسازید:
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
3. در کدهای خود هدر `#include "edc.h"` را اضافه کرده و با دستور `idf.py build` پروژه را بسازید.

---

## ۴. نصب در PlatformIO (STM32 & ESP32)

فایل `library.json` از پیش در ریشه پروژه قرار داده شده است. می‌توانید کتابخانه را در پوشه `lib/EDC` کپی کنید یا در فایل `platformio.ini` آدرس آن را ارجاع دهید:

```ini
[env:stm32f407]
platform = ststm32
board = discovery_f407vg
framework = stm32cube
lib_deps =
    EDC=file://.
```

---

## ۵. نصب در پروژه‌های مبتنی بر CMake

در فایل `CMakeLists.txt` ریشه پروژه خود اضافه کنید:

```cmake
add_subdirectory(path/to/EmbeddedDataConvert)
target_link_libraries(my_firmware_target PRIVATE edc)
```

---

## ۶. نصب در پروژه‌های مبتنی بر Makefile ساده

در Makefile خود متغیرهای کامپایلر را تنظیم کنید:

```makefile
CFLAGS += -Ipath/to/EmbeddedDataConvert/include
C_SOURCES += path/to/EmbeddedDataConvert/src/edc_common.c \
             path/to/EmbeddedDataConvert/src/edc_numeric.c \
             path/to/EmbeddedDataConvert/src/edc_string.c \
             path/to/EmbeddedDataConvert/src/edc_unicode.c \
             path/to/EmbeddedDataConvert/src/edc_persian.c \
             path/to/EmbeddedDataConvert/src/edc_gps.c \
             path/to/EmbeddedDataConvert/src/edc_units.c \
             path/to/EmbeddedDataConvert/src/edc_parser.c \
             path/to/EmbeddedDataConvert/src/edc_buffer.c
```
