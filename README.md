# Slick Rick's F1 Tracker
OpenF1 project to help track the F1 season

This project uses the OpenF1 api to gather information from the previous race and then displays that information onto the Cheap Yellow Display(CYD). 

The hardware current setup for the device is a Cheap Yellow Display with a built in ESP 32. My goal is to eventually build a 3d printed enclosure for the screen. 

Current Status of the project: Working in Progress (The project is functional at the current state it is in but there are still bugs)

Features:
- The esp32 connects to the OpenF1 to gather the information
- A boot screen at the first loading the esp32
- The device tracks the winners of the Lastest Race as well as the rest of the top five and then displays the results onto the screen. (With color coordinated with the corresponding position in the leaderboard)
- The next screen is display the Race circuit information, such as (circuit, country, and the current session key). 
- The final screen is displaying the driver's fastest lap during the race. 
- The screen in the current state will change on each touch after the inital boot screen.
- The CYD cycles through each screen by being pressed triggering the next screen.

Future Goals:
- Better circuit graphics
- Improved touchscreen navigation
- More detailed race information
- Weather information
- Lap timing information
- Additional driver statistics
- Improved UI animations
- More efficient memory usage

Software:
- Arduino IDE
- C++
- TFT_eSPI
- XPT2046_Touchscreen
- ArduinoJson
- HTTPClient
- OpenF1 API

Api Information: 
- https://api.openf1.org/v1


