# TF03K TTL Serial Communication Protocol Specification

**Baud rate:** 9600 Kbps  
**Data bits:** 8  
**Stop bit:** 1  
**Parity check:** None  
**Bytes / frame:** 16 bytes each time  
**Interval:** One second  
**Note:** Only send when coulometer working (Backlight on)  
**Interface level:** TTL level 3–5V  
**Power:** VCC and GND must be powered by external supply (5–10 mA required)  
**Isolation method:** Solid-state optocoupler  
**Checksum:** 8-bit cumulative sum from the first byte to the fifteenth byte  

---

## Byte Table

| Byte num | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|
| Meaning | Index bytes | Capacity percentage | Total battery voltage (V) | Real-time battery capacity (mAh) | Current (mA) | Remaining time (s) | Check |
| Hex example | A5 | 02 | 07 | 00 | 00 | 87 | 00 | 00 | 24 | 05 | 00 | 00 | 94 | 11 | xx |

**Example interpretation:**

- **Index:** A5  
- **Percentage:** 0%–2%  
- **Total voltage:** 0x07D0 → 2000 mV → 2.00 V  
- **Capacity:** 0x0000 → 0 mAh (example shows 2695 mAh in diagram)  
- **Current:** 0x0000 2405 → 9221 mA → 9.221 A  
- **Remaining time:** 00:00:00 → 99:59:59  

---

## Pin Description (from schematic diagram)

- **RXD:** TF03 receive pin (only need TF03 data; can disconnect otherwise)  
- **TXD:** TF03 transmission pin  
- **GND:** Ground  
- **VCC:** 3–5V power supply (must be externally powered)

**Important notes:**

- VCC and GND must connect to external power.  
- Interface is **not RS-232**; must use **TTL level**.
