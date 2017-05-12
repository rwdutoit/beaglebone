#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

int read_distance()
{
 int fd;
 char val[6];

 fd = open("/dev/rpmsg_pru31", O_RDWR);
 if(fd < 0)
 {
  //printf("Error opening pru31\n"); 
  return(-1);
 }
 write(fd,"read",4);
 read(fd, val, 6);
 // printf ("String  = %s \n",val);
 close(fd);
 return atoi(val);
}

int main(void)
{
 int dist = read_distance();
 //printf ("Distance  = %d \n",dist);



   printf ("Content-type:text/html\r\n\r\n ");      // Generate the HTML output
//   cout << "Refresh:1\r\n\r\n";      // Refresh
   printf ("<html><head>\n");
//cout << "<META HTTP-EQUIV='refresh' CONTENT='1'>";
   printf ("</head><body>\n");
   printf ("<div> Distance is: %d</div>",dist);
   printf ("</body></html>\n");



   return 0;
}
