# Reef-ATO-and-Dosing
4-channel pump controller with float switch inputs for automatic top-off and chemical additions to aquariums

Custom PCBA using an Arduino Nano module
User input through a I2C encoder with pushbutton module: 
User output through a SPI 4x20 OLED display module: 
4 n-channel mosfet outputs for 12 or 24VDC peristaltic pumps
Aquarium inputs are three float switches (Max, High, and Low)
  Max float electrically disables the mosfet outputs
  High signals water is needed and fed by CH-1
  Low is for an alarm (unimplemented)
