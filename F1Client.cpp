#include "api/F1Client.h"

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#include "data/F1Data.h"

// Open F1 Api url
const char* OPENF1_URL = "https://api.openf1.org/v1";


bool getOpenF1Data(const String& url, String& payload){
    WiFiClientSecure client;

    client.setInsecure();

    HTTPClient http;

    Serial.println();
    Serial.println("OpenF1 Request");

    Serial.print("GET ");
    Serial.println(url);

    if (!http.begin(client, url)){
        Serial.println("ERROR: http.begin() failed");
        return false;
    }

    http.setTimeout(15000);

    Serial.println("Sending Request);

    int httpCode = http.GET();

    Serial.print("HTTP response: ");

    Serial.println(httpCode);

    // Connection error
    if (httpCode <= 0){
        Serial.print("HTTP error: ");

        Serial.println( http.errorToString(httpCode));

        http.end();
        return false;
    }


    // Server error
    if (httpCode != HTTP_CODE_OK){
        Serial.println("Server returned non-200 response.");

        String errorBody = http.getString();

        Serial.print( "Response body: ");

        Serial.println(errorBody);

        http.end();

        return false;
    }

    // Read response
    Serial.println("Reading response body...");

    payload = http.getString();

    Serial.print("Response length: ");

    Serial.println(payload.length());

    if (payload.length() == 0){
        Serial.println("ERROR: Empty response body");

        http.end();
        return false;
    }

    Serial.println("Response received successfully.");

    http.end();
    return true;
}

// Finds the last race
bool getLatestRace(int year, int& sessionKey, String& sessionName, String& raceName, String& circuitName, String& countryName, String& dateStart, String& dateEnd){
    Serial.println();
    Serial.println("Finding the last race");

    // Get all Race sessions for the year
    String sessionsUrl = String(OPENF1_URL) + "/sessions?session_name=Race&year=" + String(year);
    String sessionsPayload;

    if (!getOpenF1Data(sessionsUrl, sessionsPayload)){
        Serial.println("Failed to get race sessions.");

        return false;
    }

    JsonDocument sessionsDoc;

    DeserializationError error = deserializeJson(sessionsDoc, sessionsPayload);

    if (error){
        Serial.print("Session JSON parse error: ");

        Serial.println(error.c_str());
        return false;
    }

    JsonArray sessions = sessionsDoc.as<JsonArray>();

    if (sessions.isNull()){
        Serial.println("ERROR: Session response is not an array.");

        return false;
    }

    Serial.print("Race sessions received: ");

    Serial.println(
        sessions.size()
    );

    // Temporary current date
    // Later replace this with NTP time.
    const String currentTime = "2026-10-08T23:59:59+00:00";

    bool foundRace = false;

    String newestDateEnd = "";

    int selectedMeetingKey = 0;

    // Find latest completed race
    for (JsonObject session : sessions){
        String currentSessionName = session["session_name"] | "";

        String currentDateStart = session["date_start"] | "";

        String currentDateEnd = session["date_end"] | "";

        bool cancelled =
            session["is_cancelled"] | false;

        int meetingKey =
            session["meeting_key"] | 0;

        // ----------------------------------------------------
        // Only Race sessions
        // ----------------------------------------------------

        if (
            currentSessionName !=
            "Race"
        )
        {
            continue;
        }

        // Ignore cancelled sessions
        if (cancelled){
          
            continue;
        }
      
        // Ignore future races
        if (currentDateEnd >= currentTime){
            Serial.print("Skipping future race: ");

            Serial.println(currentDateEnd);

            continue;
        }


        // Select newest completed race
        if (!foundRace || currentDateEnd > newestDateEnd){
            foundRace = true;

            newestDateEnd = currentDateEnd;

            sessionKey = session["session_key"] | 0;

            selectedMeetingKey = meetingKey;

            sessionName = currentSessionName;

            circuitName = session[ "circuit_short_name"] | "";

            countryName = session["country_name"] | "";

            dateStart = currentDateStart;

            dateEnd = currentDateEnd;
        }
    }


    // No race found
    if (!foundRace){
        Serial.println("ERROR: No completed race found.");

        return false;
    }

    // Getting metting information
    Serial.println();
    Serial.println("Getting meeting information...");

    String meetingUrl = String(OPENF1_URL) + "/meetings?meeting_key=" + String(selectedMeetingKey);
    String meetingPayload;

    if (!getOpenF1Data(meetingUrl,meetingPayload)){
        Serial.println("Failed to get meeting information.");

        return false;
    }

    JsonDocument meetingDoc;

    error = deserializeJson(meetingDoc, meetingPayload);

    if (error){
        Serial.print("Meeting JSON parse error: ");

        Serial.println(error.c_str());

        return false;
    }

    JsonArray meetings = meetingDoc.as<JsonArray>();

    if (meetings.isNull() || meetings.size() == 0){
        Serial.println("ERROR: No meeting information found.");

        return false;
    }

    JsonObject meeting = meetings[0];

    // Race name
    raceName = meeting["meeting_name"] | "UNKNOWN";


    // Circuit
    String meetingCircuit =  meeting["circuit_short_name"] | "";

    if (meetingCircuit.length() > 0){
        circuitName = meetingCircuit;
    }


    // Country
    String meetingCountry = meeting["country_name"] | "";

    if (meetingCountry.length() > 0){
        countryName = meetingCountry;
    }


    // Print Selected Race
    Serial.println();
    Serial.println("Latest COmpleted Race");
    Serial.print("Meeting Key: ");
    Serial.println(selectedMeetingKey);
    Serial.print("Race: ");
    Serial.println(raceName);
    Serial.print("Session: ");
    Serial.println(sessionName);
    Serial.print("Circuit: ");
    Serial.println(circuitName);
    Serial.print("Country: ");
    Serial.println(countryName);
    Serial.print("Session Key: ");
    Serial.println(sessionKey);
    Serial.print("Date Start: ");
    Serial.println(dateStart);
    Serial.print("Date End: ");
    Serial.println(dateEnd);
    return true;
}

// Get Drivers
bool getDrivers(int sessionKey,JsonDocument& driversDoc){
    Serial.println();
    Serial.println("Getting Drivers");
    Serial.print("Session Key: ");

    Serial.println(sessionKey);

    String url = String(OPENF1_URL) + "/drivers?session_key=" + String(sessionKey);

    String payload;

    if (!getOpenF1Data(url, payload)){
        Serial.println("Failed to get drivers.");

        return false;
    }

    DeserializationError error = deserializeJson(driversDoc, payload);

    if (error){
        Serial.print("Driver JSON parse error: ");

        Serial.println(error.c_str());

        return false;
    }

    JsonArray driverArray = driversDoc.as<JsonArray>();

    if (driverArray.isNull()){
        Serial.println("ERROR: Driver response is not an array.");

        return false;
    }

    Serial.print("Drivers received: ");
    Serial.println(driverArray.size());
    Serial.println("Driver data loaded successfully.");

    return true;
}


// Get Race Results
bool getRaceResults(int sessionKey, JsonDocument& resultsDoc){
    Serial.println();
    Serial.println("Getting Race Results");

    Serial.print("Session Key: ");
    Serial.println(sessionKey);


    String url = String(OPENF1_URL) + "/session_result?session_key=" + String(sessionKey);

    Serial.println();
    Serial.println("Race Results Request");

    Serial.print("URL: ");

    Serial.println(url);

    String payload;

    if (!getOpenF1Data(url, payload)){
        Serial.println("Failed to get race results.");

        return false;
    }

    DeserializationError error = deserializeJson(resultsDoc, payload);

    if (error){
        Serial.print("Race results JSON parse error: ");

        Serial.println(error.c_str());

        return false;
    }

    JsonArray results = resultsDoc.as<JsonArray>();

    if (results.isNull()){
        Serial.println("ERROR: Race results are not an array.");

        return false;
    }

    Serial.print("Race results received: ");

    Serial.println(results.size());

    Serial.println("Race results loaded successfully.");

    return true;
}

// Build Leaderboard
void buildLeaderboard(JsonDocument& driversDoc, JsonDocument& resultsDoc){
    Serial.println();

    Serial.println("Building Leaderboard");
    JsonArray driverArray = driversDoc.as<JsonArray>();

    JsonArray resultArray = resultsDoc.as<JsonArray>();

    driverCount = 0;

    // P1 -> P5
    for (int position = 1; position <= MAX_DRIVERS; position++){
      
        bool foundResult = false;

        for (JsonObject result : resultArray){
            int resultPosition = result["position"] | 0;

            if (resultPosition != position){
                continue;
            }

            int driverNumber = result["driver_number"] | 0;

            bool foundDriver = false;

            // Find driver name
            for (JsonObject driver : driverArray){
                int apiDriverNumber = driver["driver_number"] | 0;

                if (apiDriverNumber != driverNumber){
                    continue;
                }

                drivers[driverCount].position = String(position);
                drivers[driverCount].number = String(driverNumber);
                drivers[driverCount].name = driver["full_name"] | "UNKNOWN";

                Serial.print("P");
                Serial.print(position);

                Serial.print(" ");
                Serial.print(drivers[driverCount].name);

                Serial.print(" #");
                Serial.println(drivers[driverCount].number);

                driverCount++;
                foundDriver = true;

                break;
            }

            if (!foundDriver){
                Serial.print("WARNING: Driver #");

                Serial.print(driverNumber);

                Serial.println(" not found.");
            }

            foundResult = true;

            break;
        }

        if (!foundResult){
            Serial.print("WARNING: Position ");

            Serial.print(position);

            Serial.println(" not found.");
        }

        if (driverCount >= MAX_DRIVERS){
            break;
        }
    }

    // Final Leaderboard
    Serial.println();
    Serial.println("Leaderboard");

    for (int i = 0; i < driverCount; i++){
        Serial.print(drivers[i].position);
        Serial.print(" ");
        Serial.print(drivers[i].name);
        Serial.print(" #");
        Serial.println(drivers[i].number);
    }
}

// Get Fastest Lap For One Driver
bool getDriverFastestLap(int sessionKey, int driverNumber, float& fastestTime){
    String url = String(OPENF1_URL) + "/laps?session_key=" + String(sessionKey) + "&driver_number=" + String(driverNumber) + "&lap_duration%3E0";

    Serial.println();
    Serial.println("Fastest Lap Request");

    Serial.print("Driver #: ");

    Serial.println(driverNumber);

    Serial.print("URL: ");

    Serial.println(url);

    String payload;

    if (!getOpenF1Data(url, payload)){
        Serial.println("Failed to get driver lap data.");

        return false;
    }

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error){
        Serial.print("Lap JSON parse error: ");

        Serial.println(error.c_str());

        return false;
    }

    JsonArray laps = doc.as<JsonArray>();

    if (laps.isNull()){
        Serial.println("ERROR: Lap response is not an array." );

        return false;
    }

    Serial.print("Laps received: ");

    Serial.println(laps.size());

    fastestTime = 0.0;

    // Find fastest valid lap
    for (JsonObject lap : laps){
        float lapTime = lap["lap_duration"] | 0.0;

        bool pitOut = lap["is_pit_out_lap"] | false;


        // Invalid lap
        if (lapTime <= 0.0){
            continue;
        }

        // Ignore pit-out laps
        if (pitOut){
            continue;
        }

        // Store fastest
        if (fastestTime == 0.0 || lapTime < fastestTime){
            fastestTime = lapTime;
        }
    }

   
    // No valid lap
    if (fastestTime <= 0.0){
        Serial.println("No valid lap found.");

        return false;
    }

    Serial.print("Fastest lap: ");

    Serial.println(fastestTime,3);

    return true;
}

