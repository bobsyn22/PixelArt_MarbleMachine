# PixelArt_MarbleMachine
We created the a Marble Machine that can replicate any image as a 50x50 pixel art image using grey, black, and white marbles. This program is built using C and runs off a single MSP430 Microcontroller to control three stepper motors for marble sorting and selection.


![HongPixelArt](https://github.com/user-attachments/assets/86463b3a-6257-4938-a6cb-2f9dce86785c)


Firstly, an image is processed through an algorithm/program in MATLAB that greyscales and pixelates any colored image. This is done automatically; however, a GUI that displays each individual pixel of the simulated image to be printed was designed to allow the user to manually tweak an image if necessary. This GUI also has a "blank canvas" feature that allows you to "paint" in pixels and print custom drawings. 

Each pixel is then stored in a 2D array, representing the rows and columns of the pixelated image. 

On the hardware side, the Pixelator consists of a hopper, marble selector, and marble dispenser. Marbles first enter the system through the hopper, which is a massive tank that stores unsorted marbles. The hopper drives an agitator that feeds marbles into the selector, which consists of a vertical rotating gear with slots to hold individual marbles. These isolated marbles are then passed through a color sensor that uses an EV3 Lego Mindstorms colour sensor to detect a marble. Depending on the color, the marble is either accepted or rejected. If accepted, a hatch actuated by a solenoid opens on the bottom, feeding the marble through the dispenser and into the picture frame. If rejected, the hatch does not open, and the marble exits the selector via a slot located just after the hatch position.  
 
