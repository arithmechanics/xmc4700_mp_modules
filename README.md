# xmc4700_mp_modules
Micropython modules for Infineon XMC4700

Install Micropython to XMC4700 board from https://github.com/micropython/micropython-infineon/tree/master/xmc


flashprotect: locks MCU's flash memory from reading. 

To use this module, add these lines to Makefile after SRC_S definition:

SRC_C += modflashprotect.c \
        xmclib/src/xmc4_scu.c \
        xmclib/src/xmc4_flash.c

SRC_MOD += modflashprotect.c
