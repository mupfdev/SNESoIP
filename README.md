# SNESoIP: The SNES ethernet adapter #

![SNESoIP prototype](Archive/Rev2/hardware/images/rev02-small.jpg?raw=true)

For images in higher resolution click [here](Archive/Rev2/hardware/images/).

## Introduction

The SNESoIP (SNES over IP) is an ethernet adapter for the Super Nintendo
Entertainment System which is also known as Super NES, SNES or Super
Nintendo.  It is basically an open-source, proof-of-concept, network
adapter to connect the SNES to the internet.

I'm not planning to build and/or sell these devices but the device
should be fairly easy to recreate.  Especially because I try to use
parts that are cheap and widely available.

I also highly recommend to use this project only in combination with a
flash cartridge like the [sd2snes](http://sd2snes.de/blog/) or the Super
Everdrive.

If you want to help with the further development of this project, join
us on our [Discord server](https://discord.gg/araBwyp).

## Annotation

It's not the project's main goal to support existing SNES games.  It
rather provides the technology that can be used to develop custom
homebrew applications such as multi-user dungeons, bulletin board
systems, etc.

## Project History

The SNESoIP is, so to speak, my never-ending long-term project, which I
started on 29 July 2013 and have taken up again and again since then.

The last time I put the project down was shortly before the birth of my
son.  The following years were strenuous; the new role as family father,
the death of my best friend [Ertugrul
Söylemez](https://github.com/esoeylemez), and frankly, I never intended
to resume the project at all.  Nevertheless, the project had a big
impact on my life and was, in a manner of speaking, the foundation for
my career as a software developer for embedded systems.

As you can imagine, this project is personal. And I always had the urge
to finish what I started almost six years ago.

We'll see where 2019 is going.

### Press and Media

[2013-10-03: SystemLinks (Archive)](https://web.archive.org/web/20160714125922/http://www.systemlinks.net/blog/snesoip-xband-of-21st-century/)  
[2013-09-28: POP (Archive)](https://web.archive.org/web/20131001152616/http://www.pop.com.br/games/wii/Jogue-online-no-seu-Super-Nintendo-com-o-SNESoIP-1001612.html)  
[2013-09-27: RetroNews](http://www.retroplayers.com.br/noticias/retronews-snesoip-a-volta-da-jogatina-online-ao-super-nintendo/)  
[2013-09-27: Retro Players](http://www.retroplayers.com.br/noticias/retronews-snesoip-a-volta-da-jogatina-online-ao-super-nintendo/)  
[2013-09-24: Cubed3](http://www.cubed3.com/news/19635/1/play-original-snes-games-online.html)  
[2013-09-24: Nintendo Life](https://www.nintendolife.com/news/2013/09/online_multiplayer_comes_to_the_super_nintendo_thanks_to_snesoip)  
[2013-09-23: Jeux vidéo (Archive)](https://web.archive.org/web/20140209130208/http://www.jvn.com/actualites/snesoip-le-modem-pour-super-nintendo-a1070900)  
[2013-09-23: Kotaku Australia](https://www.kotaku.com.au/2013/09/you-can-play-your-original-snes-online-dont-you-know/)  
[2013-09-20: Motherboard (Archive)](https://web.archive.org/web/20160514015628/https://motherboard.vice.com/blog/play-your-super-nintendo-online-thanks-to-this-open-source-hardware)  
[2013-09-18: RetroCollect](http://www.retrocollect.com/News/snesoip-brings-online-multiplayer-to-the-super-nintendo.html)  
[2013-09-18: Retro Game Network](http://www.retrogamenetwork.com/2013/09/18/snesoip-super-nintendo-entertainment-system-over-internet-protocol-takes-snes-games-online/)  
[2013-09-18: Retro Nintendo (Archive)](https://web.archive.org/web/20171027161636/http://www.retronintendo.be/2013/09/18/snesoip-online-multiplayer-voor-de-super-nintendo/)  
[2013-09-17: geekimpact.de (Archive)](https://web.archive.org/web/20131205052338/http://geekimpact.de/tech/snesoip-online-multiplayer-fur-das-super-nintendo/)  
[2013-09-14: Hackaday](https://hackaday.com/2013/09/14/snesoip-its-exactly-what-it-sounds-like/)  
[2013-09-03: Hackalizer](http://hackalizer.com/snesoip-puts-controller-net/)  
[2013-04-10: pantalytron (Archive)](https://web.archive.org/web/20160717075259/http://www.pantalytron.com/blog7.php/i-am-the-chosen-one)  

## Hardware

The new hardware design will most-likely be based on a ESP32 or ESP8266
microcontroller by Espressif Systems and will therefore use Wi-Fi.

## Licence and Credits

This project is licenced under the "THE BEER-WARE LICENCE".  See the
file [LICENCE.md](LICENCE.md) for details.

"Nintendo" is a registered trademark of Nintendo of America Inc.
