# -*-makefile-*-


# Basic configuration.
BAUD    = 57600UL
CLI     = 1
DEBUG   = 1
PORT    = /dev/ttyUSB0


# Nothing below this line need to be changed.
F_CPU   = 16000000UL
MCU     = atmega168
OBJ     = src/cli.o \
          src/duoled.o \
          src/firmware.o \
          src/network.o \
          src/snesio.o \
          src/trivium.o \
          src/uart.o \
          src/net/dhcp_client.o \
          src/net/dnslkup.o \
          src/net/enc28j60.o \
          src/net/ip_arp_udp_tcp.o
PRG     = firmware
