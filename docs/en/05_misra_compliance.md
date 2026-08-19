# MISRA-C:2012 Compliance & Code Safety Report

The **EmbeddedDataConvert (EDC)** library is engineered specifically for mission-critical, automotive, and industrial embedded targets following **MISRA-C:2012** rules.

---

## 1. Key Rules Adherence

* **Rule 4.6 (Required):** Explicit fixed-width types from `stdint.h` (`uint8_t`, `int32_t`, `uint64_t`, etc.) are used exclusively.
* **Rule 8.4 (Required):** All global functions have compatible external declarations in corresponding header files.
* **Rule 10.1 (Required):** No implicit or dangerous integer promotions/conversions; all casts are explicit.
* **Rule 12.1 (Advisory):** Operator precedence is explicitly parenthesized to avoid compiler ambiguities.
* **Rule 14.3 (Required):** All loops are strictly bounded with invariant termination conditions.
* **Rule 17.2 (Required):** Zero recursive function calls; all stack execution paths are $O(1)$.
* **Rule 17.7 (Required):** Return values of all critical functions are marked with `EDC_NODISCARD` (`warn_unused_result`).
* **Rule 21.3 (Required):** Dynamic memory allocation functions (`malloc`, `free`, `calloc`, `realloc`) are strictly forbidden.
* **Rule 21.6 (Required):** Heavy standard I/O routines like `sprintf` are completely replaced by custom lightweight converters.

---

## 2. Defensive Programming

* **NULL Pointer Verification:** All pointers are guarded at function entry points, returning `EDC_ERR_NULL_PTR`.
* **Buffer Overflow Prevention:** Every string and buffer manipulation routine accepts explicit buffer capacity limits and verifies boundaries continuously.
* **Arithmetic Overflow Guards:** Integer and floating-point parsers check for overflow prior to multiplication/addition operations.

---

## 3. Compiler Compatibility Matrix

Verified with zero warnings and zero errors under:
* **GCC / ARM GCC** (`-Wall -Wextra -Werror -pedantic -std=c99`)
* **Clang / ARM Clang AC6** (Keil MDK)
* **STM32CubeIDE** (GNU ARM Toolchain)
* **ESP-IDF** (Xtensa & RISC-V GCC)
* **PlatformIO**
