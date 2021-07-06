// these are common for all MCU
#define DAC_PIN A0
#define LSB 0.0032258 // 1 LSB is this voltage, 3.2258mV
#define REGION1_LIMIT 1.0 // all voltages below this will have OFFSET1_ADJUST
#define REGION2_LIMIT 2.0 // all voltages below this will have OFFSET2_ADJUST

// these are part specific offset adjustments, make negative if they are to be subtracted
//                             0, 1, 2
const int OFFSET1_ADJUST[3] = {0, 0, 1}; // for voltage < REGION1_LIMIT
const int OFFSET2_ADJUST[3] = {0, 2, 1}; // for REGION1_LIMIT < voltage < REGION2_LIMIT 
const int OFFSET3_ADJUST[3] = {0, 4, 3}; // for voltage > REGION2_LIMIT

int offset_select;
int offset_adjust;
char unique_id[34];

void setup() {
  Serial.begin(115200);
  while(!Serial);
  analogWrite(DAC_PIN, 0); // 10bit DAC, 0=0V, 512=1.65V, 1023=3.3V
  Serial.print("Unique SAMD21 ID is: ");
  samd21_unique_id(unique_id);
  Serial.println(unique_id);
  if (String(unique_id) == "0x0c85c77e5053574c342e3120ff031e18")
  {
    offset_select = 2;
  }
  else if (String(unique_id) == "0x7223766a50535154332e3120ff0b0732")
  {
    offset_select = 1;
  }
  else
  {
    offset_select = 0; // no offsets
  }
  Serial.print("Using offset_select: ");
  Serial.println(offset_select);
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
      if (dac_val > (1023 - OFFSET3_ADJUST[offset_select]))
      {
        Serial.print(" ...Error is within ");
        Serial.print(OFFSET3_ADJUST[offset_select]+2);
        Serial.print(" LSB (");
        Serial.print((OFFSET3_ADJUST[offset_select]+2)*LSB*1000);
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
        offset_adjust = OFFSET1_ADJUST[offset_select];
      }
      else if (voltage_val < REGION2_LIMIT)
      {
        offset_adjust = OFFSET2_ADJUST[offset_select];
      }
      else
      {
        offset_adjust = OFFSET3_ADJUST[offset_select];
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
}

void samd21_unique_id( char * id_buff )
{   
    volatile uint32_t val0, val1, val2, val3;
    volatile uint32_t *val0_ptr = (volatile uint32_t *)0x0080A00C;
    volatile uint32_t *val1_ptr = (volatile uint32_t *)0x0080A040;
    volatile uint32_t *val2_ptr = (volatile uint32_t *)0x0080A044;
    volatile uint32_t *val3_ptr = (volatile uint32_t *)0x0080A048;
    val0 = *val0_ptr;
    val1 = *val1_ptr;
    val2 = *val2_ptr;
    val3 = *val3_ptr;
    static char format[] = "0x%08x%08x%08x%08x";
    sprintf(id_buff, format,val0, val1, val2, val3);
}
