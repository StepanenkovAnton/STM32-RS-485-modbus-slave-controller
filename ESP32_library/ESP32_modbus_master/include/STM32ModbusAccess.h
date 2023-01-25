#include <Arduino.h>
#include <ModbusRtu.h>


Modbus master(0,Serial2,2); // this is master and RS-232 or USB-FTDI
unsigned long timetick;
unsigned long u32wait = 0;

extern int RE_PinNumber;
extern int DE_PinNumber;
 


/// data array for modbus network sharing
///
///
uint16_t au16data[16];



///modbus message structure
///
///
modbus_t telegram;





void modbusSetup (int speed, int REPin, int DEPin)
{
RE_PinNumber = REPin;
DE_PinNumber = DEPin;
pinMode(RE_PinNumber , OUTPUT);
pinMode(DE_PinNumber , OUTPUT);
digitalWrite(RE_PinNumber,0);//set to recieve mode
digitalWrite(DE_PinNumber,0);//set to recieve mode
 
Serial2.begin(speed);
master.start();
master.setTimeOut( 250 ); // if there is no answer in "timeout" ms, roll over
       
}

int32_t STM32GetAnalogInput(int slaveAddress, int inputNumber)
{

    int32_t     errorTicker = 0;
   

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 4; // function code (this one is input registers read)
    telegram.u16RegAdd = inputNumber; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;
    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    if (errorTicker>=1)
    { 
        return au16data[0];
    }
    else
    {
        return -1;
    }


    
}




int32_t STM32SetHoldingRegister(int slaveAddress, int inputNumber, uint16_t valueToSet)
{

    int32_t     errorTicker = 0;
   

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 6; // function code (this one is holding register write)
    telegram.u16RegAdd = inputNumber; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    au16data[0]=valueToSet;
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;
    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    if (errorTicker>=1)
    { 
        return 1;
    }
    else
    {
        return -1;
    }


    
}



int32_t STM32GetHoldingRegister(int slaveAddress, int inputNumber)
{

    int32_t     errorTicker = 0;
   

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 3; // function code (this one is holdilg registers read)
    telegram.u16RegAdd = inputNumber; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;
    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    if (errorTicker>=1)
    { 
        return au16data[0];
    }
    else
    {
        return -1;
    }


    
}



int32_t STM32GetDigitalInput(int slaveAddress, int inputNumber)
{

    int32_t     errorTicker = 0;
    uint16_t    buf = 0;
    uint8_t     digitalInputs[32];
    uint16_t    extender = 1;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 2; // function code (this one is digital inputs read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 32; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      digitalInputs[i]= (au16data[0] & extender)/extender;
      extender = extender*2;  
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      digitalInputs[i+16]= (au16data[1] & extender)/extender;
      extender = extender*2;  
    }

    

    if (errorTicker>=1)
    { 
        return digitalInputs[inputNumber];
        //return au16data[1];
    }
    else
    {
        return -1;
    }


    
}

int32_t STM32GetAllDigitalInputs(int slaveAddress, int *digitalInputs[32])
{

    int32_t     errorTicker = 0;
    uint16_t    buf = 0;
    uint16_t    extender = 1;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 2; // function code (this one is digital inputs read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 32; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      *digitalInputs[i]= (au16data[0] & extender)/extender;
      extender = extender*2;  
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      *digitalInputs[i+16]= (au16data[1] & extender)/extender;
      extender = extender*2;  
    }

    

    if (errorTicker>=1)
    { 
        return 1;
        //return au16data[1];
    }
    else
    {
        return -1;
    }


    
}


int32_t STM32GetCoil(int slaveAddress, int inputNumber)
{

    int32_t     errorTicker = 0;
    uint16_t    buf = 0;
    uint8_t     digitalInputs[32];
    uint16_t    extender = 1;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 1; // function code (this one is digital inputs read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 16; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      digitalInputs[i]= (au16data[0] & extender)/extender;
      extender = extender*2;  
    }    

    if (errorTicker>=1)
    { 
        return digitalInputs[inputNumber];
    }
    else
    {
        return -1;
    }


    
}


int32_t STM32GetAllCoils(int slaveAddress, uint8_t  *digitalInputs[16])
{

    int32_t     errorTicker = 0;
    uint16_t    buf = 0;
    uint16_t    extender = 1;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 1; // function code (this one is digital inputs read)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 16; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
    extender = 1;
    for (uint8_t i = 0; i < 16; i++)
    {
      *digitalInputs[i]= (au16data[0] & extender)/extender;
      extender = extender*2;  
    }    

    if (errorTicker>=1)
    { 
        return 1;
    }
    else
    {
        return -1;
    }


    
}



int32_t STM32SETCoil(int slaveAddress, int inputNumber, uint8_t valueToSet)
{

    int32_t     errorTicker = 0;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 5; // function code (this one is single coil write)
    telegram.u16RegAdd = inputNumber; // start address in slave
    telegram.u16CoilsNo = 1; // number of elements (coils or registers) to write
    au16data[0]=valueToSet;
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
        

    if (errorTicker>=1)
    { 
        return 1;
    }
    else
    {
        return -1;
    }


    
}


int32_t STM32SETAllCoils(int slaveAddress,  uint8_t valuesToSet[16])
{

    int32_t     errorTicker = 0;

    telegram.u8id = slaveAddress; // slave address
    telegram.u8fct = 5; // function code (this one is single coil write)
    telegram.u16RegAdd = 0; // start address in slave
    telegram.u16CoilsNo = 16; // number of elements (coils or registers) to write
    for (uint8_t i = 0; i < 16; i++)
    {
        au16data[i]=valuesToSet[i];
    }
    
    
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino
    
    master.query( telegram );
    u32wait = millis() + 500;


    while (millis() <= u32wait)
    {
        if (master.poll()>0) // check incoming messages
        {
         errorTicker++;   
        }
        //if (master.getState() == 0)
        //{
        //errorTicker++; 
               
        //}
    }
    
        

    if (errorTicker>=1)
    { 
        return 1;
    }
    else
    {
        return -1;
    }


    
}
