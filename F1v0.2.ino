// Libraries needed for the project
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
// Custom folder that has circuit information
#include "tracks/tracks.h"

//WiFi Credentials
const char* ssid = "Your WiFi ";
const char* password = "Your WiFi Password";

// Create the CYD TFT display object
TFT_eSPI tft = TFT_eSPI();

// Screen Dimension
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Header
#define HEADER_Y 0
#define HEADER_H 40

// Main Panels
#define MAIN_X 8
#define MAIN_Y 48
#define MAIN_W 204
#define MAIN_H 180

// Side Panel
#define SIDE_X 220
#define SIDE_Y 92
// Right panels
#define SIDE_W 92

// Session Panel
#define SESSION_Y 48
#define SESSION_H 78

// Weather Panel
#define WEATHER_Y 136
#define WEATHER_H 92

//Color's for the screen display but unfortunaley for this project they are inverted
#define ui_black TFT_BLACK   // White going to swap eventually
#define ui_yellow TFT_RED   // Yellow
#define ui_navy TFT_NAVY  // Light Blue
#define ui_white TFT_WHITE // Black going to swap eventually
#define ui_purple TFT_DARKGREEN // Purple
#define ui_silver TFT_SILVER // Silver
#define ui_bronze TFT_GOLD // Bronse/ brown

//Weather panels
#define WEATHER_PANEL_X 220
#define WEATHER_PANEL_Y 125
#define WEATHER_PANEL_W 92
#define WEATHER_PANEL_H 88

// Touch Screen Pins
#define TOUCH_CS 33
#define TOUCH_IRQ 36
#define TOUCH_CLK 25
#define TOUCH_MISO 39
#define TOUCH_MOSI 32

// Create a separate SPI bus for the touchscreen.
SPIClass touchSPI(HSPI);

// Create the touchscreen object.
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// Global variables
const unsigned long UPDATE_INTERVAL = 600000; // 10 min timer
const unsigned long SCREEN_INTERVAL = 5000; // 5 second timer
unsigned long lastTouch = 0; 
unsigned long lastUpdate = 0; // Global timestamp tracking the last API request
unsigned long lastScreenChange = 0; // Global timestamp tracking the last display screen switch
int currentScreen = 0; // Global variable assigned to a current screen

// Based api url
const char* openF1= "https://api.openf1.org/v1";

String driverNames[5];// List storing driver's names
String driverPositions[5];// List storying drivers positions for the weekend
String driverNumbers[5];// List storying driver's number
String driverTimes[5]; // Fastest lap for each driver

//Tracks the drivers that programs counts
int driverCount = 0;

// Global Race Context variables
String currentSession = "UNKNOWN";
String currentRace = "UNKNOWN";
String currentCircuit = "UNKNOWN";
String currentCountry = "UNKNOWN";

int currentSessionKey = 0; // Unique internal integer identifier used to query target endpoints

int currentLap = 0; // Stores current racing track progression count

