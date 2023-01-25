/**
 * @file multicontroller.h
 * @author Gabriel Ballesteros (gabriel@coratronics.com)
 * @brief This file declares all the function
 * @version 0.1
 * @date 2020-11-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <Arduino.h>
#include "../ModbusRtu.h"


#define MAX_ANALOG_INPUTS  8
#define MAX_VOLTAGE_INPUTS 8
#define MAX_CURRENT_INPUTS 4

#define MAX_ANALOG_OUTPUTS  4
#define MAX_VOLTAGE_OUTPUTS 4
#define MAX_CURRENT_OUTPUTS 4

#define MAX_MODBUS_MESSAGES 20
#define MAX_MODBUS_DIGITAL_INPUTS 32

#define MODBUS_TIMEOUT 1000

#define MAX_SLAVES 16

#define MODBUS_SERIAL   Serial2
#define RX              16
#define TX              17
#define DE              23
#define RE              22
#define MODBUS_BAUDRATE 19200

/**
 * \defgroup COILS 
 * @{
 */
enum
{
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    EN_5V,
    EN_ISO_PWR,
    LED1,
    LED2,
    RESET,
    POST_RESET
};
/**@}*/

/**
 * \defgroup DISCRETE_INPUTS 
 * @{
 */
enum
{
    DI1,
    DI2,
    DI3,
    DI4,
    DI5,
    DI6,
    DI7,
    DI8, 
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    EF1,
    EF2,
    EF3,
    EF4,
    ADC1F,
    ADC2F,
    DAC1F,
    DAC2F,
    DAC3F,
    DAC4F
};


/**
 * \defgroup HOLDING_REGISTERS 
 * @{
 */
enum
{
    DAC1,
    DAC2,
    DAC3,
    DAC4,
    ADDRESS,
    ADC1_V_CH1_G,
    ADC1_V_CH2_G,
    ADC1_V_CH3_G,
    ADC1_V_CH4_G,
    ADC2_V_CH1_G,
    ADC2_V_CH2_G,
    ADC2_V_CH3_G,
    ADC2_V_CH4_G,
    ADC1_I_CH1_G,
    ADC1_I_CH2_G,
    ADC2_I_CH1_G,
    ADC2_I_CH2_G
};
/**@}*/

/**
 * \defgroup INPUT_REGISTERS 
 * @{
 */
enum
{
    ADC1_V_CH1,
    ADC1_V_CH2,
    ADC1_V_CH3,
    ADC1_V_CH4,
    ADC2_V_CH1,
    ADC2_V_CH2,
    ADC2_V_CH3,
    ADC2_V_CH4,
    ADC1_I_CH1,
    ADC1_I_CH2,
    ADC2_I_CH1,
    ADC2_I_CH2,
    ADC_IN12,
    ADC_IN13,
    ADC_VREF_INT,
    TEMPSENSOR
};
/**@}*/

enum
{
    VOLTAGE,
    CURRENT
};

enum
{
    BUS_5V,
    BUS_ISO
};

typedef struct slave
{
    uint8_t digitalInputs;

    uint8_t digitalOutputs;

    uint16_t analogInputs[MAX_ANALOG_INPUTS];
    uint16_t currentInputs[MAX_CURRENT_INPUTS];
    uint16_t voltageInputs[MAX_VOLTAGE_INPUTS];

    uint16_t analogOutputs[MAX_ANALOG_OUTPUTS];
    uint16_t voltageOutputs[MAX_VOLTAGE_OUTPUTS];
    uint16_t currentOutputs[MAX_CURRENT_OUTPUTS];

}slave;

/**
 * Multi Controller class
 * 
 */
class multiController
{
    public:
        struct runtime
        {
            modbus_t messageQueue[MAX_MODBUS_MESSAGES];
            uint8_t messageQueueDepth;

            uint16_t tempData[16];

            bool masterPowerStatus;

            uint8_t slaveAddresses[MAX_SLAVES];
        }r;


        struct settings
        {
            uint8_t analogInputType[MAX_ANALOG_INPUTS];
            uint8_t analogOutputType[MAX_ANALOG_OUTPUTS];

            uint8_t analogInputEnable[MAX_ANALOG_INPUTS];
            uint8_t analogOutputEnable[MAX_ANALOG_OUTPUTS];

            uint8_t address;
        }s;

        //modbus class
        Modbus master;

        /**
         * @brief Construct a new multi Controller::multi Controller object
         * 
         * @param serial 
         * @param rx 
         * @param tx 
         * @param de 
         * @param re 
         * @param baudrate 
         */
        multiController(HardwareSerial serial, gpio_num_t rx, gpio_num_t tx, gpio_num_t de, gpio_num_t re, uint16_t baudrate);

        /**
         * @brief Controls the digital outputs (signal relays)
         * 
         * @param pin the target pin that is controlled
         * @param onOff ON for active OFF for inactive
         * @return true if successful
         * @return false if the coil failed
         */
        bool digitalWrite(uint8_t pin, bool onOff);

        /**
         * @brief Reads a digital input
         * 
         * @param pin input pin that is read
         * @return true if HIGH
         * @return false if LOW
         */
        bool digitalRead(uint8_t pin);

        /**
         * @brief Reads an analog input
         * 
         * @param pin the pin to be read
         * @return uint16_t the value in bits
         */
        uint16_t analogRead(uint8_t pin);

        /**
         * @brief Controls the output value of an analog pin
         * 
         * @param pin target output pin
         * @param value the value to be written
         */
        void analogWrite(uint8_t pin, uint16_t value);

        /**
         * @brief Enables or disables the power buses on the multicontroller
         * 
         * @param bus target bus to be controlled
         * @param onOff target status ON active, OFF inactive
         */
        void busPower(uint8_t bus, bool onOff);

        /**
         * @brief Sets the address of the multicontroller
         * 
         * @param address the desired address
         */
        void setSlaveAddress(uint8_t address);

        /**
         * @brief Tells the multicontroller to place itself in bootloading mode
         * 
         * @return true if it was successful
         * @return false if failed
         */
        bool enterBootloader(void);

        /**
         * @brief Resets the multronctroller
         * 
         * @return true if succesful
         * @return false if failed
         */
        bool reset(void);

        /**
         * @brief Checks if the multicontroller is present
         * 
         * @return true if the multicontroller was detected on the RS485 bus
         * @return false if the multicontroller was not detect
         */
        bool isUp(void);

        /**
         * @brief Controls PoE fed to the multicontroller
         * 
         * @param onOff ON active, OFF inactive
         */
        void masterPower(bool onOff);
        
        /**
         * @brief Checks if the multicontroller has a pending message by reading the interrupt pin
         * 
         * @return true if there is a pending message
         * @return false if there is no pending message
         */
        bool alert(void);

        /**
         * @brief Discovers slaves on the RS485 bus
         * 
         * @return true 
         * @return false 
         */
        bool discoverSlaves(void);


        /**
         * @brief Adds a message to the message queue
         * 
         * @param message the message in question
         */
        void enqueueMessage(modbus_t message);


        /**
         * @brief Shift queue up one position
         * 
         */
        void multiController::shiftQueue(void);

    private:
        //comm, pins, baudrate, etc
        gpio_num_t re;
        gpio_num_t de;
        gpio_num_t tx;
        gpio_num_t rx;
        HardwareSerial serial;
        uint16_t baudrate;

        
        /**
         * @brief Takes the appropriate action if a transaction returned an error
         * 
         * @param errorSource the transaction type that returned an error
         */
        void handleError(uint8_t errorSource);
};

extern multiController mc;