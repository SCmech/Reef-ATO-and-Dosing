# Reef-ATO-and-Dosing
## 4-channel pump controller with float switch inputs for automatic top-off and chemical additions to aquariums

Custom PCBA using an Arduino Nano Every module 

User input through a I2C encoder with pushbutton module: [Duppa i2c encoder on Tindie](https://www.tindie.com/products/saimon/i2c-encoder-connect-rotary-encoders-on-i2c-bus/#:~:text=The%20I2C%20Encoder%20is%20a%20small%20board%20where%20you%20can,with%20the%20same%20I2C%20bu)

User output through a SPI 4x20 OLED display module: Newhaven NHD-0420CW-AB3

Hardware consists of:

4 n-channel mosfet outputs for 12 or 24VDC peristaltic pumps.

Aquarium inputs are three float switches (Max, High, and Low).

* Max float electrically disables the mosfet outputs
* High signals water is needed and fed by CH-1
* Low is for an alarm (unimplemented)
