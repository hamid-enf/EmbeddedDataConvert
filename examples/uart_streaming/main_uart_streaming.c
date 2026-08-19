/**
 * @file main_uart_streaming.c
 * @brief Zero-allocation UART streaming frame parser example.
 *
 * Demonstrates:
 * 1. Simulating a noisy serial bus (UART/RS485) receiving bytes one by one.
 * 2. Framing protocol packets with start delimiter '$' and end delimiter '\n'.
 * 3. Extracting and parsing key-value sensor fields without dynamic memory.
 *
 * @author EmbeddedDataConvert Team
 */

#include "edc.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("===================================================================\n");
    printf(" UART Streaming Frame Parser Example (EDC)\n");
    printf("===================================================================\n\n");

    char frame_buffer[128];
    EDC_StreamParser parser;
    if (EDC_StreamParser_Init(&parser, frame_buffer, sizeof(frame_buffer), '$', '\n') != EDC_OK) {
        return 1;
    }

    /* Simulated incoming byte stream over UART with noise and multiple frames */
    const char *uart_rx_stream =
        "NOISE_BYTE_0xFF_0x00\r\n"
        "$NODE=101,TEMP=24.8,STATUS=\"OK\"\n"
        "GARBAGE_CHARS_1234"
        "$NODE=102,TEMP=31.2,STATUS=\"WARN\"\n";

    size_t stream_len = strlen(uart_rx_stream);
    printf("Feeding %zu bytes into Stream Parser...\n\n", stream_len);

    uint32_t frame_count = 0;

    for (size_t i = 0; i < stream_len; i++) {
        uint8_t byte = (uint8_t)uart_rx_stream[i];
        const char *frame = NULL;
        size_t frame_len = 0;

        EDC_Status st = EDC_StreamParser_Feed(&parser, byte, &frame, &frame_len);
        if (st == EDC_OK) {
            frame_count++;
            printf("[FRAME #%u RECEIVED] Length: %zu bytes | Content: %s\n",
                   (unsigned int)frame_count, frame_len, frame);

            /* Parse Key-Value payload inside frame */
            EDC_KVPair pairs[8];
            size_t pair_count = 0;
            if (EDC_KV_Parse(frame, frame_len, ',', '=', pairs, 8, &pair_count) == EDC_OK) {
                const char *node_str = NULL;
                const char *temp_str = NULL;
                const char *status_str = NULL;
                size_t n_len = 0, t_len = 0, s_len = 0;

                uint32_t node_id = 0;
                float temperature = 0.0f;

                if (EDC_KV_Find(pairs, pair_count, "NODE", 4, &node_str, &n_len) == EDC_OK) {
                    if (EDC_StringToU32(node_str, n_len, 10, &node_id, NULL) != EDC_OK) {
                        node_id = 0;
                    }
                }
                if (EDC_KV_Find(pairs, pair_count, "TEMP", 4, &temp_str, &t_len) == EDC_OK) {
                    if (EDC_StringToFloat(temp_str, t_len, &temperature, NULL) != EDC_OK) {
                        temperature = 0.0f;
                    }
                }
                if (EDC_KV_Find(pairs, pair_count, "STATUS", 6, &status_str, &s_len) != EDC_OK) {
                    status_str = NULL;
                }

                printf("  -> Extracted Node ID     : %u\n", (unsigned int)node_id);
                printf("  -> Extracted Temperature : %.2f °C\n", temperature);
                if (status_str != NULL) {
                    printf("  -> Extracted Status      : %.*s\n\n", (int)s_len, status_str);
                }
            }
        }
    }

    printf("===================================================================\n");
    printf(" Stream Processing Complete. Successfully parsed %u valid frames.\n", (unsigned int)frame_count);
    printf("===================================================================\n\n");
    return 0;
}
