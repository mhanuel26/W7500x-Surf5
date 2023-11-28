**Please refer to [getting_started.md](https://github.com/Wiznet/W7500x-Surf5/blob/main/getting_started.md) for examples usage.**



<!--
Link
-->

# W7500x-Surf5


Surf 5 is a library developed based on the W7500x_StdPeriph_Lib, designed for use with the W7500 MCU. 
For detailed information about W7500x_StdPeriph_Lib, please refer to the [README.md](https://github.com/Wiznet/W7500x_StdPeriph_Lib/blob/master/README.md) button.




## Supported devices and toolchains

### Supported Surf5 devices

The W7500x-Surf5 Library supports [Surf 5](https://docs.wiznet.io/Product/Open-Source-Hardware/surf5) devices.

### Supported toolchains

The Standard Peripherals Library supports the following toolchains:
- MDK-ARM (KEIL 5)
- GNU MCU (Eclipse)
- GNU Arm Embbeded Toolchain(VS Code)



## How to use the Library

### Create a project

Create a project in various toolchains. 

If you want to create a project in the Keil 5 environment, use the template project under Project\W7500x_StdPeriph_Templates within the library. 

Alternatively, if you want to create a project in the VS Code environment, use the template project under Project\W7500x_StdPeriph_Examples within the library.

### Configure w7500x.h

The Library entry point is w7500x.h (under Libraries\CMSIS\Device\WIZnet\W7500\Include), user has to include it in the application main and configure it:


- Select the target system clock source to be used, comment/uncomment the right define:
  ```
  /**
   * @brief In the following line adjust the value of External oscillator clock (8MHz ~ 24MHz)
   used in your application

   Tip: If you want to use the Internal 8MHz RC oscillator clock, uncomment the line below.
   */
  //#define OCLK_VALUE 12000000UL
  ```
- Change the PLL value to be used.
  ```
  /**
   * @brief In the following line adjust the value of PLL
   */
  #define PLL_VALUE 1
  ```
  
### Add the system_w7500x.c

Add the system_w7500x.c (under Libraries\CMSIS\Device\WIZnet\W7500\Source) file in your application, this file provide functions to setup the W7500x system.




## How to use the Examples

### Copy and Paste

Copy all source files from under Projects\W7500x_StdPeriph_Examples\xxx\xxx folder to the under src folder.
  



## Revision History

### v1.0.0
- First release

### v1.0.1 (Release soon)
- Add i2c example using GPIO
- Fix some bugs for GPIO function
- Update GPIO Interrupt examples
