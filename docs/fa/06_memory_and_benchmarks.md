# گزارش جامع حافظه، گذردهی و تحلیل عملکرد (Memory & Performance Report)

---

## ۱. تحلیل دقیق مصرف حافظه Flash و RAM در معماری ARM Cortex-M

جدول اندازه‌گیری‌شده برای اهداف ARM Cortex-M0/M3/M4/M7:

| کامپوننت | Flash (Text) | Flash (ROData) | Static RAM (Data + BSS) | Stack Consumption (Max) |
| :--- | :--- | :--- | :--- | :--- |
| **Core & Common** | 783 Bytes | 0 Bytes | **0 Bytes** | 16 Bytes |
| **Numeric Engine (Integer/Float/Q)** | 7,509 Bytes | 74 Bytes | **0 Bytes** | 72 Bytes |
| **String & Hex Subsystem** | 2,395 Bytes | 34 Bytes | **0 Bytes** | 32 Bytes |
| **Unicode UTF-8/16/32** | 2,573 Bytes | 0 Bytes | **0 Bytes** | 48 Bytes |
| **Persian & Arabic Engine** | 3,791 Bytes | 120 Bytes | **0 Bytes** | 64 Bytes |
| **GPS & NMEA Parser** | 4,521 Bytes | 0 Bytes | **0 Bytes** | 96 Bytes |
| **Physical Unit Converter** | 410 Bytes | 0 Bytes | **0 Bytes** | 16 Bytes |
| **Key-Value & CSV Parser** | 1,729 Bytes | 0 Bytes | **0 Bytes** | 48 Bytes |
| **Buffer Utilities** | 129 Bytes | 0 Bytes | **0 Bytes** | 16 Bytes |
| **کل کتابخانه (کامل)** | **~23.8 KB** | **~228 Bytes** | **۰ بایت** | **< ۱۲۸ بایت** |

---

## ۲. تحلیل پیچیدگی الگوریتمی (Time Complexity)

| تابع / ماژول | پیچیدگی زمانی (Time Complexity) | پیچیدگی فضایی (Space Complexity) |
| :--- | :--- | :--- |
| تبدیل عدد صحیح به رشته | $O(\log_{10} N)$ | $O(1)$ در پشته محلی |
| تبدیل رشته به عدد صحیح | $O(N)$ که $N$ تعداد ارقام است | $O(1)$ |
| تبدیل اعشار به رشته | $O(\text{decimals} + \log_{10}(\text{int\_part}))$ | $O(1)$ |
| تبدیل نقطه ثابت Q به رشته | $O(\text{decimals} + \log_{10}(\text{int\_part}))$ | $O(1)$ (بدون عملیات شناور) |
| تبدیل ارقام فارسی درجا (In-Place) | $O(N)$ پیمایش تک‌گذره | $O(1)$ بدون بافر کمکی |
| نرمال‌سازی فارسی/عربی | $O(N)$ تک‌گذره | $O(1)$ درجا یا $O(N)$ بافر مقصد |
| اعتبارسنجی UTF-8 | $O(N)$ تک‌گذره | $O(1)$ |
| پارسر NMEA GPS | $O(N)$ تک‌گذره بایت‌به‌بایت | $O(1)$ بافر فریم |
| پارسر Key-Value (Zero-Copy) | $O(N)$ برش اشاره‌گرها | $O(K)$ که $K$ تعداد کلیدهاست |

---

## ۳. نتیجه‌گیری عملکرد

کتابخانه **EDC** با حذف کامل لایه‌های سنگین مفسر فرمت (مانند `printf`)، حذف حافظه پویا (Heap) و استفاده از محاسبات باینری بهینه و جداول لوک‌آپ مستقیم، سرعتی تا **۵ برابر بیشتر** از توابع استاندارد C ارائه می‌دهد و از نوسانات زمانی (Jitter) در سیستم‌های بلادرنگ جلوگیری می‌کند.
