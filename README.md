# Arduino
Some Arduino sketches for my random projects

The Mask

The Mask and Mask Controller are for my Halloween costume based on the Adafruit Roboface project, and the costume project found here https://www.youtube.com/watch?v=8HZFY4Xti7k .

I didn't use the voice modulation part, which made things a lot easier and cut out a lot of electronics to haul around.  I ended up using 5 1.2" 8x8 LED matrices from Adafruit, and 2 Adafruit Feather nRF52 Bluefruit LE - nRF52832. One feather for the mask, and one for a 4 button controller for scrolling text or changing the face.  The mouth is sound reactive using a Electret Microphone Amplifier - MAX9814 with Auto Gain Control.  You might want one without autogain so it doesn't pick up smaller noises, but it's what I had around to use.  You can see in the code I adjusted for the autogain by adding a minRegister variable.  The basic code was pieced together by removing all the WavShield code from the demon mask, and parts of the Bluerfruit BLE examples.  Then I changed up the eyes a bit, added some scrolling "Happy Halloween" messages, etc.  The LCD matrices are wired directly to the USB pin for power to get 5v outside the 3v regulator on the Feather. This all lasted a little over 3 hours on a 2000 mAH 1A max USB charger, and that was with the brightness up to full.


The Mask Controller (Optional)

The remote mask controller is based off https://learn.adafruit.com/remote-control-with-the-huzzah-plus-adafruit-io/button-wiring and uses the other BLE Feather.  This is optional, and you can always use your phone with the Bluefruit Connect app to do it as well. The packetParser.cpp was modified a bit so that I could send text to scroll over the UART, but it still works with the app control pad. I just made a controller because the design of the mask and the gloves I'll be wearing make it harder to use/see my phone. A 500 mAH li-poly battery should last quite a while, and since it boots fast you can add an on/off switch to only turn it on when you need it.  