// ============================================================
// Build Fastest Lap Leaderboard
void buildFastestLaps(int sessionKey){
    Serial.println();

    Serial.println("Building Fastest Lap Leaderboard");

    

    fastestLapCount = 0;

    // Get fastest lap for every driver currently in our leaderboard.
    for (int i = 0; i < driverCount; i++){
        int driverNumber = drivers[i].number.toInt();

        if (driverNumber <= 0){
            continue;
        }

        float fastestTime = 0.0;

        // Request this driver's laps
        if (!getDriverFastestLap(sessionKey, driverNumber, fastestTime)){
            Serial.print("Could not find fastest lap for ");

            Serial.println(drivers[i].name);

            continue;
        }

        
        // Store driver name
        fastestLaps[fastestLapCount].driverName = drivers[i].name;

       
        // Store driver number
        fastestLaps[fastestLapCount].driverNumber = drivers[i].number;

        // Store raw lap time
        fastestLaps[fastestLapCount].lapTime = fastestTime;

        
        // Convert seconds to M:SS.mmm
        int minutes = (int)(fastestTime / 60.0);

        float seconds = fastestTime - (minutes * 60.0);

        char formatted[16];

        snprintf(formatted, sizeof(formatted), "%d:%06.3f", minutes, seconds);

        fastestLaps[fastestLapCount].formattedTime = String(formatted);

        
        // Debug
        Serial.print(drivers[i].name);

        Serial.print(" #");

        Serial.print(drivers[i].number);

        Serial.print(" -> ");

        Serial.println(fastestLaps[fastestLapCount].formattedTime);

        fastestLapCount++;

        if (fastestLapCount >= MAX_DRIVERS){
            break;
        }
    }

    // Sort Fastest -> Slowest
    for (int i = 0; i < fastestLapCount - 1; i++){
        for (int j = i + 1; j < fastestLapCount; j++){
            if (fastestLaps[j].lapTime < fastestLaps[i].lapTime){
                FastestLap temp = fastestLaps[i];

                fastestLaps[i] = fastestLaps[j];

                fastestLaps[j] = temp;
            }
        }
    }

    // ========================================================
    // Final Fastest Lap List
    Serial.println();
    Serial.println("Fastest Laps");

    for (int i = 0; i < fastestLapCount; i++){
        Serial.print(i + 1);
        Serial.print(" ");
        Serial.print(fastestLaps[i].driverName);
        Serial.print(" #");
        Serial.print(fastestLaps[i].driverNumber);
        Serial.print(" ");
        Serial.println(fastestLaps[i].formattedTime);
    }
}


