//String homepage;

namespace AppWebServer {
  String homepage;
  
  WiFiServer server(80);
  
  String bsLink(String link) {
    return "<a class=\"btn btn-success\" href=\"" + link + "\">" + link + "</a>";
  }
  
  void returnJSON(WiFiClient client, String json) {
  client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("");
    client.println(json);
    client.flush();
    delay(1);
  }
  
  void buildHomepage() {
    homepage = "HTTP/1.1 200 OK";
    homepage += "\r\nContent-Type: text/html";
    homepage += "\r\n";
    homepage += "\r\n<!DOCTYPE HTML>";
    homepage += "\r\n<html>";
    homepage += "\r\n<head>";
    // homepage += "\r\n<link rel=\"icon\" href=\"data:;base64,=">"; // NB: Shortest version, but doesn't validate against HTML5, use this instead:-
    // homepage += "\r\n<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
    homepage += "\r\n<link rel=\"icon\" href=\"data:,\">"; // NB: Apparently valid HTML5 and no IE <= 8 browser quirks!
    homepage += "\r\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">";
    homepage += "\r\n<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.5/css/bootstrap.min.css\" integrity=\"sha384-AysaV+vQoT3kOAXZkl02PThvDr8HYKPZhNT5h/CXfBThSRXQ6jW5DO2ekP5ViFdi\" crossorigin=\"anonymous\">";
    homepage += "\r\n</head>";
    homepage += "\r\n<body>";
    homepage += "\r\n<div class=\"container\">";
    homepage += "\r\n<div class=\"jumbotron\">";
    homepage += "\r\n<div class=\"lead\">WiFi LED</div>";
    homepage += "\r\n<div>Try these links: <br/>";
    homepage += "\r\n" + bsLink("/red");
    homepage += "\r\n" + bsLink("/green");
    homepage += "\r\n" + bsLink("/blue");
    homepage += "\r\n" + bsLink("/black");
    homepage += "\r\n" + bsLink("/white");
    homepage += "\r\n" + bsLink("/purple");
    homepage += "\r\n" + bsLink("/mixed");
    homepage += "\r\n" + bsLink("/redgreen");
    homepage += "\r\n" + bsLink("/rrrggg");
    homepage += "\r\n" + bsLink("/rrrgggbbb");
    homepage += "\r\n" + bsLink("/rainbow");
    homepage += "\r\n</div>";
    homepage += "\r\n<div>";
    homepage += "\r\n" + bsLink("/addled");
    homepage += "\r\n" + bsLink("/removeled");
    homepage += "\r\n" + bsLink("/restart");
    homepage += "\r\n" + bsLink("/reset");
    homepage += "\r\n" + bsLink("/clearwifi");
    homepage += "\r\n" + bsLink("/darken");
    homepage += "\r\n" + bsLink("/lighten");
    homepage += "\r\n" + bsLink("/save");
    homepage += "\r\n" + bsLink("/load");
    homepage += "\r\n</div>";
    // Add Bootstrap JS
    homepage += "\r\n</div>";
    homepage += "\r\n<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js\" integrity=\"sha384-3ceskX3iaEnIogmQchP8opvBy3Mi7Ce34nWjpBIwVTHfGYWQS9jwHDVRnpKKHJg7\" crossorigin=\"anonymous\"></script>";
    homepage += "\r\n<script src=\"https://cdnjs.cloudflare.com/ajax/libs/tether/1.3.7/js/tether.min.js\" integrity=\"sha384-XTs3FgkjiBgo8qjEjBk0tGmf3wPrWtA6coPfQDfFEY8AnYJwjalXCiosYRBIBZX8\" crossorigin=\"anonymous\"></script>";
    homepage += "\r\n<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.5/js/bootstrap.min.js\" integrity=\"sha384-BLiI7JTZm+JWlgKa0M0kGRpJbF2J8q+qreVrKBC47e3K6BW78kGLrCkeRX6I9RoK\" crossorigin=\"anonymous\"></script>";
    // End of Bootstrap JS
    homepage += "\r\n</body>";
    homepage += "\r\n</html>";
  }

  void returnHomepage(WiFiClient client) {
    client.println(homepage);
    client.flush();
    delay(1);
  }

  void setupWebServer()
  {
    buildHomepage();
    //AppWebServer::buildHomepage();
    
    server.begin();
    Serial.print("HTTP server started - http://");
    Serial.println(WiFi.localIP());
  }

