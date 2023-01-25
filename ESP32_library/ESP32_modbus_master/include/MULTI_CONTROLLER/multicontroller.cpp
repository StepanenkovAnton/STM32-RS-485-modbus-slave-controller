/**
 * @file multicontroller.cpp
 * @author Gabriel Ballesteros (gabriel@coratronics.com)
 * @brief This file contains all the functions declared in multicontroller.h
 * @version 0.1
 * @date 2020-11-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "multicontroller.h"

multiController mc(MODBUS_SERIAL, (gpio_num_t)RX, (gpio_num_t)TX, (gpio_num_t)DE, (gpio_num_t)RE, MODBUS_BAUDRATE);

enum{
    IDLE,
    SENDING,
    RECEIVING,
    PARSING,
};

TaskHandle_t modbusTaskHandle;



void modbusTask(void * parameter)
{
    uint8_t modbusState = IDLE;

    while(1)
    {
        switch(modbusState)
        {
            case IDLE:
            {
                //if there is something in the queue load the corresponding packet
                if(mc.r.messageQueueDepth) modbusState = SENDING;
            }
            break;

            case SENDING:
            {
                //send message
                mc.master.query(mc.r.messageQueue[0]);

                //change to receiving
                modbusState = RECEIVING;
            }
            break;

            case RECEIVING:
            {
                //wait for message until timeout or error
                int8_t modbusAnswerCode = mc.master.poll();

                //the sent packet was a command, not expecting payload back
                //can confirm by just looking at function code of [0] element if necessary
                if(modbusAnswerCode == 0) 
                {
                    //shift queue
                    mc.shiftQueue();

                    //send to IDLE
                    modbusState = IDLE;
                }
                
                //got answer back and it is not an error
                if(modbusAnswerCode > 6) modbusState = PARSING;

                //got an error
                else
                {
                    //if error do something depending on type of error? !!!
                    switch(modbusAnswerCode)
                    {
                        //info terminal message

                        //do something! move on
                        case NO_REPLY:
                            break;
                        case ERR_BUFF_OVERFLOW:
                            break;
                        case ERR_EXCEPTION:
                            break;

                        case EXC_FUNC_CODE:
                            break;
                        case EXC_ADDR_RANGE:
                            break;


                    }
                }
            }
            break;

            case PARSING:
            {
                //expected a reply, so parse it depending on the function
                switch(mc.r.messageQueue[0].u8fct)
                {
                    case MB_FC_READ_COILS:
                        break;
                    case MB_FC_READ_DISCRETE_INPUT:
                        break;
                    case MB_FC_READ_REGISTERS:
                        break;
                    case MB_FC_READ_INPUT_REGISTER:
                        break;
                }

                //shift queue up
                mc.shiftQueue();

                //back to IDLE
                modbusState = IDLE;
            }
            break;


        }
    }
};

bool multiController::digitalWrite(uint8_t pin, bool onOff)
{
    modbus_t message;

    message.u8id         = this->s.address;     // slave address
    message.u8fct        = MB_FC_WRITE_COIL;    // function code (this one is single coil write)
    message.u16RegAdd    = pin;                 // start address in slave
    message.u16CoilsNo   = 1;                   // number of elements (coils or registers) to write
    this->r.tempData[0]  = onOff;               // target status
    message.au16reg      = this->r.tempData;    // pointer to a memory array in the Arduino
    
    enqueueMessage(message);
}

bool multiController::digitalRead(uint8_t pin)
{
    modbus_t message;

    message.u8id        = this->s.address;              // slave address
    message.u8fct       = MB_FC_READ_DISCRETE_INPUT;    // function code (this one is digital inputs read)
    message.u16RegAdd   = 0;                            // start address in slave
    message.u16CoilsNo  = MAX_MODBUS_DIGITAL_INPUTS;   // number of elements (coils or registers) to read
    message.au16reg     = this->r.tempData;             // pointer to a memory array in the Arduino

    enqueueMessage(message);
}

uint16_t multiController::analogRead(uint8_t pin)
{
    modbus_t message;
   
    message.u8id       = this->s.address;             // slave address
    message.u8fct      = MB_FC_READ_INPUT_REGISTER;   // function code (this one is input registers read)
    message.u16RegAdd  = pin;                         // start address in slave
    message.u16CoilsNo = 1;                           // number of elements (coils or registers) to read
    message.au16reg    = this->r.tempData;            // pointer to a memory array in the Arduino
    
    enqueueMessage(message);
}



void multiController::analogWrite(uint8_t pin, uint16_t value)
{
    modbus_t message;
   
    message.u8id         = this->s.address;           // slave address
    message.u8fct        = MB_FC_WRITE_REGISTER;      // function code (this one is input registers read)
    message.u16RegAdd    = pin;                       // start address in slave
    message.u16CoilsNo   = 1;                         // number of elements (coils or registers) to read
    this->r.tempData[0]  = value;                     // target status
    message.au16reg      = this->r.tempData;          // pointer to a memory array in the Arduino
    
    enqueueMessage(message);
}

void multiController::busPower(uint8_t bus, bool onOff)
{
    switch(bus)
    {
        case BUS_5V:
            this->digitalWrite(EN_5V, onOff);
            break;
        case BUS_ISO:
            this->digitalWrite(EN_ISO_PWR, onOff);
            break;
        default:
            break;
    }
}



void multiController::setSlaveAddress(uint8_t address)
{
    this->analogWrite(ADDRESS, address);
}




bool multiController::reset(void)
{
    this->digitalWrite(RESET, HIGH);
}

bool multiController::enterBootloader(void)
{

}

bool multiController::isUp(void)
{

}




multiController::multiController(HardwareSerial serial, gpio_num_t rx, gpio_num_t tx, gpio_num_t de, gpio_num_t re, uint16_t baudrate)
{
    //configure RE and DE
    this->de = de;
    this->re = re;

    pinMode(this->de, OUTPUT);
    pinMode(this->re, OUTPUT);
    digitalWrite(this->de, LOW);
    digitalWrite(this->re, LOW);

    //configure RX and TX
    this->rx = rx;
    this->tx = tx;

    //configure serial
    this->serial = serial;
    this->serial.begin(this->baudrate, SERIAL_8N1, this->rx, this->tx, false, 20000);

    //start modbus master and set timeout
    this->master.start();
    this->master.setTimeOut(MODBUS_TIMEOUT); 
}

void multiController::enqueueMessage(modbus_t message)
{
    this->r.messageQueue[this->r.messageQueueDepth++] = message; //make a copy of "pointer" !!!

    if(this->r.messageQueueDepth >= MAX_MODBUS_MESSAGES)
    {
        //signal buffer overflow ? !!!

        //reset index
        this->r.messageQueueDepth = 0;
    }
}

void multiController::shiftQueue(void)
{
    //move all elements up one position
    for(int i=0; i<this->r.messageQueueDepth-1; i++)
    {
        this->r.messageQueue[i] = this->r.messageQueue[i+1];
    }

    //decrease depth 
    if(this->r.messageQueueDepth > 0) this->r.messageQueueDepth--;

    //purge last message if it is the only one
    //if(this->r.messageQueueDepth == 0) this->r.messageQueueDepth[0] = {};
}

void multiController::handleError(uint8_t errorSource)
{

}


void multiController::masterPower(bool onOff)
{
    
}

bool multiController::alert(void)
{
    
}

bool multiController::discoverSlaves(void)
{
    
}