// Update F1 Data
bool updateF1Data()
{
    Serial.println();

    Serial.println("Updating F1 Data");

    // Race information
    int sessionKey = 0;

    String sessionName;
    String raceName;
    String circuitName;
    String countryName;
    String dateStart;
    String dateEnd;

    if (!getLatestRace(2026, sessionKey, sessionName, raceName, circuitName, countryName, dateStart, dateEnd)){
        Serial.println();

        Serial.println("FAILED: Could not find completed race.");

        Serial.println("F1 update failed.");

        return false;
    }

    // Update Current Race
    currentRace.sessionKey = sessionKey;
    currentRace.sessionName = sessionName;
    currentRace.raceName = raceName;
    currentRace.country = countryName;
    currentRace.circuit = circuitName;

    // Print Current Race
    Serial.println();
    Serial.println("Current Race");

    Serial.print("Race: ");
    Serial.println(currentRace.raceName);

    Serial.print("Circuit: ");
    Serial.println(currentRace.circuit);

    Serial.print("Country: ");
    Serial.println(currentRace.country);

    Serial.print("Session: ");
    Serial.println(currentRace.sessionName);

    Serial.print("Session Key: ");
    Serial.println(currentRace.sessionKey);
    JsonDocument driversDoc;

    if (!getDrivers(sessionKey, driversDoc)){
        Serial.println();

        Serial.println("FAILED: Drivers.");

        Serial.println("F1 update failed.");
        return false;
    }

    JsonDocument resultsDoc;

    if (!getRaceResults(sessionKey, resultsDoc)){
        Serial.println();

        Serial.println("FAILED: Race results.");

        Serial.println("F1 update failed.");

        return false;
    }

    // Build Race Leaderboard
    buildLeaderboard(driversDoc, resultsDoc);

    // Fastest Laps
    buildFastestLaps(sessionKey);
  
    Serial.println();
    Serial.println("F1 DATA UPDATE SUCCESSFUL");

    Serial.print("Race: ");

    Serial.println(currentRace.raceName);

    Serial.print("Circuit: ");
    Serial.println(currentRace.circuit);

    Serial.print("Country: ");
    Serial.println(currentRace.country);

    Serial.print("Session: ");
    Serial.println(currentRace.sessionName);

    Serial.print("Session Key: ");
    Serial.println(currentRace.sessionKey);

    Serial.print("Drivers loaded: ");
    Serial.println(driverCount);

    Serial.print("Fastest laps loaded: ");
    Serial.println(fastestLapCount);

    Serial.println("F1 data loaded.");

    return true;
}