// Boot Screen
void showBootScreen() {

  // Clear the display
  tft.fillScreen(ui_white);

  // Show actual TFT dimensions
  Serial.print("Boot Screen Width: ");
  Serial.println(tft.width());

  Serial.print("Boot Screen Height: ");
  Serial.println(tft.height());

  // Yellow border around the active display area
  tft.drawRect(0,0,tft.width(),tft.height(),ui_yellow);

  // Main Boot text
  tft.setTextColor(TFT_MAROON);
  tft.setTextSize(2);
  tft.setCursor(70, 70);
  tft.println("TEXT YOU WOULD LIKE YOU PUT");

  tft.setTextColor(ui_black);
  tft.setCursor(95, 100);
  tft.println("F1 Tracker");

  tft.setTextSize(1);
  tft.setCursor(130, 140);
  tft.println("v0.1"); // Version of the tracker

  delay(1500);
}
// Draw UI panel boxes for sections of a screen mainly used in the Session information screen
void drawUIPanel(int x, int y, int w, int h){

  // Panel Backgroud set to violet/ light purple
  tft.fillRoundRect(x,y,w,h,6,TFT_DARKGREEN);


  //Panel Border/ black border
  tft.drawRoundRect( x,y,w,h,6,ui_white);
}
// Draws the header for the Previous race
void drawPreviousRaceHeader(){

  //Header background
  tft.fillRect( 10,0,SCREEN_WIDTH + 60 ,HEADER_H, TFT_DARKGREEN);

  // Bottom accent
  tft.fillRect(8,39,304,2,ui_yellow);

  //Back Button
  tft.fillRoundRect(8,7,38,28,5,TFT_DARKGREY);
  tft.drawRoundRect(8,7,38,28,5,ui_white);

  //Back Arrow just visual currently but will be a functional button in the future
  tft.setTextColor(ui_white);
  tft.setTextSize(2);

  tft.setCursor(19, 11);
  tft.print("<");

  //Header title
  tft.setTextColor(ui_white);
  tft.setTextSize(2);

  tft.setCursor(55, 11);
  tft.print("PREVIOUS RACE");
}
// Draws the circuit information panels using drawUIPanel()
void drawSessionPanel(){

  drawUIPanel(SIDE_X, SESSION_Y, SIDE_W, SESSION_H);

  // Title
  tft.setTextColor(ui_white);
  tft.setTextSize(1);
  tft.setCursor(SIDE_X + 10, SESSION_Y + 10);
  tft.print("SESSION");

  //Divder
  tft.drawFastHLine(SIDE_X + 10, SESSION_Y + 25, SIDE_W - 20, TFT_DARKGREY);

  // Key
  tft.setTextColor(ui_white);
  tft.setCursor(SIDE_X + 10, SESSION_Y + 35);
  tft.print("Key");
  //Session Key
  tft.setTextColor(ui_white);
  tft.setTextSize(2);
  tft.setCursor(SIDE_X + 10, SESSION_Y + 50);
  tft.print(currentSessionKey);
}
// Panel for the circuit itself
void drawCircuitPanel(){
  ///UI PANELS
  drawUIPanel(MAIN_X, MAIN_Y, MAIN_W, MAIN_H);

  //Prints the Circuit
  tft.setTextColor(ui_white);

  // Circuit Display
  tft.setTextSize(1);
  tft.setCursor(MAIN_X + 10, MAIN_Y +10);

  // Adds the circuit name
  tft.setTextColor(ui_white);
  tft.setTextSize(2);
  tft.setCursor(MAIN_X + 10, MAIN_Y + 25);
  tft.println(currentCircuit);

  // Divider
  tft.drawFastHLine( MAIN_X + 10, MAIN_Y + 48, MAIN_W - 20,TFT_DARKGREY);

  //Prints the Location
  tft.setTextColor(ui_white);
  tft.setTextSize(1);
  tft.setCursor( MAIN_X + 10, MAIN_Y + 58);
  tft.print("Location");

  // Adds country
  tft.setTextSize(2);
  tft.setCursor( MAIN_X + 10, MAIN_Y + 73);
  tft.println(currentCountry);
}
// Weather Panel
void drawWeatherPanel(){

  drawUIPanel(SIDE_X, WEATHER_Y, SIDE_W, WEATHER_H);

  tft.setTextColor(TFT_LIGHTGREY);
  tft.setTextSize(1);

  tft.setCursor(SIDE_X + 10, WEATHER_Y + 10);
  tft.print("Weather");

  //Divider
  tft.drawFastHLine(SIDE_X + 10, WEATHER_Y + 25, SIDE_W - 20, TFT_DARKGREY);

  //Place holder getWeather is not made yet
  tft.setTextColor(ui_white);
  tft.setTextSize(2);
  tft.setCursor(SIDE_X + 10, WEATHER_Y + 38);
  tft.print("--");

  tft.setTextSize(1);
  tft.setCursor(SIDE_X + 10, WEATHER_Y + 62);
  tft.setTextColor(TFT_LIGHTGREY);
  tft.print("Temp");
}
// puts all the previous panels together for the Cirucit infor screen
void drawCircuitInfo(){

  // Clear screen
  tft.fillScreen(ui_white);

  // Header
  drawPreviousRaceHeader();

  // Main circuit panel
  drawCircuitPanel();

  // Session panel
  drawSessionPanel();

  // Weather panel
  drawWeatherPanel();
}
// Draw leaderboard header
void drawLeaderBoardHeader(){
  // Draw a violet header for the leaderboard 
  tft.fillRect( 10,0,SCREEN_WIDTH + 60 ,HEADER_H, TFT_DARKGREEN);

  // Small LIVE label
  tft.setTextColor(TFT_YELLOW);// Makes the text red
  tft.setTextSize(1);
  tft.setCursor(10, 25);
  tft.print("LIVE");

  
  tft.setTextColor(ui_white);// Switch the text black to black
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.println(currentCircuit + " Leaderboard"); // Adds the previous circuit to the leaderboard header

  // Session indicator on right
  tft.setTextSize(1);
  tft.setTextColor(ui_black);
  tft.setCursor(250, 15);
  tft.print("TOP 5");
  
  tft.drawLine(10, 52, tft.width(), 52, ui_yellow);
}

