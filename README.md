# SNESoIP: The SNES ethernet adapter #

![SNESoIP prototype](hardware/images/prototype-small.jpg?raw=true)


## Attention! ##

This branch is for testing/development purposes only and is most likely
incomplete.  If you're not a developer, check out the [master](https://github.com/mupfelofen-de/SNESoIP/tree/master) branch.


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

The easiest way to donate is via PayPal, simply click
[here](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=ESZJS7TMYMNNW&lc=GB&item_name=mupfelofen%2ede&item_number=SNESoIP&no_note=1&no_shipping=1&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted).

If you want to help in the development of this project, join us in
[#retrotardation](http://de.irc2go.com/webchat/?net=euIRC&room=retrotardation)
on [euIRC](http://www.euirc.net/en/).  We're also working on a similar
Gameboy Classic / Super Gameboy project.

And feel free to contribute to our
[development wiki](https://github.com/mupfelofen-de/SNESoIP/wiki). Any help is welcome!


## Features ##

- Plug and play (IP is obtained via DHCP),
- DNS lookup of the server hostname,
- small firmware size (fits on an ATmega8),
- easy to rebuild (even on a stripboard),
- [low component count](hardware/rev01/docs/partlist.md),
- adaption of other platforms (e.g. Sega Mega Drive) is possible,
- possible cross-platform capability.


## Todo ##
- Some kind of interface to avoid hard-coded configuration,
- detailed documentation.


## How it works ##

### Communication ###

The SNESoIP ethernet adapter is continously sending 4-byte data packets
via UDP to a (remote) server and receive 2-byte answer packets from the
(remote) server.

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
- Sciurus for his work on the web interface,
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
