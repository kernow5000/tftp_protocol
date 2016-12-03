// Shaun Bradley
// Bsc Mulimedia computing Yr3
// 
// CNET308 File transfer protocol
// Protocol Test harness

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "protocol.h"  // my transport protocol defines

// global variables
int outfile, infile = 0;


//function prototypes
int open_input_file(char *file);
int create_output_file(char *file);
int write_chunk(int id, struct mypacket_t chunk);
struct mypacket_t read_chunk(int id);
  

// opens the source file for reading
int open_input_file(char *file)
{
   
   // try to open the requested file
   if( (infile = open(file, O_RDONLY)) == -1) // open in binary reading mode
     {
	//send an error, can't find file
	//proto_send_error(0,CANTOPENFILE);
	//error sending broken at present
	close(outfile);
	return -1;
     }
   
   return 0;
}


// creates the output file ready for writing
int create_output_file(char *file)
{
   char temp[256]; // for building new filename
   
   strcat(temp, file);
   strcat(temp, ".copy"); // add extension
   
   if( (outfile = open (temp, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR)) == -1)
     {
	// send an error, can't open file
	//proto_send_error(0,CANTOPENFILE);
	//error sending broken at present
	close(outfile);
	return -1; // error
     }
   
   return 0;
}


// reads a chunk from the input file
struct mypacket_t read_chunk(int id)
{
   
   // read a chunk from the input file
 
   struct mypacket_t t;  // temp packet
   

   // read a chunk of file
   t.size = read(infile, t.data, CHUNK_SIZE);
   t.header = DATA;
   t.id = id;
   
   if(t.size == -1) // read error occurred
     {
	// send an error, cant read a chunk
	//proto_send_error(0,CANTREADFILE);
	//error sending broken at present
	close(infile);
     }
   
   
   return t; // return the full packet
}


// writes a packet data chunk to the output file
int write_chunk(int id, struct mypacket_t chunk)
{
   
   // write a chunk to the output file
   if(write(outfile, chunk.data, chunk.size) <= 0)
     {
	// send an error, can't write to file
	//proto_send_error(0,CANTWRITEFILE);
	//error sending broken at present
	close(outfile);
	return -1;
     }
   
   return 0;
}




int main(int argc, char *argv[])
{
   char *host;               // host to connect to
   char *filename;           // file to retrieve
   int  port = DEFAULT_PORT; // from header
   
   
   
   // display some info   
   printf("\nCNET 308 File transfer protocol test harness\n");
   printf("Shaun Bradley - Bsc Multimedia computing Yr3\n");

  
   // check command line arguments

   if(argc == 1) { // no arguments
      // running in SEND mode
      printf("<*> Running in send mode, on port %d\n", port);
      printf("<*> Press Ctrl-C to exit\n");
      proto_serve_file();
      // wait.. then chunk up some file
   }
   
   if(argc == 3) {
      // running in RECV mode
      printf("<*> Running in recieve mode\n");
      host = argv[1];
      filename = argv[2];
      printf("<*> connecting to %s\n", host);
      if( proto_connect(host, port) == -1) {
	 printf("<e> A connection error has occurred\n");
	 exit(1);  // tell the shell
      }
      else printf("<*> connected to %s, on port %d\n", host, port);
      
      // request a file
      printf("<r> requesting %s, from %s\n", filename, host);
      proto_request_file(filename);
      
   }
   
   proto_finish();
   
   
   return 0;
}
