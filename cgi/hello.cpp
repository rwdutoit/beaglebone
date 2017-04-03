/* C++ CGI BeagleBone uptime example -- Written by Derek Molloy (www.derekmolloy.ie) */

#include <iostream>         // for the input/output
#include <stdlib.h>         // for the getenv call
#include <sys/sysinfo.h>    // for the system uptime call
using namespace std;

int main(){
   cout << "Content-type:text/html\r\n\r\n ";      // Generate the HTML output
//   cout << "Refresh:1\r\n\r\n";      // Refresh
   cout << "<html><head>\n";
cout << "<script type=text/javascript>";
cout << "window.onload = function() {";
cout << "    var frameRefreshInterval = setInterval(2000, function() {";
cout << "        document.getElementById(\"myframe\").src = document.getElementById(\"myframe\").src";
cout << "    });";
    // any other code
cout << "}";
cout << "</script>";
/*
cout << "<script>";
cout << "<window.setInterval(\"reloadIFrame();\", 100);";
cout << "<function reloadIFrame() {";
cout << "< document.frames[\"iframe_a\"].location.reload();";
cout << "<}";
cout << "</script>";
*/
//cout << "<META HTTP-EQUIV='refresh' CONTENT='1'>";
   cout << "<title>Beaglebone</title>\n";
   cout << "</head><body>\n";
   cout << "<h1>Counter</h1>\n";
cout << "<iframe name=\"myframe\" src=\"/cgi-bin/demo.cgi\" width=\"300\" height=\"150\">";
cout << "</iframe>";
   cout << "<div> Hello</div>";
   cout << "</body></html>\n";

   return 0;
}
