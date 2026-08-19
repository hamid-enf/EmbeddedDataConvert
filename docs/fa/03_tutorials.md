# مجموعه ۱۱ آموزش کاربردی کتابخانه EDC (Step-by-Step Tutorials)

---

## آموزش ۱: تبدیل اعداد صحیح (Integer Conversion)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_01(void) {
    char buffer[32];
    int64_t sensor_val = -45230;

    /* تبدیل عدد صحیح به رشته ده‌دهی */
    EDC_ToDecimal(sensor_val, buffer, sizeof(buffer));
    printf("Decimal: %s\n", buffer); // "-45230"

    /* تبدیل رشته متنی به عدد ۳۲ بیتی */
    uint32_t baud_rate = 0;
    size_t consumed = 0;
    if (EDC_StringToU32("  115200 bps", 0, 10, &baud_rate, &consumed) == EDC_OK) {
        printf("Baud: %u (Consumed: %zu chars)\n", baud_rate, consumed); // 115200
    }
}
```

---

## آموزش ۲: تبدیل اعداد اعشاری (Float Conversion)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_02(void) {
    char buffer[32];
    float voltage = 3.2847f;

    /* تبدیل اعشار به رشته با ۳ رقم اعشار */
    EDC_Float_ToString(voltage, buffer, sizeof(buffer), 3, NULL);
    printf("Voltage: %s V\n", buffer); // "3.285 V"

    /* تجزیه رشته علمی به عدد اعشاری double */
    double pressure = 0.0;
    EDC_StringToDouble("1.01325e+05", 0, &pressure, NULL);
    printf("Pressure: %.2f Pa\n", pressure); // 101325.00 Pa
}
```

---

## آموزش ۳: کار با هگزادسیمال و باینری (Hex / Binary)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_03(void) {
    char buf[64];
    uint8_t mac_addr[] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};

    /* تبدیل آرایه بایت به رشته هگز با فاصله */
    EDC_BytesToHex(mac_addr, sizeof(mac_addr), buf, sizeof(buf), true, true, NULL);
    printf("MAC: %s\n", buf); // "00 1A 2B 3C 4D 5E"

    /* تبدیل عدد به باینری */
    EDC_ToBinary(0b10110011, buf, sizeof(buf));
    printf("Binary: %s\n", buf); // "10110011"
}
```

---

## آموزش ۴: کار با یونیکد و UTF-8 (Unicode Subsystem)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_04(void) {
    const uint8_t utf8_text[] = "سلام دنیا 😀";
    size_t invalid_offset = 0;

    /* اعتبارسنجی کامل فرمت UTF-8 */
    if (EDC_UTF8_Validate(utf8_text, sizeof(utf8_text) - 1, &invalid_offset) == EDC_OK) {
        printf("UTF-8 is 100%% valid!\n");
    }

    /* تبدیل UTF-8 به UTF-16 */
    uint16_t utf16_buf[32];
    size_t utf16_len = 0;
    EDC_UTF8_ToUTF16(utf8_text, sizeof(utf8_text) - 1, utf16_buf, 32, false, &utf16_len);
    printf("UTF-16 Word Count: %zu\n", utf16_len);
}
```

---

## آموزش ۵: کار با ارقام فارسی (Persian Digits)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_05(void) {
    char buffer[64];

    /* تبدیل ارقام انگلیسی به فارسی جهت نمایشگر */
    EDC_EnglishDigitsToPersian("123.45", 0, buffer, sizeof(buffer), NULL);
    printf("نمایشگر: %s\n", buffer); // "۱۲۳٫۴۵"

    /* تبدیل ارقام فارسی به انگلیسی درجا (In-Place بدون بافر دوم) */
    char sensor_str[] = "دمای محیط: ۲۸٫۵ درجه";
    size_t len = sizeof(sensor_str);
    EDC_PersianDigitsToEnglish_InPlace(sensor_str, &len);
    printf("%s\n", sensor_str); // "دمای محیط: 28.5 درجه"
}
```

---

## آموزش ۶: نرمال‌سازی متن فارسی و عربی (Normalization)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_06(void) {
    char text[] = "شركت بازرگاني مَجْمُوعَةٌ ســــلام";
    size_t len = sizeof(text);

    /* اصلاح حروف ی و ک عربی، حذف اعراب، حذف کشیدگی و تبدیل ة به ه */
    EDC_Persian_Normalize_InPlace(text, &len, EDC_NORM_DEFAULT_PERSIAN);
    printf("متن استاندارد: %s\n", text); // "شرکت بازرگانی مجموعه سلام"
}
```

