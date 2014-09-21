# SNESoIP firmware #

## Compiling ##

To compile, simply type:
```
$ make
$ make flash
```

## Command-line/config interface ##

To boot into the command-line interface (if enabled), hold B + Y at
startup.

The following commands are available (self-explanatory):
```
$ quit
$ wipe
$ mymac de:ad:AF:FE:23:42
$ gwip 192.168.0.1
$ netmask 255.255.255.0
$ username johndoe
$ password pleasedonttellanyone
$ key hh4sa,s4$5
$ sourceport 51233
$ serverport 51234
```

If not set (0.0.0.0 or 255.255.255.255) the DHCP is used:
```
$ myip 192.168.0.3
```

Both examples are working:
```
$ serverhost snesoip.de
$ serverhost 85.214.247.157
```
