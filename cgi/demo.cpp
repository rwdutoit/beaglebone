/* C++ CGI BeagleBone uptime example -- Written by Derek Molloy (www.derekmolloy.ie) */

#include <iostream>         // for the input/output
#include <stdlib.h>         // for the getenv call
#include <sys/sysinfo.h>    // for the system uptime call
using namespace std;

int main()
{

cout << "Content-type:text/html\r\n\r\n ";      // Generate the HTML output
cout << "<html>\n";
cout << "<body>\n";

cout << "<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js\"></script>";
cout << "<script type=\"text/javascript\">";
cout << "    setInterval(refreshIframe, 1000);";
cout << "    function refreshIframe() {";
cout << "        $(\"#Iframe1\")[0].src = $(\"#Iframe1\")[0].src;";
cout << "    }";
cout << "</script>";
cout << "<h1>Counter jQuery</h1>\n";
cout << "    <iframe id=\"Iframe1\" name=\"Iframe1\" src=\"/cgi-bin/distance.cgi\" frameborder=\"0\"></iframe>";


cout << "<h1>Counter javascript</h1>\n";
cout << "   <script type=\"text/javascript\">";
cout << "       setInterval(refreshIframe2, 1000);";
cout << "       function refreshIframe2() {";
cout << "           var frame = document.getElementById(\"Frame2\");";
cout << "           frame.src = frame.src;";
cout << "       }";

cout << "   </script>";
cout << "   <iframe id=\"Frame2\" src=\"/cgi-bin/value.cgi\" frameborder=\"0\"></iframe>";


cout << "</body>";
cout << "</html>\n";

   return 0;
}
