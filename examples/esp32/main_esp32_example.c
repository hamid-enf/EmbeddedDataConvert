/**
 * @file main_esp32_example.c
 * @brief Complete ESP32 / ESP-IDF / FreeRTOS IoT example using EDC library.
 *
 * Demonstrates:
 * 1. MQTT / HTTP Key-Value Telemetry Packet Parsing (Zero-Copy Slices).
 * 2. Physical Unit Conversions for IoT Sensor Hub (Celsius <-> Fahrenheit, Pa <-> Bar).
 * 3. Persian Transliteration for GSM SMS Text Notifications.
 *
 * @author EmbeddedDataConvert Team
 */

#include "edc.h"
#include <stdio.h>
#include <string.h>

static void ESP32_Task_MQTTParsing(void) {
    printf("\n--- 1. ESP32 MQTT / HTTP Key-Value Payload Parsing ---\n");

    const char *payload = "DEVICE_ID=\"ESP32_NODE_01\",TEMP=28.75,HUM=55.2,PRESSURE=101325,PUMP=\"ON\"";
    printf("[ESP32_MQTT_RX] Payload: %s\n", payload);

    EDC_KVPair pairs[8];
    size_t count = 0;
    if (EDC_KV_Parse(payload, 0, ',', '=', pairs, 8, &count) == EDC_OK) {
        const char *val = NULL;
        size_t val_len = 0;

        /* Extract Temperature */
        if (EDC_KV_Find(pairs, count, "TEMP", 4, &val, &val_len) == EDC_OK) {
            float temp_c = 0.0f;
            if (EDC_StringToFloat(val, val_len, &temp_c, NULL) == EDC_OK) {
                double temp_f = EDC_Unit_CelsiusToFahrenheit((double)temp_c);
                char temp_f_str[16];
                if (EDC_Double_ToString(temp_f, temp_f_str, sizeof(temp_f_str), 2, NULL) == EDC_OK) {
                    printf(" -> Parsed TEMP: %.2f °C (Converted: %s °F)\n", temp_c, temp_f_str);
                }
            }
        }

        /* Extract Pressure and Convert to Bar */
        if (EDC_KV_Find(pairs, count, "PRESSURE", 8, &val, &val_len) == EDC_OK) {
            double pa = 0.0;
            if (EDC_StringToDouble(val, val_len, &pa, NULL) == EDC_OK) {
                double bar = EDC_Unit_PaToBar(pa);
                printf(" -> Parsed PRESSURE: %.0f Pa (Converted: %.3f bar)\n", pa, bar);
            }
        }

        /* Extract Pump State */
        if (EDC_KV_Find(pairs, count, "PUMP", 4, &val, &val_len) == EDC_OK) {
            bool pump_state = false;
            if (EDC_StringToBool(val, val_len, &pump_state, NULL) == EDC_OK) {
                printf(" -> Parsed PUMP State: %s (Boolean: %d)\n", pump_state ? "ACTIVE" : "INACTIVE", pump_state);
            }
        }
    }
}

static void ESP32_Task_SMSNotification(void) {
    printf("\n--- 2. ESP32 Persian SMS / Transliteration for GSM Modem ---\n");

    const char *alert_persian = "هشدار دما بالا رفت";
    char alert_latin[128];

    EDC_TranslitOptions opts;
    EDC_Translit_GetDefaultOptions(&opts);
    opts.uppercase_first = true;

    if (EDC_Persian_TransliterateToLatin(alert_persian, 0, alert_latin, sizeof(alert_latin), &opts, NULL) == EDC_OK) {
        printf(" -> Original Persian Alert : %s\n", alert_persian);
        printf(" -> GSM 7-bit SMS (Fingilish): %s\n", alert_latin);
    }
}

int main(void) {
    printf("=========================================================\n");
    printf(" ESP32 EDC (EmbeddedDataConvert) Production Example Run\n");
    printf("=========================================================\n");

    ESP32_Task_MQTTParsing();
    ESP32_Task_SMSNotification();

    printf("\n=========================================================\n");
    printf(" ESP32 Example Completed Successfully.\n");
    printf("=========================================================\n\n");
    return 0;
}