  void handle()
  {
    WiFiClient client = server.available();
    //WiFiClient client = AppWebServer.server.available();
  
    bool available = false;
    if (client) {
      //Serial.print("New client: ");
      //Serial.println(client.remoteIP());
  
      // Wait until client is ready
      const int clientDelay = 10;
      int remainingWaits = 100;
      while (!client.available() && remainingWaits-- > 0) {
        delay(clientDelay);
      }
      if (client.available()) {
        //Serial.println("client available, continuing");
        available = true;
      } else {
        client.stop();
      }
    }
    if (available)
    {
      // Read the first line of the request
      String request = client.readStringUntil('\r');
      String url;  // null represents no match
      bool doNotReturnHomepage = false;
      Serial.println(request);
      client.flush();
  
      // NB: Request should be of this following format
      // GET /somepath HTTP/1.1
      if (request.startsWith("GET ")) {
        url = request.substring(4);
        int spacePos = url.indexOf(' ');
        url = url.substring(0, spacePos);
        Serial.print("Handling GET request: ");
        Serial.println(url);
      } else if (request.startsWith("POST ")) {
        Serial.print("Unhandled POST request: ");
        Serial.println(request.substring(5));
      }
  
      if (url) {
        // Match the request
        if (url == "/") {
          Serial.println("Returning homepage only");
        } else if (url == "/black") {
          //Serial.println("Setting to black");
          //setLEDs(1, colours_black);
          //setLEDs(CRGB::Black);
          update(COMMAND_BLACK);
        } else if (url == "/white") {
          //Serial.println("Setting to white");
          //setLEDs(1, colours_white);
          //setLEDs(CRGB::White);
          update(COMMAND_WHITE);
        } else if (url == "/blue") {
          //Serial.println("Setting to blue");
          //setLEDs(1, colours_blue);
          //setLEDs(CRGB::Blue);
          update(COMMAND_BLUE);
        } else if (url == "/red") {
          Serial.println("Setting to red");
          //setLEDs(CRGB::Red);
          //update(LEDCommands::Red);
          update(COMMAND_RED);
        } else if (url == "/green") {
          //Serial.println("Setting to green");
          //setLEDs(1, colours_green);
          //setLEDs(CRGB::Green);
          update(COMMAND_GREEN);
        } else if (url == "/redgreen") {
          //Serial.println("Setting to red-green");
          //setLEDs(2, colours_redgreen);
          update(COMMAND_REDGREEN);
        } else if (url == "/rrrggg") {
          Serial.println("Setting to rrr-ggg");
          //setLEDs(6, colours_rrrggg);
          update(COMMAND_RRRGGG);
        } else if (url == "/rrrgggbbb") {
          //Serial.println("Setting to rrr-ggg-bbb");
          //setLEDs(2, colours_rrrgggbbb);
          update(COMMAND_RRRGGGBBB);
        } else if (url == "/mixed") {
          Serial.println("Setting to mixed");
          setLEDs(3, colours_mixed);
        } else if (url == "/purple") {
          Serial.println("Setting to purple");
          setLEDs(CRGB::Purple);
        } else if (url == "/rainbow") {
          Serial.println("Setting to rainbow");
          update(COMMAND_RAINBOW);
        } else if (url == "/save") {
          //Serial.println("Saving settings");
          update(COMMAND_SAVE);
          Serial.println("COMMAND_SAVE completed");
        } else if (url == "/load") {
          //Serial.println("Loading settings");
          update(COMMAND_LOAD);
        } else if (url == "/favicon.ico") {
          Serial.println("ignored request for favicon.ico");
          doNotReturnHomepage = true;
        } else if (url == "/addled") {
          Serial.println("Adding LED");
          addLED();
        } else if (url == "/removeled") {
          Serial.println("Removing LED");
          removeLED();
        } else if (url == "/clearwifi") {
          Serial.println("Clearing WiFi");
          WiFiManager wiFiManager;
          wiFiManager.resetSettings();
        } else if (url == "/restart") {
          Serial.println("Restarting (soft-reboot)");
          ESP.restart();
        } else if (url == "/reset") {
          Serial.println("Resetting (hard-reboot)");
          ESP.reset();
        } else {
          Serial.print("Unhandled path: ");
          Serial.println(url);
          //client.stop();
        }
      }
      if (!doNotReturnHomepage) {
        returnHomepage(client);
      }
      client.flush();
      client.stop();
    }
  }
}
