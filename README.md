# Nosh Tasks 
Assignment by Nosh

## Task 1
Required Blinking of LED. The Time period of blinking changed based on how many clicks ahead we were from boot. 
A variable "mode" keep track of the count of the number of clicks. 
The Button is kept as an interrupt, so that, as soon as the button is clicked the time preiod of the LED blink is changed immediately.
A switch case based map is stored in program, which dictates, which click has how much time to wait.

In main Loop we just Toggle LED according to the global "led_delay_ms" variable, which can be changed in the interrupt.
After the CPU toggles the LED ON or OFF, it goes into sleep mode relying on the API present in the SDK "HAL_PWR_EnterSLEEPMode"
This would potentailly save a lot of power 

![image](https://github.com/user-attachments/assets/1fbb891c-dacb-4cfa-8ec0-023ce3ec190c)  
CubeMX pin config 

PC13 used by External button with external pull up
PA5 used as the onboard led pin

Note: Nucleo-L476RG is used instead of the mentioned NUCLEO-G070RB board, because of hardware availability.

## Task 2
Required Generating random number between 0 to 5 every 1 second and adding it to a global structure.
This was perform in a seperated called "sensor_simulator"
We have a different thread which wakes up every 10 seconds to check if the buffer has 50 bytes and prints the latest 50 bytes in hex.

A C program is given that can also be tested on any online c compiler

