FBoot-Linux
===========

Linux tool for fastboot AVR Bootloader, see http://www.mikrocontroller.net/articles/AVR_Bootloader_FastBoot_von_Peter_Dannegger

Compile with 'make', run like:
<pre>
./bootloader -d /dev/serial/by-id/usb-FTDI_FT232R_* -b 115200 -P Peda -p ../../IRtest/IR.hex

=================================================
|           BOOTLOADER, Target: V2.1            |
|            (Dec  7 2012 20:29:06)             |
=================================================
Now program device.
Port          : /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A1013SRB-if00-port0
Baudrate      : 115200
File          : ../../IRtest/IR.hex
Reading       : ../../IRtest/IR.hex... File read.
Size          : 2216 Bytes
-------------------------------------------------
Waiting for device... connected!
Bootloader    : V2.1
Target        : 1E930A ATmega88
Buffer        : 960 Byte
Size available: 7680 Byte
CRC enabled and OK.
Programming   : 0x00000 - 0x008A7
Writing [###################################################################] 100%
Elapsed time  : 0.44 seconds, 5034 Bytes/sec.

 ++++++++++ Device successfully programmed! ++++++++++

...starting application
</pre>

Known options are:
<pre>
bootloader [-d /dev/ttyS0] [-b 9600] -[v|p] file.hex
-d /dev/ttynn       serial device, (use e.g. /dev/serial/by-id/usb-FTDI* for FT232)
-b nn               Baudrate
-t nn               TxD Blocksize (i.e. number of bytes written in one block); USB serial
                    adaptors for example perform best if they can transfer a block of
                    characters at a time
-w nn               do not use tcdrain, wait nn times byte transmission time instead.
                    Normally tcdrain is used to wait until all bytes have been transferred,
                    with some serial adaptors (bluetooth?) this does not work; then waiting
                    can be used (for cost of performance)
-r                  switch reset off, DTR will not be changed
-R (default)        toggle DTR to reset device: DTR will toggle during sending of password
                    until connection is established (i.e. like Arduino)
-v                  Verify flash
-p                  Program flash
-e                  Erase, use together with -p to erase controller,
                    with -v to check if it is erased
-H                  Hide progress bar. Useful to use within Emacs.
-P pwd              Password that is set in the AVR. Since the bootloader prepends 0x0d to
                    the password for autobaud, it is not necessary for the password to contain
                    an autobaud character like 'a'. So there might be used arbitrary characters
                    for the 4 password characters.
-T                  enter terminal mode
</pre>
