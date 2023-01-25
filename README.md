# STM32-RS-485-modbus-slave-controller
Multipurpose RS-485 modbus slave controller firmware for water pump station control (but not limited to).
An STM32-based MODBUS-enabled controller firmware. 
Has 8 logic inputs, 8 logic outputs, 4 analog 4-20 mA outputs, 16 analog (by STM's ADC and ADC1015 chip) inputs.
Inputs and outputs are corresponded to dedicated modbus registers.
Fail-safe logic implemented. 
Programmable MODBUS address.
Built-it bootloader which can update the firmware via RS-485 interface.
ESP32 acts as a master and as a programmer when needed to update firmware.
