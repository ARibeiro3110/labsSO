/*
 * open-read.c
 *
 * Simple example of opening and reading to a file.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int min(int a, int b) { return a < b ? a : b;}

int main(int argc, char *argv[])
{
   /*
    *
    * the attributes are:
    * - O_RDONLY: open the file for reading
    *
    */
   FILE *fd = fopen("test.txt", "r");
   if (fd < 0){
      fprintf(stderr, "open error: %s\n", strerror(errno));
      return -1;
   }

   char buffer[128];
   memset(buffer,0,strlen(buffer));

   /* read the contents of the file */
   FILE *out = fopen("test-out.txt", "w");

   int bytes_read;
   int bytes_written = 0;

   while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer) - 1, fd)) > 0) {
      if (bytes_read < 0) {
         fprintf(stderr, "read error: %s\n", strerror(errno));
         return -1;
      }
      printf("%s", buffer);
      bytes_written += fwrite(buffer, sizeof(char), strlen(buffer), out);
      memset(buffer,0,strlen(buffer));
   }

   /* close the file */
   fclose(out);
   fclose(fd);

   return 0;
}
