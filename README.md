# Modular-RGB-LED-Sign

This repo holds code for modular 3D printed RGD LED signs as shown in this video: 
https://www.youtube.com/watch?v=QHCL2vn8N_w&t=7s 

Code is based on the FastLED library and following Encoder library: https://github.com/PaulStoffregen/Encoder  

Video above describes how to install and configure the code to fit your specific sign. 
Wiring.png shows parts required (encoder is optional really).

**Code description:**

**RGB_LED_Sign_001:**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Main code as used in video, with encoder and modes.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 0 (startup): Toggles the animations (rainbow, rainbowWithGlitter, confetti, sinelon, juggle and bpm) at 10 sec each  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 1: rainbow  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 2: rainbowWithGlitter  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 3: confetti  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 4: sinelon  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 5: juggle  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 6: bpm  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Mode 7: freeColor (lets user set color for each character/symbol) 

**RGB_LED_Sign_003:**  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Added Mode 8: freeColorLeds (lets user set color for each led)
  
  **BasicStatic:**   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Simple code example to set static colors only, no encoder.
