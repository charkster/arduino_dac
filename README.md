# arduino_dac
Arduino library DAC control using a serial (USB) terminal. Tried this on Adafruit SAMD21 QT PY and Seeeduino Xiao.
Additional offset correction is implemented to allow for very accurate DAC output. Three ranges are defined by REGION1_LIMIT and REGION2_LIMIT.
OFFSET1_ADJUST is for voltages from 0.0V to REGION1_LIMIT, OFFSET2_ADJUST is for voltages greater than REGION1_LIMIT, but less than REGION2_LIMIT. OFFSET3_ADJUST is for voltages above REGION2_LIMIT. The offset adjustments are numbers of LSB to add (or subtract if specified negative).
