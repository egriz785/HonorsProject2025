/* A LCD Character Display applications
* Written by Derek Molloy for the book "Exploring BeagleBone: Tools and 
* Techniques for Building with Embedded Linux" by John Wiley & Sons, 2014
* ISBN 9781118935125. Please see the file README.md in the repository root 
* directory for copyright and GNU GPLv3 license information.

Updated by Ethan Griswold and Jeremy Burke
April 2025
*/

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <cmath>
#include "display/LCDCharacterDisplay.h"

// scroll speed, time between character shifts
#define delay_time 500000

using namespace std;
using namespace exploringBB;

int main(int argc, char* argv[]){

  // Not enough arguments
  if(argc != 3) {
    cout << "Program takes 2 arguments." << endl;
    cout << "Example invocation:" << endl;
    cout << "LCDScroll str1 str2" << endl;
    return -1;
  }

  SPIDevice *busDevice = new SPIDevice(0,0);     //Using second SPI0.0
  busDevice->setSpeed(1000000);                  // Have access to SPI Device object
  ostringstream s;                               // Using this to combine text and int data
  LCDCharacterDisplay display(busDevice, 16, 2); // Construct 16x2 LCD Display
  display.clear();                               // Clear the character LCD module
  display.home();                                // Move the cursor to the (0,0) position
   
  // get the command line arguments and save them as strings
  string top(argv[1]);
  string bottom(argv[2]);

  // get the length of both strings
  int top_len = top.length();
  int bot_len = bottom.length();

  // pad the shorter string

  // if the top string is longer,
  // pad the bottom string
  if(top_len > bot_len) {
    // make a stringstream to load up
    stringstream temp;
    
    // add spaces to the beginning of the string
    for(int i = 0; i < ceil((float)(top_len-bot_len)/2); i++) {
      temp << " ";
    }
    
    // add the actual text
    temp << bottom;
    
    // add spaces to end of string
    for(int i = 0; i < floor((float)(top_len-bot_len)/2); i++) {
      temp << " ";
    }
    
    // set the new value
    bottom = temp.str();

  }

  // if the bottom string is longer,
  // pad the top string
  else if(top_len < bot_len) {
    // make a stringstream to load up
    stringstream temp;
    
    // add spaces to the beginning of the string
    for(int i = 0; i < ceil((float)(bot_len-top_len)/2); i++) {
      temp << " ";
    }
    
    // add the actual text
    temp << top;

    // add spaces to end of string
    for(int i = 0; i < floor((float)(bot_len-top_len)/2); i++) {
      temp << " ";
    }

    // set the new value
    top = temp.str();
  }

  // save the longer string's length in len
  int len = (top_len > bot_len) ? top_len : bot_len;

  // shut off the cursor (underlined segment) and the blinking (white segment)
  display.setCursorOff(true);
  display.setCursorBlink(false);
  
  while(1) {
    for(int i = -len; i < 16; i++) {
      display.clear();

      // no strings are on the screen yet
      if(i == -len) {
        // don't print anything
        continue;
      }

      // the end of the string should be on the screen
      else if (i > -len && i < 0) {
        // print top line
        display.setCursorPosition(0,0);
        // only show part of the string
        display.print(top.substr(-i,len));
        // print bottom line
        display.setCursorPosition(1,0);
        // only show part of the screen
        display.print(bottom.substr(-i,len));
      }

      // either the entire string should be on screen
      // or the end goes off screen, but doesn't need extra logic
      else {
        // print top line
        display.setCursorPosition(0,i);
        display.print(top);
        // print bottom line
        display.setCursorPosition(1,i);
        display.print(bottom);
      }

      usleep(delay_time); // half second sleep
    }
  }

return 0;
}
