# EDC Complete API Reference Manual

---

## 1. Numeric Module (`edc_numeric.h`)

### `EDC_U64_ToStringEx` / `EDC_I64_ToStringEx`
Converts 64-bit unsigned/signed integers to string in any base between 2 and 36.
* **Parameters:**
  * `val`: Value to convert (`uint64_t` or `int64_t`).
  * `buf`: Output character buffer.
  * `buf_size`: Total buffer capacity in bytes.
  * `base`: Numerical base (2 to 36).
  * `uppercase`: True for 'A'-'F', false for 'a'-'f'.
  * `pad_zeros`: Minimum digits (padded with leading '0's).
  * `out_len`: Optional pointer to receive written length (excluding `\0`).
* **Complexity:** $O(\log_{base} N)$, Stack $< 64$ bytes.

### `EDC_StringToU64` / `EDC_StringToI64`
Parses string to integer with automatic base detection (`0x` for hex, `0b` for binary, decimal default), whitespace skipping, and strict overflow/underflow prevention.

### `EDC_Float_ToString` / `EDC_Double_ToString`
Formats single/double precision floats into strings with customizable decimal places without libc `sprintf`. Handles `NaN`, `+inf`, `-inf`, and `-0.0`.

### `EDC_StringToFloat` / `EDC_StringToDouble`
Parses decimal and scientific notation strings (e.g. `1.23e-4`) to floating-point values.

### `EDC_Float_To_Q` / `EDC_Q_To_Float` / `EDC_Q_ToString`
Converts float to fixed-point Q-format (Q8, Q15, Q31, Qm.n) and formats Q numbers directly to decimal strings without floating-point FPU hardware.

---

## 2. String & Hex Module (`edc_string.h`)

* `EDC_StrLen`, `EDC_StrCopy`, `EDC_StrCompare`, `EDC_StrCompareCase`: Bounded, overflow-safe string manipulation.
* `EDC_StrTrim`: Zero-copy slice trimming of leading and trailing whitespace.
* `EDC_BytesToHex`: Converts raw byte buffers to hex strings with optional spacing.
* `EDC_HexToBytes`: Converts hex strings (with optional `0x`, `:`, spaces) to raw bytes.

---

## 3. Unicode Module (`edc_unicode.h`)

* `EDC_UTF8_DecodeChar` / `EDC_UTF8_EncodeChar`: RFC 3629 compliant codepoint (U+0000 to U+10FFFF) encode/decode.
* `EDC_UTF8_Validate`: Full buffer validation rejecting overlong sequences, surrogate pairs (U+D800..U+DFFF), and codepoints > U+10FFFF.
* `EDC_UTF8_ToUTF16` / `EDC_UTF16_ToUTF8`: Transcoding between UTF-8 and UTF-16 arrays.
* `EDC_UTF8_StreamDecoder`: Byte-by-byte streaming state machine for UART/SPI reception.

---

## 4. Persian & Arabic Module (`edc_persian.h`)

* `EDC_PersianDigitsToEnglish` / `EDC_PersianDigitsToEnglish_InPlace`: Converts Persian (۰-۹) and Arabic (٠-٩) digits to ASCII (0-9). In-place conversion uses zero extra memory.
* `EDC_EnglishDigitsToPersian` / `EDC_EnglishDigitsToArabic`: Converts ASCII digits to Persian/Arabic UTF-8.
* `EDC_Persian_Normalize` / `EDC_Persian_Normalize_InPlace`: Normalizes Arabic Yeh/Kaf to Persian, strips Harakat/diacritics, strips Tatweel, converts Teh Marbuta to Heh, handles ZWNJ.
* `EDC_Persian_TransliterateToLatin`: Phonetic transliteration of Persian text to Latin characters (Fingilish) for standard 7-bit GSM SMS and serial logs.

---

## 5. GPS & NMEA Module (`edc_gps.h`)

* `EDC_GPS_NMEA_ToDecimalDegrees` / `EDC_GPS_DecimalDegrees_ToNMEA`: Converts NMEA DDMM.MMMM format to Decimal Degrees and back.
* `EDC_GPS_ParseNMEA`: Complete parser for $GPGGA, $GPRMC, $GPVTG, $GPGLL with XOR checksum verification.
* `EDC_GPS_StreamFeed`: Streaming byte-by-byte feeder for UART ISRs.

---

## 6. Unit Conversion Module (`edc_units.h`)

Fast `static inline` conversion routines for:
* **Distance:** Meters, Kilometers, Feet, Inches, Nautical Miles.
* **Speed:** m/s, km/h, Knots, mph.
* **Temperature:** Celsius, Fahrenheit, Kelvin.
* **Pressure:** Pascal, Bar, PSI, hPa, Atmosphere.
* **Angle:** Degrees, Radians.

---

## 7. Parser Module (`edc_parser.h`)

* `EDC_KV_Parse` / `EDC_KV_Find`: Zero-copy key-value pair extraction from delimiter-separated strings (supports quotes).
* `EDC_CSV_Tokenizer`: Delimiter/CSV token scanner.
* `EDC_StreamParser`: Packet framer for serial byte streams with start and end delimiters.
