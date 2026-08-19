# EDC Step-by-Step Tutorials (11 Complete Tutorials)

---

## Tutorial 1: Integer Conversion

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_01(void) {
    char buf[32];
    int64_t val = -12345;
    EDC_ToDecimal(val, buf, sizeof(buf));
    printf("Formatted: %s\n", buf); // "-12345"

    uint32_t parsed_baud = 0;
    EDC_StringToU32("115200", 0, 10, &parsed_baud, NULL);
    printf("Parsed Baud: %u\n", parsed_baud); // 115200
}
```

---

## Tutorial 2: Floating Point Conversion

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_02(void) {
    char buf[32];
    float temp = 25.467f;
    EDC_Float_ToString(temp, buf, sizeof(buf), 2, NULL);
    printf("Temperature: %s C\n", buf); // "25.47 C"

    double parsed_val = 0.0;
    EDC_StringToDouble("1.234e-03", 0, &parsed_val, NULL);
    printf("Parsed: %f\n", parsed_val); // 0.001234
}
```

---

## Tutorial 3: Hexadecimal & Binary

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_03(void) {
    char hex[64];
    uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF};
    EDC_BytesToHex(mac, sizeof(mac), hex, sizeof(hex), true, true, NULL);
    printf("Hex: %s\n", hex); // "DE AD BE EF"
}
```

---

## Tutorial 4: Unicode & UTF-8

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_04(void) {
    const uint8_t text[] = "Hello دنیا 😀";
    size_t err_off = 0;
    if (EDC_UTF8_Validate(text, sizeof(text) - 1, &err_off) == EDC_OK) {
        printf("Valid UTF-8!\n");
    }
}
```

---

## Tutorial 5: Persian Digits

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_05(void) {
    char buf[64];
    EDC_EnglishDigitsToPersian("123.45", 0, buf, sizeof(buf), NULL);
    printf("OLED: %s\n", buf); // "۱۲۳٫۴۵"

    char inplace[] = "Speed: ۱۲۰ km/h";
    size_t len = sizeof(inplace);
    EDC_PersianDigitsToEnglish_InPlace(inplace, &len);
    printf("%s\n", inplace); // "Speed: 120 km/h"
}
```

---

## Tutorial 6: Persian & Arabic Normalization

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_06(void) {
    char text[] = "شركت بازرگاني مَجْمُوعَةٌ";
    size_t len = sizeof(text);
    EDC_Persian_Normalize_InPlace(text, &len, EDC_NORM_DEFAULT_PERSIAN);
    printf("Normalized: %s\n", text); // "شرکت بازرگانی مجموعه"
}
```

---

## Tutorial 7: Transliteration (Fingilish)

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_07(void) {
    const char *msg = "سلام دنیا";
    char latin[64];
    EDC_Persian_TransliterateToLatin(msg, 0, latin, sizeof(latin), NULL, NULL);
    printf("Latin SMS: %s\n", latin); // "slam dnya"
}
```

---

## Tutorial 8: GPS NMEA Parsing

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_08(void) {
    const char *gga = "$GPGGA,123519,3541.4000,N,05125.2000,E,1,08,0.9,1200.5,M,0.0,M,,*4A";
    EDC_GPS_Sentence gps;
    if (EDC_GPS_ParseNMEA(gga, 0, &gps) == EDC_OK) {
        printf("Lat: %.6f, Lon: %.6f, Alt: %.1f m\n", gps.latitude, gps.longitude, gps.altitude_m);
    }
}
```

---

## Tutorial 9: UART Streaming

```c
#include "edc.h"
#include <stdio.h>

void Tutorial_09(void) {
    char buf[128];
    EDC_StreamParser sp;
    EDC_StreamParser_Init(&sp, buf, sizeof(buf), '$', '\n');

    const char *stream = "noise$DATA=100\nmore";
    const char *frame = NULL;
    size_t flen = 0;

    for (size_t i = 0; stream[i] != '\0'; i++) {
        if (EDC_StreamParser_Feed(&sp, (uint8_t)stream[i], &frame, &flen) == EDC_OK) {
            printf("Frame: %s\n", frame);
        }
    }
}
```

---

## Tutorial 10: STM32 Integration

```c
#include "edc.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;

void SendTelemetry(float temp, float volt) {
    char t_str[16], v_str[16], tx[64];
    EDC_Float_ToString(temp, t_str, sizeof(t_str), 1, NULL);
    EDC_Float_ToString(volt, v_str, sizeof(v_str), 2, NULL);
    int len = snprintf(tx, sizeof(tx), "T:%s,V:%s\r\n", t_str, v_str);
    HAL_UART_Transmit(&huart2, (uint8_t*)tx, len, 100);
}
```

---

## Tutorial 11: ESP32 Integration

```c
#include "edc.h"
#include "esp_log.h"

void ParseMQTT(const char *payload) {
    EDC_KVPair pairs[4];
    size_t count = 0;
    if (EDC_KV_Parse(payload, 0, ',', '=', pairs, 4, &count) == EDC_OK) {
        const char *val = NULL;
        size_t vlen = 0;
        if (EDC_KV_Find(pairs, count, "TEMP", 4, &val, &vlen) == EDC_OK) {
            float t = 0.0f;
            EDC_StringToFloat(val, vlen, &t, NULL);
            ESP_LOGI("MQTT", "Parsed Temp: %.2f", t);
        }
    }
}
```
