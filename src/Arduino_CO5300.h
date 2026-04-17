#ifndef _ARDUINO_CO5300_H_
#define _ARDUINO_CO5300_H_

#include <Arduino_GFX.h>

class Arduino_CO5300 : public Arduino_GFX {
public:
    Arduino_CO5300(Arduino_DataBus *bus, int8_t rst = -1, uint8_t r = 0, bool ips = false,
                   int16_t w = 410, int16_t h = 502,
                   uint8_t col_offset1 = 0, uint8_t row_offset1 = 0,
                   uint8_t col_offset2 = 0, uint8_t row_offset2 = 0)
        : Arduino_GFX(w, h), _bus(bus), _rst(rst), _r(r), _ips(ips),
          _col_offset1(col_offset1), _row_offset1(row_offset1),
          _col_offset2(col_offset2), _row_offset2(row_offset2) {}

    bool begin(int32_t speed = GFX_NOT_DEFINED) override {
        if (_bus->begin(speed) == false) return false;
        if (_rst != GFX_NOT_DEFINED) {
            pinMode(_rst, OUTPUT);
            digitalWrite(_rst, HIGH); delay(100);
            digitalWrite(_rst, LOW);  delay(100);
            digitalWrite(_rst, HIGH); delay(100);
        }
        return true;
    }

    void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override {
        _bus->writeCommand(0x2C); // Memory Write
        _bus->write16(color);
    }

protected:
    Arduino_DataBus *_bus;
    int8_t _rst, _r;
    bool _ips;
    uint8_t _col_offset1, _row_offset1, _col_offset2, _row_offset2;
};

#endif