// Draws the rows for the leaderboard to put the top 5 from the race in order
void drawLeaderBoardRow(){

  for(int i = 0; i < driverCount && i < 5; i++){

    int y = 65 + (i * 32);

    uint16_t positionColor;

    if(driverPositions[i] == "1"){
      //Sets color to gold
      positionColor = ui_yellow;
    }else  if(driverPositions[i] == "2"){
      // Sets the color to silver for second
      positionColor = ui_silver;
    }else if(driverPositions[i] == "3"){
      // Sets color to bronze
      positionColor = ui_bronze;
    }else{
      // Default white
      positionColor = ui_black;
    }
    // Makes the screen black
    tft.fillRoundRect(8,y,304,31,6,ui_white);
    // Blue border
    tft.drawRoundRect(8,y,304,31,6,ui_navy);

    tft.fillRoundRect(13,y + 5,4,21,2,TFT_LIGHTGREY);
    // Sets the color back to correspond with the correct Position the last two will just be defualted to text colro white
    tft.setTextColor(positionColor);
    
    // Position Number
    tft.setTextSize(2);
    tft.setCursor(24, y + 7);
    tft.println(driverPositions[i]);

    // Driver Name
    tft.setCursor(55, y + 9);
    tft.println(driverNames[i]);

    // Draws the Drivers Numbers
    tft.setTextSize(1);
    tft.setCursor(180, y + 5);
    tft.print("#");
    tft.println(driverNumbers[i]);
  }

}
// Leaderboard 
void drawLeaderBoard(){
  // Makes the screen black
  tft.fillScreen(ui_white);

  // Creates header
  drawLeaderBoardHeader();

  // Error boundary safety catch: displays diagnostic notice if the data feed arrays are empty
  if(driverCount == 0){
    tft.setTextColor(ui_black);
    tft.setCursor(0, 35);
    tft.println("NO LIVE DATA");

    tft.setCursor(0,35);
    tft.println("Waiting for race...");

    return;// Fast escape exit block
  }

  // Creates the rows
  drawLeaderBoardRow();

}
// Draw the screen for the fastest lap
void drawFastestLap(){

  tft.fillScreen(ui_white);

  tft.setTextColor(ui_black);
  tft.setCursor(10,30);
  tft.setTextSize(2);
  tft.println("Fastest Laps");
  tft.drawLine(10, 52, tft.width(), 52, ui_navy);

  tft.setTextColor(ui_black);
  tft.setTextSize(3);

  tft.setCursor(10,55);

  for (int i = 0; i < driverCount && i < 5; i++) {
    // Calculate the vertical position for each driver
    int y = 65 + (i * 32);

    // Draws the driver names
    tft.setTextSize(2);
    tft.setCursor(10, y);
    tft.println(driverNames[i]);
    // Draws the driver's number
    tft.setTextSize(1);
    tft.setCursor(180, y + 5);
    tft.print("#");
    tft.println(driverNumbers[i]);
    // Draws the fastest times
    tft.setCursor(240, y +5);
    tft.println(driverTimes[i]);
  }
}
// Get driver time
void getDriverTime() {

  // Temporary arrays used for sorting fastest laps
  String tempNames[5];
  String tempNumbers[5];
  String tempTimesFormatted[5];
  float tempTimes[5];

  int tempCount = 0;

  // Request lap data for each driver in the current top 5
  for (int i = 0; i < driverCount && i < 5; i++) {

    int driver = driverNumbers[i].toInt();

    if (driver <= 0) {
      continue;
    }
    // Use HTTPS for API requests.
    // Certificate validation is disabled for this prototype.
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url = String(openF1) + "/laps?session_key=" + String(currentSessionKey) + "&driver_number=" + String(driver) + "&is_pit_out_lap=false";

    Serial.print("Requesting laps for #");
    Serial.println(driver);

    if (!http.begin(client, url)) {
      Serial.println("HTTP begin failed.");
      continue;
    }

    int responseCode = http.GET();

    if (responseCode != 200) {

      Serial.print("Lap request failed: ");
      Serial.println(responseCode);

      http.end();
      continue;
    }

    String payload = http.getString();

    http.end();

    DynamicJsonDocument doc(12000);

    DeserializationError error =
      deserializeJson(doc, payload);

    if (error) {

      Serial.print("Lap JSON error: ");
      Serial.println(error.c_str());

      continue;
    }

    // Find the fastest valid lap
    float fastestLap = 9999.0;

    for (JsonObject lap : doc.as<JsonArray>()) {

      float lapDuration =
        lap["lap_duration"] | 0.0;

      // Ignore invalid lap times
      if (lapDuration <= 0) {
        continue;
      }

      // Ignore unrealistic values
      if (lapDuration >= 9999.0) {
        continue;
      }

      if (lapDuration < fastestLap) {
        fastestLap = lapDuration;
      }
    }

    // Save valid fastest lap
    if (fastestLap < 9999.0 && tempCount < 5) {

      tempNumbers[tempCount] = driverNumbers[i];

      tempNames[tempCount] = driverNames[i];

      tempTimes[tempCount] = fastestLap;

      tempCount++;

      Serial.print("Fastest lap for ");
      Serial.print(driverNames[i]);
      Serial.print(": ");
      Serial.println(fastestLap);
    }

    // Small delay between API requests
    delay(350);
  }

  // Sort fastest lap
  for (int i = 0; i < tempCount - 1; i++) {

    for (int j = i + 1; j < tempCount; j++) {

      if (tempTimes[j] < tempTimes[i]) {

        // Swap time
        float tempTime = tempTimes[i];

        tempTimes[i] = tempTimes[j];

        tempTimes[j] = tempTime;


        // Swap driver number
        String tempNumber = tempNumbers[i];

        tempNumbers[i] = tempNumbers[j];

        tempNumbers[j] = tempNumber;


        // Swap driver name
        String tempName = tempNames[i];

        tempNames[i] = tempNames[j];

        tempNames[j] = tempName;
      }
    }
  }

  // Store fastest lap data
  // Clear old fastest-lap data
  for (int i = 0; i < 5; i++) {

    driverTimes[i] = "";
  }


  // Store the sorted fastest laps
  for (int i = 0; i < tempCount && i < 5; i++) {

    // Convert seconds into M:SS.sss

    int minutes =
      (int)(tempTimes[i] / 60);

    float seconds =
      tempTimes[i] -
      (minutes * 60);

    char formattedTime[20];

    snprintf(
      formattedTime,
      sizeof(formattedTime),
      "%d:%06.3f",
      minutes,
      seconds
    );

    driverTimes[i] =
      String(formattedTime);


    Serial.print("FASTEST #");
    Serial.print(i + 1);
    Serial.print(" ");
    Serial.print(tempNames[i]);
    Serial.print(" ");
    Serial.println(driverTimes[i]);
  }
  // Confirms everything went well
  Serial.println("Fastest lap processing complete.");
}
// Gets the race result order
void getRaceResults(){
  // Use HTTPS for API requests.
  // Certificate validation is disabled for this prototype.
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  // Request the race results using the selected session key
  String url = String(openF1) + "/session_result?session_key=" + String(currentSessionKey);

  if(!http.begin(client, url)){
    Serial.println("HTTP begin failed.");

    return;
  }

  int responseCode = http.GET();

  if(responseCode != 200){

    Serial.print("Race result request failed: ");
    Serial.println(responseCode);

    http.end();

    return;
  }

  String payload = http.getString();

  http.end();

  DynamicJsonDocument doc(30000);

  DeserializationError error = deserializeJson(doc, payload);

  if(error){
    Serial.print("Race result JSON error: ");
    Serial.println(error.c_str());

    return;
  }

  // Clear results from the previous race
  for(int i = 0; i < 5; i++){

    driverPositions[i] = "";
    driverNumbers[i] = "";
    driverNames[i] = "UNK";

  }
  driverCount = 0;

  for(JsonObject obj : doc.as<JsonArray>()){
    int position = obj["position"] | 0;

    int driverNumber = obj["driver_number"] | 0;

    // Only store drivers finishing in the top five
    if(position >= 1 && position <= 5 && driverNumber > 0){

      // Convert the race position into an array index
      int index = position - 1;

      driverPositions[index] = String(position);

      driverNumbers[index] = String(driverNumber);
    }
  }

  // Count how many valid drivers were found
  for(int i = 0; i < 5; i++){
    if (driverNumbers[i].length() > 0) {

      driverCount++;
    }
  }
  Serial.println();
  Serial.println("TOP 5:");
  for(int i = 0; i < 5; i++){
    if(driverNumbers[i].length() > 0){
      Serial.print(driverPositions[i]);

      Serial.print(" #");

      Serial.println(driverNumbers[i]);
    }

  }
}
// Gather driver names 
void getDriverNames(){
  // Use HTTPS for API requests.
  // Certificate validation is disabled for this prototype.
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();

  // Request drivers for the selected race session
  String url = String(openF1) + "/drivers?session_key=" + String(currentSessionKey);

  if(!http.begin(client, url)){
    // Runs if not successful
    Serial.println("Driver HTTP begin failed.");
    return;
  }

  int responseCode = http.GET();

  if(responseCode != 200){
    // Runs if not successful
    Serial.print("Driver request failed: ");
    Serial.println(responseCode);

    http.end();

    return;
  }

  // Store the API response
  String payload = http.getString();
  http.end();

  // Create a JSON document for the API response
  JsonDocument doc;

  // Convert the API response into JSON data
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("Driver JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  // Clear old driver names before loading new data
  for(int i = 0; i < 5; i++){

  driverNames[i] = "Unknown";

  }

  // Match each leaderboard driver number with the driver information from OpenF1
  for(int i = 0; i < driverCount && i < 5; i++){

    int resultDriverNumber = driverNumbers[i].toInt();

    for(JsonObject driver : doc.as<JsonArray>()){

      int apiDriverNumber = driver["driver_number"] | 0;

      // Check whether the driver numbers match
      if(apiDriverNumber == resultDriverNumber){

        const char* name = driver["name_acronym"] | "Unknown";

        driverNames[i] = String(name);

        break;
      }
    }
  }
}
// Find the last race
void findLastRace(){

  // Use HTTPS for API requests.
  // Certificate validation is disabled for this prototype.
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  // Request all 2026 Formula 1 race sessions
  String url = String(openF1) + "/sessions?session_name=Race&year=2026";

  if(!http.begin(client,url)){
    Serial.println("HTTP begin failed.");

    return;
  }

  int responseCode = http.GET();

  if(responseCode != 200){
    Serial.print("Session request failed: ");
    Serial.println(responseCode);

    http.end();

    return;
  }

  String payload = http.getString();

  http.end();

  DynamicJsonDocument doc(50000);

  DeserializationError error = deserializeJson(doc, payload);

  if(error){
    Serial.print("Session JSON error: ");
    Serial.println(error.c_str());

    return;
  } 

  time_t now;
  time(&now);

  struct tm* timeinfo = gmtime(&now);

  char currentTime[25];

  strftime(currentTime, sizeof(currentTime), "%Y-%m-%dT%H:%M:%S", timeinfo);

  String nowString = String(currentTime);

  Serial.print("Current UTC: ");
  Serial.println(nowString);

  String latestDate = "";

  int latestSessionKey = 0;

  String latestCircuit = "UNKNOWN";

  String latestLocation = "UNKNOWN";
  String latestCountry = "UNKNOWN";

  for(JsonObject session : doc.as<JsonArray>()){
    int sessionKey = session["session_key"] | 0;

    String dateEnd = session["date_end"] | "";

    String circuit = session["circuit_short_name"] | "UNKNOWN";

    String location = session["location"] | "UNKNOWN";

    String country = session["country_name"] | "UNKNOWN";

    bool cancelled = session["is_cancelled"] | false;

    // Ignore invalid session records
    if(sessionKey == 0){
      continue;
    }

    // Ignore sessions without a valid end time
    if(dateEnd.length() < 19){

      continue;
    }

    // Ignore cancelled races
    if(cancelled){
      continue;
    }

    String endTime = dateEnd.substring(0, 19);

    // Only consider races that have already finished
    if(endTime <= nowString){
    // Keep the newest completed race found so far
      if(latestDate == "" || endTime > latestDate){

        latestDate = endTime;
        latestSessionKey = sessionKey;
        latestCircuit = circuit;
        latestLocation = location;
        latestCountry = country;

        Serial.println();
        Serial.println("New latest race found:");

        Serial.print("Session Key: ");
        Serial.println(sessionKey);

        Serial.print("Circuit: ");
        Serial.println(circuit);

        Serial.print("Location: ");
        Serial.println(currentCountry);

        Serial.print("Finished: ");
        Serial.println(endTime);

      }
    }

  }
  
  if(latestSessionKey != 0){

    currentSessionKey = latestSessionKey;

    currentCircuit = latestCircuit;

    currentRace = latestLocation;

    currentCountry = latestCountry;

    currentSession = "RACE";
    Serial.println();

    Serial.println("LAST COMPLETED RACE");

    Serial.print("Session Key: ");
    Serial.println(currentSessionKey);

    Serial.print("Circuit: ");
    Serial.println(currentCircuit);

    Serial.print("Location: ");
    Serial.println(currentCountry);
  }else{
    Serial.println("No completed race found.");
  }
}
// Draws the current track layout still A working progress 
void drawTrackBitmap(const uint8_t *bitmap, int x, int y, int width, int height) {
    tft.drawBitmap(x, y, bitmap, width, height, ui_black);
}
// Draws the screen for the Track currently under progress due to memory size a lot of distortion is happening
void drawCurrentTrack(){

  Serial.print(currentCircuit);

  // Clear the screen
  tft.fillScreen(ui_white);

  //Header
  tft.fillRoundRect(0,0, tft.width(), 40,8, ui_purple);
  tft.setTextColor(ui_white);
  tft.setTextSize(2);
  tft.setCursor(10,10);
  tft.print("Track");

  //Circuit Name
  tft.setTextSize(1);
  tft.setCursor(180, 14);
  tft.print(currentCircuit);

  //Divider
  tft.drawFastHLine(10,45,SCREEN_WIDTH, ui_yellow);

  for(int i = 0; i < trackBitmapCount; i++){

    if(currentCircuit.equalsIgnoreCase(trackBitmaps[i].circuitName)){

      // Confirms track 
      Serial.print("Track found: ");
      Serial.println(trackBitmaps[i].circuitName);

      //Centers the bitmap
      int x = (tft.width() - trackBitmaps[i].width) / 2;

      // Positon the bitmpa underneath
      int y = 70;

      //Draws the bitmap
      drawTrackBitmap(trackBitmaps[i].bitmap,x,y,trackBitmaps[i].width,trackBitmaps[i].height);

      tft.setTextColor(ui_purple);
      tft.setTextSize(2);

      int textWidth = currentCircuit.length() * 12;

      int textX = (tft.width() - textWidth) / 2;

      tft.setCursor(textX, 220);
      tft.print(currentCircuit);

      return;
    }
  }

  Serial.print("Track not found: ");
  Serial.println(currentCircuit);

  tft.setTextColor(ui_black);
  tft.setTextSize(2);

  tft.setCursor(35, 110);
  tft.println("TRACK NOT FOUND");

  tft.setTextSize(1);

  tft.setCursor(35, 140);
  tft.print("Circuit: ");

  tft.println(currentCircuit);
}
// Updates all the F1 date
void updateF1(){

  // Stop if the ESP32 is not connected to Wi-Fi
  if(WiFi.status() != WL_CONNECTED){

    Serial.println("WiFi disconnected");

    return;
  }
  // Find the most recently completed race
  findLastRace();

  // Stop if no valid race session was found
  if(currentSessionKey == 0){
    Serial.println("No completed race available.");

    return;
  }

  // Get the finishing positions and driver numbers
  getRaceResults();
  // Match driver numbers with driver abbreviations
  getDriverNames();
  // Find and sort the fastest laps
  getDriverTime();
}
// Draws the current screen
void drawCurrentScreen(){

 if(currentScreen == 0){
    drawLeaderBoard();
  }else if(currentScreen == 1){
    drawCircuitInfo();
  }else if(currentScreen == 2){
    drawFastestLap();
  }else if(currentScreen == 3){
    drawCurrentTrack();
  }
}
// Setup code
void setup() {

  Serial.begin(115200);

  // Initialize TFT
  tft.init();

  // Turn on CYD backlight
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  //Begin touch screen
  touchSPI.begin( TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  // Starts the touch screen
  touch.begin(touchSPI);
  touch.setRotation(1);

  // Landscape orientation
  tft.setRotation(3);

  // Clear display
  tft.fillScreen(ui_purple);

  // Default text color
  tft.setTextColor(ui_white);

  // Boot screen
  showBootScreen();

  // Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  // NTP
  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  delay(2000);

  // Wi-Fi information
  tft.fillScreen(ui_white);

  tft.setCursor(0, 0);
  tft.print("WiFi Connected");

  tft.setCursor(0, 15);
  tft.print("IP: ");
  tft.println(WiFi.localIP());

  delay(2000);

  // Get F1 data
  updateF1();

  // Draw the first screen
  currentScreen = 0;
  drawCurrentScreen();

  // Start timers
  lastUpdate = millis();
  lastScreenChange = millis();
}
// Loop code
void loop() {

  // Update F1 data every 10 min

  if (millis() - lastUpdate >= UPDATE_INTERVAL) {

    updateF1();

    lastUpdate = millis();

    // Redraw screen with new data
    drawCurrentScreen();
  }

  //Touch Screen Variables
  uint8_t touchX;
  uint8_t touchY;

  //Changes the screen on touch
  if (touch.touched()){

    //Prevents the change of one press to multiple screens
    if(millis() - lastTouch >= 500){

      //Move to next screen
      currentScreen++;

      //Resets the current screen
      if(currentScreen > 3){

        currentScreen = 0;
      }
      //Draw new screen
      drawCurrentScreen();

      // Save Touch Time
      lastTouch = millis();
    }
  }
  //Small delay
  delay(10);
}
