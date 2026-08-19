/**
 * @file main_stm32_example.c
 * @brief Complete, practical STM32 application example demonstrating EDC library integration.
 *
 * Applicable to STM32F1, F4, G4, H7 with STM32 HAL / LL or Bare Metal.
 *
 * Demonstrates:
 * 1. ADC Sensor Voltage & Temperature Reading -> Float & Fixed-Point Q15 conversion.
 * 2. Persian Digit Formatting for Character LCD / OLED Display.
 * 3. Hexadecimal Conversion for RS485 / Modbus / CAN Frames.
 * 4. Streaming UART RX for NMEA GPS Telemetry ($GPGGA / $GPRMC).
 *
 * @author EmbeddedDataConvert Team
 */

#include "edc.h"
#include <stdio.h>
#include <string.h>

/* Mock UART Transmit function simulating HAL_UART_Transmit */
static void Mock_HAL_UART_Transmit(const char *msg) {
    printf("[STM32_UART_TX] %s", msg);
}

/* Simulated ADC Sensor task */
static void STM32_Task_SensorReadings(void) {
    printf("\n--- 1. STM32 ADC Sensor Reading & Formatting ---\n");

    uint16_t adc_raw = 2785;
    float voltage = ((float)adc_raw / 4095.0f) * 3.3f;
    float temperature = (voltage - 0.76f) / 0.0025f + 25.0f;

    char tx_buf[128];
    char temp_str[16];
    char volt_str[16];

    if (EDC_Float_ToString(temperature, temp_str, sizeof(temp_str), 2, NULL) == EDC_OK &&
        EDC_Float_ToString(voltage, volt_str, sizeof(volt_str), 3, NULL) == EDC_OK) {
        snprintf(tx_buf, sizeof(tx_buf), "ADC: %u | Voltage: %s V | Temperature: %s C\r\n",
                 (unsigned int)adc_raw, volt_str, temp_str);
        Mock_HAL_UART_Transmit(tx_buf);
    }

    /* Fixed-Point Q15 conversion for Cortex-M0/M3 without hardware FPU */
    int32_t temp_q15 = 0;
    if (EDC_Float_To_Q15(temperature, false, &temp_q15) == EDC_OK) {
        char q_str[16];
        if (EDC_Q_ToString(temp_q15, 15, q_str, sizeof(q_str), 2, NULL) == EDC_OK) {
            snprintf(tx_buf, sizeof(tx_buf), "Fixed-Point Q15 Temperature: %s C (Raw Q15: %ld)\r\n", q_str, (long)temp_q15);
            Mock_HAL_UART_Transmit(tx_buf);
        }
    }
}

/* Simulated Persian OLED Display Formatting */
static void STM32_Task_PersianDisplay(void) {
    printf("\n--- 2. STM32 Persian OLED / LCD Display Formatting ---\n");

    float speed_kmh = 85.4f;
    char speed_ascii[16];
    if (EDC_Float_ToString(speed_kmh, speed_ascii, sizeof(speed_ascii), 1, NULL) == EDC_OK) {
        char speed_persian[32];
        if (EDC_EnglishDigitsToPersian(speed_ascii, 0, speed_persian, sizeof(speed_persian), NULL) == EDC_OK) {
            char oled_line[128];
            snprintf(oled_line, sizeof(oled_line), "سرعت خودرو: %s کیلومتر بر ساعت\r\n", speed_persian);
            Mock_HAL_UART_Transmit(oled_line);
        }
    }

    /* Arabic to Persian character normalization */
    char user_input_arabic[64] = "اطلاعات ارسالي از دستگاه";
    size_t len = strlen(user_input_arabic);
    if (EDC_Persian_Normalize_InPlace(user_input_arabic, &len, EDC_NORM_DEFAULT_PERSIAN) == EDC_OK) {
        char oled_line[128];
        snprintf(oled_line, sizeof(oled_line), "متن اصلاح‌شده: %s\r\n", user_input_arabic);
        Mock_HAL_UART_Transmit(oled_line);
    }
}

/* Simulated CAN / Modbus Hexadecimal Packet */
static void STM32_Task_HexModbusPacket(void) {
    printf("\n--- 3. STM32 Modbus / CAN Hex Conversion ---\n");

    uint8_t modbus_frame[] = {0x01, 0x03, 0x00, 0x64, 0x00, 0x02, 0x85, 0xD4};
    char hex_str[64];

    if (EDC_BytesToHex(modbus_frame, sizeof(modbus_frame), hex_str, sizeof(hex_str), true, true, NULL) == EDC_OK) {
        char tx_buf[128];
        snprintf(tx_buf, sizeof(tx_buf), "Modbus Request Frame: [ %s ]\r\n", hex_str);
        Mock_HAL_UART_Transmit(tx_buf);
    }
}

/* Simulated UART RX Interrupt with NMEA GPS Parser */
static void STM32_Task_GPSStreamRX(void) {
    printf("\n--- 4. STM32 UART Streaming GPS NMEA Parsing ---\n");

    EDC_GPS_StreamContext gps_stream;
    EDC_GPS_StreamInit(&gps_stream);
    EDC_GPS_Sentence gps_data;

    const char *gps_uart_rx = "$GPGGA,142530,3541.4000,N,05125.2000,E,1,09,1.1,1200.5,M,0.0,M,,*4A\r\n";
    size_t rx_len = strlen(gps_uart_rx);

    for (size_t i = 0; i < rx_len; i++) {
        uint8_t byte = (uint8_t)gps_uart_rx[i];
        EDC_Status st = EDC_GPS_StreamFeed(&gps_stream, byte, &gps_data);
        if (st == EDC_OK) {
            char lat_str[32], lon_str[32], alt_str[16];
            if (EDC_Double_ToString(gps_data.latitude, lat_str, sizeof(lat_str), 6, NULL) == EDC_OK &&
                EDC_Double_ToString(gps_data.longitude, lon_str, sizeof(lon_str), 6, NULL) == EDC_OK &&
                EDC_Float_ToString(gps_data.altitude_m, alt_str, sizeof(alt_str), 1, NULL) == EDC_OK) {

                char report[256];
                snprintf(report, sizeof(report),
                         "GPS FIX Valid! Lat: %s, Lon: %s, Alt: %s m, Sats: %u, Time: %02u:%02u:%02u UTC\r\n",
                         lat_str, lon_str, alt_str, (unsigned int)gps_data.satellites,
                         (unsigned int)gps_data.time.hour, (unsigned int)gps_data.time.minute, (unsigned int)gps_data.time.second);
                Mock_HAL_UART_Transmit(report);
            }
        }
    }
}

int main(void) {
    printf("=========================================================\n");
    printf(" STM32 EDC (EmbeddedDataConvert) Production Example Run\n");
    printf("=========================================================\n");

    STM32_Task_SensorReadings();
    STM32_Task_PersianDisplay();
    STM32_Task_HexModbusPacket();
    STM32_Task_GPSStreamRX();

    printf("\n=========================================================\n");
    printf(" STM32 Example Completed Successfully.\n");
    printf("=========================================================\n\n");
    return 0;
}
