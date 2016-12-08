//String homepage;

namespace AppWebServer {
  String bsLink(String link) {
    return "<a class=\"btn btn-success\" href=\"" + link + "\">" + link + "</a>";
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
}
