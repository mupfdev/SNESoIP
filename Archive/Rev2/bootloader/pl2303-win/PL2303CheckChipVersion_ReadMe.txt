
==================================================
PL-2303 CheckChipVersion Tool Program v1.0.0.6
For Windows OS Only
==================================================

System Requirement:
  . USB 1.1/2.0/3.0 Host Controller
  . Device using PL2303HX/X/EA/RA/SA/TA/TB version chips
  . PL-2303 Windows Driver Installer v1.8.0
	- Windows 2000/XP/Server2003 (32 & 64-bit) WDM WHQL Driver: v2.1.44.226
	- Windows Vista/7/Server2008 (32 & 64-bit) WDF WHQL Driver: v3.4.48.272
  . Supports the following Windows OS Family:
	- Windows XP SP2 and above (32 & 64 bit)
	- Windows Vista (32 & 64 bit)
	- Windows 7 (32 & 64 bit)
	- Windows 8/8.1 (32 & 64 bit) 
	NOTE: 
	  - Windows 8 is NOT supported in HXA/XA chip versions (will show yellow mark Error Code 10).
	  - Windows 8 is supported only in following chip versions:
	      - PL2303HXD (HX Rev D)
	      - PL2303EA (ESD protection)
	      - PL2303RA (Built-in transceiver)
	      - PL2303SA (SOP8 package)
	      - PL2303TA (PL2303HXA/XA replacement)
	      - PL2303TB (12 GPIO)
 
Supported device ID and product strings:
  . VID_067B&PID_2303 for "Prolific USB-to-Serial Comm Port"
  . VID_067B&PID_2304 for "Prolific USB-to-Serial Comm Port" (PL2303TB chip)
  . Other VID/PID based on PL2303 chip. 


Release Notes:
---------------
PL2303CheckChipVersion v1.0.0.6
 	. Do not lock VID/PID. 


===========================
How to Run Tool Program
===========================
1. Install PL-2303 Windows Driver Installer v1.8.0 or above.
2. Plug PL2303 USB Device and go to Device Manager to check COM Port number. 
3. Run PL2303CheckChipVersion tool and set COM Port number. 
4. Click Check button to show PL-2303 chip version. 


Download Latest Driver here: 
http://www.prolific.com.tw/US/ShowProduct.aspx?p_id=225&pcid=41

========================================
Prolific Technology Inc.
http://www.prolific.com.tw


