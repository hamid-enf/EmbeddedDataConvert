# EDC (EmbeddedDataConvert) Architecture & Design Principles

The **EmbeddedDataConvert (EDC)** library is an ultra-lightweight, standalone, and MISRA-C:2012 compliant C99/C11 library engineered for data conversion, parsing, formatting, encoding, and normalization in resource-constrained embedded systems, with primary focus on **STM32** and **ESP32** microcontrollers.

---

## 1. Core Architectural Principles

### 1.1. Zero Dynamic Memory Allocation
* No calls to `malloc()`, `free()`, `calloc()`, or `realloc()`.
* Eliminates memory fragmentation and guarantees deterministic $O(1)$ stack behavior.
* Safe for use in Interrupt Service Routines (ISRs) and hard Real-Time tasks.

### 1.2. Zero Hardware or RTOS Dependencies
* Pure, standard C99/C11 with explicit `stdint.h` types.
* No dependencies on STM32 HAL, CMSIS, Arduino Core, or FreeRTOS.
* Works seamlessly across Bare-Metal, FreeRTOS, Zephyr, ESP-IDF, STM32Cube, and standard POSIX/Windows hosts.

### 1.3. Full Modularity & Configurable Footprint
Fine-grained control via `edc_config.h`:
* `EDC_FEATURE_NUMERIC`
* `EDC_FEATURE_FLOAT`
* `EDC_FEATURE_FIXED_POINT`
* `EDC_FEATURE_STRING`
* `EDC_FEATURE_UNICODE`
* `EDC_FEATURE_PERSIAN`
* `EDC_FEATURE_TRANSLITERATION`
* `EDC_FEATURE_GPS`
* `EDC_FEATURE_UNITS`
* `EDC_FEATURE_PARSER`
* `EDC_FEATURE_BUFFER`

Users needing only integer conversions do not pull in Unicode or GPS modules, keeping Flash usage under 2 KB.

---

## 2. Thread-Safety & Stateless Design

* **Zero Global Mutable State:** No static or global mutable variables anywhere in the codebase.
* **100% Reentrant:** All functions operate strictly on passed buffers and local stack memory, enabling lockless concurrent access across multi-threaded RTOS tasks and multi-core architectures (e.g. ESP32 dual-core).

---

## 3. Status Codes & Error Handling

EDC never fails silently. All critical functions return an `EDC_Status` enum:

| Status Code | Value | Description |
| :--- | :--- | :--- |
| `EDC_OK` | 0 | Operation completed successfully. |
| `EDC_ERR_NULL_PTR` | -1 | NULL pointer provided for required parameter. |
| `EDC_ERR_INVALID_PARAM` | -2 | Parameter out of valid range or unsupported. |
| `EDC_ERR_BUFFER_TOO_SMALL` | -3 | Destination buffer capacity is insufficient. |
| `EDC_ERR_INVALID_FORMAT` | -4 | Input string/data does not conform to format. |
| `EDC_ERR_OVERFLOW` | -5 | Arithmetic or numeric representation overflow. |
| `EDC_ERR_UNDERFLOW` | -6 | Arithmetic or numeric representation underflow. |
| `EDC_ERR_INVALID_ENCODING` | -7 | Malformed UTF byte sequence or invalid codepoint. |
| `EDC_ERR_UNSUPPORTED` | -8 | Base or operation is not supported. |
| `EDC_ERR_INCOMPLETE` | -9 | Streaming input incomplete, waiting for more bytes. |
| `EDC_ERR_NOT_FOUND` | -10 | Key, delimiter, or token not found. |
| `EDC_ERR_CHECKSUM_MISMATCH`| -11 | Checksum validation failed (e.g. NMEA XOR). |
