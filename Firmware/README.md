# SNESoIP Firmware

SNESoIP firmware.

[![Build status](https://travis-ci.org/mupfelofen-de/SNESoIP.svg?branch=master)](https://travis-ci.org/mupfelofen-de/SNESoIP)

## Installation

1. Install [PlatformIO Core](http://docs.platformio.org/page/core.html)
2. Run these commands:

```bash
    # Build project
    > platformio run

    # Upload firmware
    > platformio run --target upload

    # Clean build files (optional)
    > platformio run --target clean
```

## WiFi configuration

To configure your WiFi connection, just use EspTouch for
[Android](https://github.com/EspressifApp/EsptouchForAndroid/archive/v0.3.7.1.zip)
or
[iOS](https://github.com/EspressifApp/EsptouchForIOS/archive/v0.3.7.0.zip).

The source code of these apps is also available on GitHub:

[EsptouchForAndroid](https://github.com/EspressifApp/EsptouchForAndroid)  
[EsptouchForIOS](https://github.com/EspressifApp/EsptouchForIOS)  
