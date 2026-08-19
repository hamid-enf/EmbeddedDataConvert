# Conceptual Distinction Matrix in Embedded Data Processing

A critical design goal of **EDC** is clear technical differentiation between operations frequently confused in embedded software.

---

## 1. Technical Comparison Matrix

| Concept | Technical Definition | Example Input | Example Output | EDC Module |
| :--- | :--- | :--- | :--- | :--- |
| **Conversion** | Changing in-memory data representation type | `int32_t: 123` | `float: 123.0f` | `edc_numeric` |
| **Parsing** | Extracting binary/numeric value from structured text | `"123.45"` | `double: 123.45` | `edc_numeric`, `edc_gps` |
| **Formatting** | Generating human-readable text from binary data | `double: 123.456` | `"123.46"` (2 decimals) | `edc_numeric`, `edc_gps` |
| **Encoding** | Mapping abstract characters to standard byte sequences | `U+0633` (س) | UTF-8 `0xD8 0xB3` | `edc_unicode` |
| **Decoding** | Extracting Unicode codepoints from byte sequences | UTF-8 `0xD8 0xB3` | `U+0633` | `edc_unicode` |
| **Normalization** | Standardizing equivalent glyphs within a script | Arabic `ي` (`U+064A`) | Persian `ی` (`U+06CC`) | `edc_persian` |
| **Transliteration** | Phonetic mapping of characters across alphabets | Persian `"سلام"` | Latin `"salam"` | `edc_persian` |
| **Translation** | Semantic mapping between natural languages | Persian `"سلام دنیا"` | English `"Hello World"` | **Out of Scope** |
| **Unit Conversion** | Scaling physical quantities | `10.0 m/s` | `36.0 km/h` | `edc_units` |
| **Protocol Parsing** | Extracting telemetry fields from serial frames | `"$LAT=31.12,LON=48.12\n"` | GPS struct | `edc_parser`, `edc_gps` |

---

## 2. Why Transliteration is NOT Language Translation

* **Transliteration (Fingilish):** Deterministic 1-to-1 or 1-to-many phonetic mapping table ($O(1)$ space, $< 100$ bytes Flash). Used for GSM 7-bit ASCII SMS, debug consoles, and serial displays.
* **Translation:** Requires lexical dictionaries, grammar rules, and NLP models (megabytes of storage), unsuitable for a lightweight $< 24$ KB MCU conversion library.

EDC provides configurable **Transliteration** and makes no claim of natural language translation.
