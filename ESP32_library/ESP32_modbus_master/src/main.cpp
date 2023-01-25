#include  <Arduino.h>
//#include  <ModbusRtu.h>
#include  <STM32ModbusAccess.h>




int RE_PinNumber = 22;
int DE_PinNumber = 23;




void setup() {
  // put your setup code here, to run once:
  modbusSetup(19200,22,23);
  Serial.begin(19200);
}

void loop() {
  // // put your main code here, to run repeatedly:
  // int32_t buffer1;
  // uint16_t counterbuf;

  // for (uint8_t i = 0; i < 16; i++)
  // {
  //   Serial.printf("Analog input# ");
  //   counterbuf=i+1;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetAnalogInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  // for (uint8_t i = 0; i < 8; i++)
  // {
  //   Serial.printf("Digital input# ");
  //   counterbuf=i+1;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetDigitalInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }

  // static bool status = false;
  // for (uint8_t i = 0; i < 8; i++)
  // {
  //   Serial.printf("Relay output# ");
  //   counterbuf=i+1;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32SETCoil(1,i, status);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  // status = !status;

  // for (uint8_t i = 8; i < 16; i++)
  // {
  //   Serial.printf("S input# ");
  //   counterbuf=i+1-8;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetDigitalInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  // for (uint8_t i = 16; i < 20; i++)
  // {
  //   Serial.printf("EF input# ");
  //   counterbuf=i+1-16;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetDigitalInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  // for (uint8_t i = 20; i < 22; i++)
  // {
  //   Serial.printf("ADC chip failure signal# ");
  //   counterbuf=i+1-20;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetDigitalInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }

  // for (uint8_t i = 22; i < 26; i++)
  // {
  //   Serial.printf("DAC chip failure signal# ");
  //   counterbuf=i+1-22;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetDigitalInput(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
   
  // for (uint8_t i = 0; i < 12; i++)
  // {
  //   Serial.printf("Read coil# ");
  //   counterbuf=i+1;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetCoil(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }

  // for (uint8_t i = 0; i < 4; i++)
  // {
  //   Serial.printf("Read DAC setup# ");
  //   counterbuf=i+1;
  //   Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetHoldingRegister(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  //   for (uint8_t i = 4; i < 5; i++)
  // {
  //   Serial.printf("Read the device address set ");
  //   //counterbuf=i+1;
  //   //Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetHoldingRegister(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }


  // for (uint8_t i = 5; i < 13; i++)
  // {
  //   Serial.printf("Read the ADC chip Vgain # ");
  //   //counterbuf=i+1;
  //   //Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetHoldingRegister(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }
  // for (uint8_t i = 13; i < 17; i++)
  // {
  //   Serial.printf("Read the ADC chip Igain # ");
  //   //counterbuf=i+1;
  //   //Serial.println(counterbuf);
  //   Serial.printf(" = ");
  //   buffer1=STM32GetHoldingRegister(1,i);
  //   if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println(buffer1);
  //   }
  // }

  // Serial.println("setting DAC#1 to 2048");
  // buffer1=STM32SetHoldingRegister(1,0,2048);
  // if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println("done");
  //   }

  // Serial.println("setting EN_5V to 1");
  // buffer1=STM32SETCoil(1,8,1);
  // if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println("done");
  //   }

  // Serial.println("setting EN_ISO_PWR to 1");
  // buffer1=STM32SETCoil(1,9,1);
  // if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println("done");
  //   }


  // Serial.println("setting R3 to 1");
  // buffer1=STM32SETCoil(1,2,1);
  // if (buffer1 == -1)
  //   {
  //     Serial.printf("MODBUS ERROR");
  //   }
  //   else
  //   {
  //     Serial.println("done");
  //   }

  

}
  
  
  

  
 