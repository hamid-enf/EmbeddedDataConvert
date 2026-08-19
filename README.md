# EmbeddedDataConvert (EDC)

**کتابخانه فوق‌العاده سبک، مستقل، استاندارد و حرفه‌ای تبدیل، تجزیه (Parsing)، قالب‌بندی (Formatting)، کدگذاری (Encoding) و نرمال‌سازی داده‌ها برای میکروکنترلرهای STM32، ESP32 و سیستم‌های نهفته صنعتی (Embedded Systems).**

[![C99 Standard](https://img.shields.io/badge/C-Standard%20C99%2FC11-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![MISRA-C:2012](https://img.shields.io/badge/MISRA--C%3A2012-Compliant-green.svg)](docs/fa/05_misra_compliance.md)
[![Zero Dynamic Allocation](https://img.shields.io/badge/Memory-Zero%20Dynamic%20Alloc-brightgreen.svg)](docs/fa/01_architecture.md)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

---

## ۱. ویژگی‌های کلیدی

* **بدون هیچ‌گونه وابستگی (Zero Dependencies):** بدون نیاز به STM32 HAL، CMSIS، Arduino یا FreeRTOS.
* **بدون تخصیص پویای حافظه (Zero Dynamic Memory Allocation):** بدون استفاده از `malloc()`، `free()`، `calloc()` یا `realloc()`.
* **مصرف حافظه استاتیک صفر بایت (0 Bytes Static RAM):** فاقد هرگونه متغیر سراسری یا `static mutable`.
* **کاملاً Thread-Safe و Reentrant:** مناسب برای پروژه‌های چندتسکی FreeRTOS و وقفه‌ها (ISR).
* **معماری ماژولار (Modular by Design):** قابلیت فعال/غیرفعال‌سازی ماژول‌ها از طریق `edc_config.h` جهت کاهش Flash تا کمتر از ۲ کیلوبایت.
* **بهینه‌سازی شده برای سرعت و کارایی:** تا ۵ برابر سریع‌تر از توابع استاندارد C مانند `sprintf` و `atof`.
* **پشتیبانی کامل از زبان‌های فارسی و عربی:** تبدیل دوطرفه ارقام (۰-۹ و ٠-٩ به 0-9)، نرمال‌سازی نویسه‌ها (ي، ك، ة، اعراب، کشیدگی، نیم‌فاصله) و نویسه‌گردانی فونتیک (Transliteration / Fingilish).
* **پشتیبانی از فرمت‌های ناوبری GPS / NMEA-0183:** پردازش جملات $GPGGA, $GPRMC, $GPVTG, $GPGLL، بررسی Checksum و پردازش جریانی (Streaming) بایت‌به‌بایت.
* **محاسبات نقطه ثابت (Fixed-Point Q-Format):** تبدیل اعداد اعشاری به Q8, Q15, Q31 و فرمت Generic Qm.n و قالب‌بندی رشته بدون استفاده از FPU.
* **انطباق با استاندارد صنعتی MISRA-C:2012:** کدهای بدون رفتار تعریف‌نشده (Zero UB)، با بافرهای کنترل‌شده و انواع داده صریح `stdint.h`.

---

## ۲. معماری ماژول‌های کتابخانه

```text
EmbeddedDataConvert (EDC)
│
├── Numeric (تبدیل و تجزیه اعداد)
│   ├── تبدیل انواع عدد صحیح (int8_t تا uint64_t)
│   ├── مبناهای ۲ تا ۳۶ (Decimal, Hex, Binary, Octal) با Padding صفر
│   ├── اعداد اعشاری (Float / Double / Scientific Notation) بدون sprintf
│   ├── نقطه ثابت (Fixed-Point Q-Format: Q8, Q15, Q31, Qm.n)
│   └── مقادیر بولی (Boolean: true/false, 1/0, yes/no, on/off)
│
├── String & Hex (رشته و هگزادسیمال ایمن)
│   ├── طول رشته، کپی ایمن با مرزبندی بافر، مقایسه با/بدون حساسیت به حروف
│   ├── برش بدون کپی فضاها (Zero-Copy Trim)
│   └── تبدیل آرایه بایت به رشته هگزادسیمال و برعکس (CAN/Modbus Frames)
│
├── Unicode & Transcoding (سیستم یونیکد استاندارد)
│   ├── اعتبارسنجی کامل RFC 3629 / Unicode Standard (رد Overlong و Surrogates)
│   ├── کدگذاری و کدگشایی کاراکترهای یونیکد (U+0000 تا U+10FFFF)
│   ├── تبدیل‌های دوطرفه UTF-8 ↔ UTF-16 ↔ UTF-32
│   └── دیکودر جریانی بایت‌به‌بایت UTF-8 (State Machine) برای پورت سریال
│
├── Persian & Arabic (ماژول اختصاصی فارسی و عربی)
│   ├── تبدیل ارقام فارسی (۰-۹) و عربی (٠-٩) به انگلیسی (0-9) و بالعکس
│   ├── تبدیل درجا بدون حافظه جانبی (In-Place Conversion)
│   ├── نرمال‌سازی حروف (ي/ی، ك/ک، ة/ه، حذف اعراب، حذف کشیدگی، حذف نیم‌فاصله)
│   └── نویسه‌گردانی فونتیک به الفبای لاتین (Transliteration / Fingilish)
│
├── GPS & NMEA (ناوبری و تله‌متری)
│   ├── تبدیل مختصات NMEA (DDMM.MMMM) به درجه اعشاری و درجه-دقیقه-ثانیه (DMS)
│   ├── پارسر پکت‌های NMEA ($GPGGA, $GPRMC, $GPVTG, $GPGLL) با بررسی XOR Checksum
│   └── دیکودر جریانی بایت‌به‌بایت برای وقفه و DMA واحد UART
│
├── Physical Units (تبدیل واحدهای فیزیکی سنسورها)
│   ├── مسافت و طول: متر، کیلومتر، فوت، اینچ، مایل دریایی
│   ├── سرعت: m/s، km/h، گره (Knot)، mph
│   ├── دما: سلسیوس، فارنهایت، کلوین
│   └── فشار: پاسکال، بار، PSI، هکتوپاسکال، اتمسفر
│
├── Parsers (پارسرهای بدون حافظه پویا)
│   ├── استخراج جفت‌های کلید/مقدار (Key=Value) به شیوه Zero-Copy Slices
│   ├── توکن‌ساز CSV و رشته‌های دارای جداکننده دلخواه
│   └── فریم‌ساز جریانی پکت‌های سریال با مشخص کردن کاراکتر شروع و پایان
│
└── Buffer Utilities (ابزارهای بافر)
    ├── برش‌های بافر (Const / Mutable Slices)
    └── پردازش و تغییر شکل داده‌ها درجا (In-Place Transformers)
```

---

## ۳. راهنمای شروع سریع (Quick Start)

تنها کافی است هدر اصلی کتابخانه را در پروژه خود اضافه کنید:

```c
#include "edc.h"
#include <stdio.h>

int main(void) {
    char buffer[64];

    /* ۱. تبدیل عدد اعشاری سنسور به رشته با ۲ رقم اعشار */
    float temperature = 24.856f;
    EDC_Float_ToString(temperature, buffer, sizeof(buffer), 2, NULL);
    printf("Temp: %s °C\n", buffer); // "24.86 °C"

    /* ۲. تبدیل ارقام انگلیسی به فارسی جهت نمایش روی LCD/OLED */
    char persian_buf[64];
    EDC_EnglishDigitsToPersian(buffer, 0, persian_buf, sizeof(persian_buf), NULL);
    printf("دما: %s درجه\n", persian_buf); // "دما: ۲۴٫۸۶ درجه"

    /* ۳. تبدیل ارقام فارسی به انگلیسی درجا (بدون بافر اضافه) */
    char inplace_text[] = "سرعت: ۱۲۰٫۵ کیلومتر";
    size_t len = sizeof(inplace_text);
    EDC_PersianDigitsToEnglish_InPlace(inplace_text, &len);
    printf("Normalized: %s\n", inplace_text); // "سرعت: 120.5 کیلومتر"

    /* ۴. پارس کردن پکت GPS */
    EDC_GPS_Sentence gps;
    const char *nmea = "$GPGGA,123519,3541.4000,N,05125.2000,E,1,08,0.9,1200.5,M,0.0,M,,*4A";
    if (EDC_GPS_ParseNMEA(nmea, 0, &gps) == EDC_OK) {
        printf("Lat: %.6f, Lon: %.6f, Alt: %.1f m\n", gps.latitude, gps.longitude, gps.altitude_m);
    }

    return 0;
}
```

---

## ۴. نتایج بنچ‌مارک و مصرف حافظه

کامپایل شده با فلگ `-Os` روی معماری ARM:

* **Static RAM:** دقیقاً **۰ بایت**
* **Heap RAM:** دقیقاً **۰ بایت**
* **Flash Footprint:** کل ماژول‌ها **~۲۳ کیلوبایت** (ماژول عددی تنها ۷ کیلوبایت)
* **سرعت پردازش:**
  * تبدیل عدد صحیح به رشته: **~58 نانوثانیه** (بیش از ۱۷ میلیون عملیات در ثانیه)
  * تبدیل رشته به اعشار: **~13 نانوثانیه** (بیش از ۷۵ میلیون عملیات در ثانیه)
  * تبدیل نقطه ثابت Q15: **~2.9 نانوثانیه** (بیش از ۳۳۰ میلیون عملیات در ثانیه)
  * پردازش کامل جمله NMEA GPS: **~316 نانوثانیه**

گزارش کامل در [docs/fa/06_memory_and_benchmarks.md](docs/fa/06_memory_and_benchmarks.md) در دسترس است.

---

## ۵. ساختار فایل‌های پروژه

```text
EmbeddedDataConvert/
│
├── include/                  # هدر فایل‌های استاندارد C
│   ├── edc.h                 # هدر مستر جامع
│   ├── edc_common.h          # تعاریف پایه، کدهای خطا و ماکروها
│   ├── edc_config.h          # فلگ‌های فعال‌سازی ماژول‌ها
│   ├── edc_numeric.h         # توابع تبدیل اعداد، Float و Q-Format
│   ├── edc_string.h          # توابع رشته و هگزادسیمال
│   ├── edc_unicode.h         # ساب‌سیستم یونیکد UTF-8/16/32
│   ├── edc_persian.h         # ماژول فارسی، عربی و نویسه‌گردانی
│   ├── edc_gps.h             # پارسر NMEA و محاسبات مختصات GPS
│   ├── edc_units.h           # تبدیل واحدهای فیزیکی
│   ├── edc_parser.h          # پارسرهای Key-Value، CSV و جریان سریال
│   └── edc_buffer.h          # ابزارهای برش و بافرینگ ایمن
│
├── src/                      # سورس‌کدهای زبان C
│   ├── edc_common.c
│   ├── edc_numeric.c
│   ├── edc_string.c
│   ├── edc_unicode.c
│   ├── edc_persian.c
│   ├── edc_gps.c
│   ├── edc_units.c
│   ├── edc_parser.c
│   └── edc_buffer.c
│
├── tests/                    # تست‌های واحد جامع با فریم‌ورک Unity
│   ├── unity/
│   ├── test_edc_numeric.c
│   ├── test_edc_string.c
│   ├── test_edc_unicode.c
│   ├── test_edc_persian.c
│   ├── test_edc_gps.c
│   ├── test_edc_units.c
│   ├── test_edc_parser.c
│   ├── test_edc_buffer.c
│   └── test_main.c
│
├── fuzz/                     # تستر Fuzzing برای اعتبارسنجی داده‌های مخرب
│   └── fuzz_runner.c
│
├── benchmarks/               # ابزار سنجش کارایی و تاخیر
│   ├── benchmark_main.c
│   └── benchmark_results.md
│
├── examples/                 # پروژه‌ها و مثال‌های کاربردی کامل
│   ├── stm32/                # پروژه کاربردی STM32 (ADC, OLED, CAN, GPS)
│   ├── esp32/                # پروژه کاربردی ESP32 (MQTT, IoT, Units, SMS)
│   ├── gps_tracker/          # ردیاب تله‌متری خودرو
│   └── uart_streaming/       # پارسر جریانی خط سریال UART
│
├── docs/fa/                  # مستندات کامل فارسی
│   ├── 01_architecture.md
│   ├── 02_api_reference.md
│   ├── 03_tutorials.md       # ۱۱ آموزش گام‌به‌گام
│   ├── 04_concepts_distinction.md # تفکیک دقیق مفاهیم تبدیل و پردازش
│   ├── 05_misra_compliance.md
│   ├── 06_memory_and_benchmarks.md
│   └── 07_stm32_esp32_integration.md
│
├── CMakeLists.txt            # تنظیمات ساخت CMake
├── Makefile                  # دستورات ساخت و تست سریع
├── library.json              # تنظیمات PlatformIO
└── platformio.ini
```

---

## ۶. نحوه کامپایل و اجرای تست‌ها

```bash
# ساخت کتابخانه استاتیک libedc.a
make lib

# اجرای تست‌های واحد (Unit Tests)
make test

# اجرای تست‌های فازینگ (Fuzz Testing)
make fuzz

# اجرای بنچ‌مارک کارایی
make bench

# اجرای تمام مثال‌های کاربردی (STM32, ESP32, GPS, UART)
make examples
```

---

## ۷. مستندات تکمیلی (فارسی)

1. [معماری داخلی و اصول طراحی](docs/fa/01_architecture.md)
2. [مرجع کامل توابع و پارامترها (API Reference)](docs/fa/02_api_reference.md)
3. [مجموعه ۱۱ آموزش گام‌به‌گام کاربردی](docs/fa/03_tutorials.md)
4. [تفکیک دقیق مفاهیم تبدیل، قالب‌بندی، یونیکد، نرمال‌سازی و نویسه‌گردانی](docs/fa/04_concepts_distinction.md)
5. [گزارش انطباق با استاندارد MISRA-C:2012](docs/fa/05_misra_compliance.md)
6. [گزارش مصرف حافظه، پیچیدگی و بنچ‌مارک](docs/fa/06_memory_and_benchmarks.md)
7. [راهنمای نصب در STM32CubeIDE, Keil, ESP-IDF و PlatformIO](docs/fa/07_stm32_esp32_integration.md)

---

## ۸. مجوز (License)

این پروژه تحت مجوز بین‌المللی **MIT** منتشر شده و استفاده از آن در پروژه‌های تجاری و صنعتی آزاد است.
