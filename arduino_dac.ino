#define DAC_PIN A0
#define REGION1_LIMIT 1.0 // all voltages below this will have OFFSET1_ADJUST
#define REGION2_LIMIT 2.0 // all voltages below this will have OFFSET2_ADJUST
#define OFFSET1_ADJUST 0  // for voltage < REGION1_LIMIT
#define OFFSET2_ADJUST 2  // for REGION1_LIMIT < voltage < REGION2_LIMIT 
#define OFFSET3_ADJUST 4  // for voltage > REGION2_LIMIT
#define LSB 0.0032258     // 1 LSB is this voltage, 3.2258mV

int offset_adjust;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  analogWrite(DAC_PIN, 0); // 10bit DAC, 0=0V, 512=1.65V, 1023=3.3V
  Serial.println("Give DAC voltage between 0.0 and 3.3 (do not include V):");
}

void loop() {
  byte n = Serial.available();
  if (n != 0)
  {
    float voltage_val = Serial.parseFloat();
    if ((voltage_val < 0.0) || (voltage_val > 3.3))
    {
      Serial.println("DAC voltage must be between 0.0 and 3.3V inclusive");
    }
    else
    {
      int dac_val = int(voltage_val / 3.3 * 1023 + 0.001); // add small amount to correct float to int conversion
      int error = int((voltage_val - (dac_val * LSB)) * 1000.0); // error is in mV as dac_val is a truncation
      Serial.print("New DAC voltage: ");
      Serial.print(voltage_val,3);
      Serial.print("V ...DAC code is decimal ");
      Serial.print(dac_val);
      if (dac_val > (1023 - OFFSET3_ADJUST))
      {
        Serial.print(" ...Error is within ");
        Serial.print(OFFSET3_ADJUST+2);
        Serial.print(" LSB (");
        Serial.print((OFFSET3_ADJUST+2)*LSB*1000);
        Serial.println("mV)");
      }
      else if (error == 0)
      {
        Serial.println(" ...Error is within 1 LSB (3.23mV)");
      }
      else
      {
        Serial.println(" ...Error is within 2 LSBs (6.45mV)");
      }
      if (voltage_val < REGION1_LIMIT)
      {
        offset_adjust = OFFSET1_ADJUST;
      }
      else if (voltage_val < REGION2_LIMIT)
      {
        offset_adjust = OFFSET2_ADJUST;
      }
      else
      {
        offset_adjust = OFFSET3_ADJUST;
      }
      if (dac_val < (1024 - offset_adjust))
      {
        analogWrite(DAC_PIN, dac_val + offset_adjust);
      }
      else
      {
        analogWrite(DAC_PIN, dac_val); // this has greater than OFFSET3_ADJUST error
      }
    }
  }
  delay(1000);