---

## آموزش ۷: نویسه‌گردانی فونتیک (Transliteration / Fingilish)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_07(void) {
    const char *persian_msg = "هشدار سرعت بالا";
    char latin_sms[64];

    EDC_TranslitOptions opts;
    EDC_Translit_GetDefaultOptions(&opts);
    opts.uppercase_first = true;

    EDC_Persian_TransliterateToLatin(persian_msg, 0, latin_sms, sizeof(latin_sms), &opts, NULL);
    printf("SMS Text: %s\n", latin_sms); // "Hshdar Srat Bala"
}
```

---

## آموزش ۸: پردازش داده‌های GPS NMEA

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_08(void) {
    const char *nmea = "$GPGGA,123519,3541.4000,N,05125.2000,E,1,08,0.9,1200.5,M,0.0,M,,*4A";
    EDC_GPS_Sentence gps;

    if (EDC_GPS_ParseNMEA(nmea, 0, &gps) == EDC_OK) {
        printf("Latitude  : %.6f\n", gps.latitude);  // 35.690000
        printf("Longitude : %.6f\n", gps.longitude); // 51.420000
        printf("Altitude  : %.1f m\n", gps.altitude_m); // 1200.5 m
    }
}
```

---

## آموزش ۹: پردازش جریانی پورت سریال UART (Streaming Parser)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_09(void) {
    char buf[128];
    EDC_StreamParser sp;
    EDC_StreamParser_Init(&sp, buf, sizeof(buf), '$', '\n');

    /* بایت‌های دریافتی از وقفه UART RX */
    const char *raw_stream = "noise$TEMP=24.5,FAN=ON\nmore_noise";
    const char *frame = NULL;
    size_t frame_len = 0;

    for (size_t i = 0; raw_stream[i] != '\0'; i++) {
        if (EDC_StreamParser_Feed(&sp, (uint8_t)raw_stream[i], &frame, &frame_len) == EDC_OK) {
            printf("Received Frame: %s\n", frame); // "$TEMP=24.5,FAN=ON"
        }
    }
}
```

---

## آموزش ۱۰: یکپارچه‌سازی با STM32 (STM32 HAL Integration)

```c
#include "edc.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void SendSensorTelemetry(float temp, float pressure) {
    char temp_str[16], pres_str[16], tx_buf[64];

    EDC_Float_ToString(temp, temp_str, sizeof(temp_str), 1, NULL);
    EDC_Float_ToString(pressure, pres_str, sizeof(pres_str), 2, NULL);

    int len = snprintf(tx_buf, sizeof(tx_buf), "T:%s,P:%s\r\n", temp_str, pres_str);
    HAL_UART_Transmit(&huart2, (uint8_t*)tx_buf, len, 100);
}
```

---

## آموزش ۱۱: یکپارچه‌سازی با ESP32 و FreeRTOS (ESP32 Integration)

```c
#include "edc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "EDC_ESP32";

void mqtt_payload_task(void *pvParameters) {
    const char *payload = "DEVICE=\"NODE_1\",HUMIDITY=62.4,RELAY=\"ON\"";
    EDC_KVPair pairs[4];
    size_t count = 0;

    if (EDC_KV_Parse(payload, 0, ',', '=', pairs, 4, &count) == EDC_OK) {
        const char *hum_val = NULL;
        size_t hum_len = 0;
        if (EDC_KV_Find(pairs, count, "HUMIDITY", 8, &hum_val, &hum_len) == EDC_OK) {
            float hum = 0.0f;
            EDC_StringToFloat(hum_val, hum_len, &hum, NULL);
            ESP_LOGI(TAG, "Humidity Reading: %.1f %%", hum);
        }
    }
    vTaskDelete(NULL);
}
```
