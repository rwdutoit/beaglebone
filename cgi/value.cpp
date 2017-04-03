#include <stdlib.h>         // for the getenv call
#include <string>
// basic file operations
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
 int counter=0;

  string line;
  ifstream infile ("counter.txt");
  if (infile.is_open())
  {
    //while ( getline (infile,line) )
    while ( getline (infile,line) )
    {
     //cout << line << '\n';
     const char * c = line.c_str();
     counter = atoi(c);
     //cout << "counter: " << counter;
    }
    infile.close();
  }
  else
    cout << "Unable to open file"; 


   cout << "Content-type:text/html\r\n\r\n ";      // Generate the HTML output
//   cout << "Refresh:1\r\n\r\n";      // Refresh
   cout << "<html><head>\n";
//cout << "<META HTTP-EQUIV='refresh' CONTENT='1'>";
   cout << "</head><body>\n";
   cout << "<div> Counter is: "  << counter << "</div>";
   cout << "</body></html>\n";




  ofstream myfile ("counter.txt");
  if (myfile.is_open())
  {
     myfile << ++counter << "\n";
    //myfile << "This is a line.\n";
    //myfile << "This is another line.\n";
    myfile.close();
  }
  else cout << "Unable to open file";

   return 0;
}
