# shroom-puter
A climate controller for a peltier cooled fruting chamber for mushrooms and other cultures.

This is a personal project so it is not as polished as it could be.

It is based on a Nucleo STM32 Nucleo L031-K6 development board (RS Components part# 9173781)  Other suppliers can be found on [octopart](https://octopart.com/search?q=NUCLEO-L031)

The temperature and humidity sensor is an SHT10.  I found that only the soil temperature and humidity sensors would handle the high humidity environment present inside the chamber.  I got mine on ebay, similar to this one: [Link to eBay page](http://www.ebay.com.au/itm/1-PCS-Temperature-Humidity-Sensor-Soil-sensor-with-SHT10-Sensor-Probe-NEW-/222600539398?hash=item33d406ad06:g:CmQAAOSwuxpZbxKu)

The LCD is a Nokia 3310/5510 on a breakout board similar to this one: [Link to eBay page](http://www.ebay.com.au/itm/1PCS-84-48-84x48-LCD-Module-White-Backlight-Adapter-PCB-for-Nokia-5110-Arduino-/152131918753?epid=619794453&hash=item236bc4dba1:g:VhwAAOSwbYZXYmrM)

The PCB layout is done in Eagle.  I have a newer version in KiCad in the works, but the firmware has not yet been altered to work with the new design.

The sofware is developed with System Workbench for STM32 (sw4stm32) you can use the on-board debugger on the Nucleo to program the firmware.
