# SNESoIP: The SNES ethernet adapter #

## Introduction ##

The SNESoIP ethernet adapter for the Super Nintendo Entertainment System
(also known as the Super NES, SNES or Super Nintendo) is an open-source,
proof-of-concept, network-bridge for sharing local controller-inputs
over the Internet.

The project it currently in its early stage, but fully functional and
could be used to play multiplayer games over the Internet.
Also some kind of server-controlled BBS system could be
conceivable as a future project.

SNESoIP is an open-source project developed completely in my spare time.
If you find it useful, please consider donating a small amount of money.
Your donation will be used to support further development of the
project.

The easiest way to donate is via PayPal, simply click the button below:

<form action="https://www.paypal.com/cgi-bin/webscr"
	method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="GDFBJDCHFGTCU">
<input type="image"
	src="https://www.paypalobjects.com/en_US/GB/i/btn/btn_donateCC_LG.gif"
	border="0" name="submit" alt="PayPal – The safer, easier way to pay
	online.">
<img alt="" border="0"
	src="https://www.paypalobjects.com/de_DE/i/scr/pixel.gif"
	width="1" height="1">
</form>


## Features ##

- Plug and play (IP is obtained via DHCP),
- DNS lookup of the server hostname,
- small firmware size (fits on an ATmega8),
- easy to rebuild (even on a stripboard),
- low component count,
- adaption of other platforms (e.g. Sega Mega Drive) is possible
- possible cross-platform capability.


## Todo ##
- Some kind of interface to avoid hard-coded configuration,
- detailed documentation.


## How it works ##

### Communication ###

The SNESoIP ethernet adapter is continously sending 4-byte data packets
via UDP to a (remote) server and may receive 2-byte answer packets from
the (remote) server.

A packet sent to the server has the following format:

	+--------+--------+--------+--------+
	| Controller Data |  CID   | P2CID  |
	+--------+--------+--------+--------+

An answer packet has the following format:

	+--------+--------+
	| Controller Data |
	+--------+--------+

The 2-byte `Controller Data` field holds the state of SNES controller
which is basically a 16 bit shift register of which 12 bits are being
used to represent button states (unused bits are pulled high).

`CID` is a value uniqely identifying the sending controller instance to
the server and used to store its state on the server.

`P2CID` is the `CID` of your selected "Player Two".  The server will
answer your packet with the controller data of the controller identified
by `P2CID`.

Update packets are being sent to the server continously even if the
local controller state does not change state.

### Switch Mode ###

To ease sorting out which player is to play as *player 1* and which is
to play as *player 2* and prevent tedious interchanging of plugs the
SNESoIP implements a "Switched mode" which exchanges player 1 and 2 in
software.

To activate "Switched mode" simply hold down the B and Y keys of your
controller while booting the SNESoIP. If successful, the SNESoIP's LED
will blink briefly.

To deactivate "Switch Mode", simply reboot the SNESoIP without any
buttons held down.


## Special thanks goes to ##

- [lytron](http://pantalytron.com) for testing the initial prototype,
- Farbauti for his code improvements,
- the [snesfreaks.com](http://snesfreaks.com) community for all the
  support and motivation and
- last but not least, to my mates at our local hackerspace,
  [shackspace](http://shackspace.de).  You guys rock!


## License ##

The whole project (except the
[IP stack](http://tuxgraphics.org/common/src2/article09051/ "The
tuxgraphics TCP/IP stack") by tuxgraphics.org) has has been released
under the terms of a BSD-like license.  See the file [LICENSE](LICENSE)
for details.

"Nintendo" is a registered trademark of Nintendo of America Inc.
