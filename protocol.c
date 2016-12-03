// Shaun Bradley
// Bsc Multimedia computing Yr3
// 
// CNET308 File transfer protocol
// Protocol functions


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>          // for gethostbyname()
#include <netdb.h>              // for gethostbyname()


#include "protocol.h"           // my transport protocol functions


// global variables  
int sockfd = 0;                 // socket descriptor
int len = 0;                    // for storing sizeof results
struct sockaddr_in p_in, p_out;
int id;                         // for storing packet id numbers
struct hostent *hostinfo;
int portused = 0;
int quit = 0;

struct mypacket_t inpacket,outpacket;      // 2 packet buffers, of my design



// implementation


// sets up a connection
int proto_connect(char *host, int port)
{
   char ourhostname[256];
   char *hostname;
   char **addrs;
   struct hostent *hostinfo;    // data struct for host info
   
   
   // grab a socket from the OS
   printf("<*> grabbing a sock\n");
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(sockfd == -1) {
      //printf("<e> socket error\n");  // debug
      return -1;
   }
   
   p_in.sin_family = AF_INET; // internet family
   hostname = host;
   
   hostinfo = gethostbyname(hostname);
   if(!hostinfo) {
      //printf("<e> gethost error\n");  // debug
      return -1;
   }
   
   addrs = hostinfo->h_addr_list;                   // get IP from hostinfo struct
   hostname = inet_ntoa(*(struct in_addr *)*addrs); // convert
   
   p_in.sin_addr.s_addr = inet_addr(hostname);      // use the first IP in list
   p_in.sin_port = htons(portused);                 // use port specified in header
   
   len = sizeof(p_in);                              // get size, of proto in struct
   
   if(connect(sockfd, (struct sockaddr *)&p_in, len) != 0)
     {
	//printf("<e> connect() error\n"); // debug
	close(sockfd);
	return -1;
     }
   
   
   return 0;
}


// closes connections, and cleans up
void proto_disconnect() 
{
   close(sockfd);
}


// requests a file to be sent
int proto_request_file(char *file)
{
   char *filename;

   filename = file;
   len = sizeof(p_in);
   
   // prepare a request packet
   outpacket.id = 0;
   outpacket.header = REQ;
   
   strcpy(outpacket.data, filename);  // store in data field of packet
   
   // send the request
   sendto (sockfd, (struct mypacket_t *)&outpacket, sizeof(outpacket),0,(struct sockaddr *)&p_out, sizeof(p_out));
   
   // wait for a reply
   recvfrom (sockfd, (struct mypacket_t *)&inpacket, sizeof(inpacket),0,(struct sockaddr*)&p_in, &len);
   
   if(inpacket.header == ERR) 
     {
	//printf("<e> request error\n"); // debug
	exit(1); // exit?
	close(sockfd);
	
	return -1;
     }
   
   if(inpacket.header == DATA)
     {
	// tell the harness somehow to write the chunk to disk.. but how?
	write_chunk(id, inpacket);
	// try to keep it modular
     }
   	 
	  
   return 0;
}



// serves a requested file
int proto_serve_file() 
{
   
   sockfd = socket(AF_INET, SOCK_DGRAM, 0);   // UDP datagrams
   if(sockfd == -1) {
      //printf("<e> server socket error\n");  // debug
      close(sockfd);
      return -1;  // return error
   }
   
   portused = DEFAULT_PORT; // set port number
   
   p_out.sin_family = AF_INET;         // internet family
   p_out.sin_addr.s_addr = INADDR_ANY;
   p_out.sin_port = htons(portused);   // port number
   
   
   // bind to port
   if( bind(sockfd,(struct sockaddr *)&p_out, sizeof(p_out)) )
     {
	//printf("<e> bind error\n"); // debug
	close(sockfd);
	return -1;
     }
   
   len = sizeof(p_in);
   
   // enter serving loop
   while(!quit) {
      
      // wait for a request packet
      recvfrom(sockfd, (struct mypacket_t *)&inpacket, sizeof(inpacket),0, (struct sockaddr *)&p_in, &len);
      
      //printf("<*> file request: %s\n", inpacket.data); // debug only
      id = inpacket.id;
      outpacket.header = 0;
      
      //req handler
      if(inpacket.header == REQ) {
	 // we have to get a chunk of file
	 //inpacket = read_chunk(id);
	 printf("request packet received\n");  // debug
      }
      
      // carry on serving after initial request-send
      
      //check outpacket header isn't ERR, and continue
      if(outpacket.header != ERR) 
	{
	   do {
	      // read a chunk of file
	      //inpacket = read_chunk(id);
	      // increment the id counter
	      id++;
	      
	      //send it across udp
	      sendto (sockfd, (struct mypacket_t *)&outpacket, sizeof(outpacket),0, (struct sockaddr *)&p_out, sizeof(p_out));
	      
	      // get an acknowledgment
	      recvfrom (sockfd, (struct mypacket_t *)&inpacket, sizeof(inpacket),0, (struct sockaddr *)&p_in, &len);

	   } while(outpacket.header != END || (inpacket.header != ERR && inpacket.id == 0));  // end of file, and no errors
	   // end of do..
   	   
	} // end while
   
      close(sockfd);
   }
   return 0;
   
}



// send a packet across the transport
void proto_send_packet(int id, int header, int size, char *data)
{
   sendto (sockfd, (struct mypacket_t *)&outpacket, sizeof(outpacket),0, (struct sockaddr *)&p_out, sizeof(p_out));
}


// receive a packet across the transport
void proto_recv_packet(int id, int header, int size, char *data)
{
   recvfrom (sockfd, (struct mypacket_t *)&inpacket, sizeof(inpacket),0, (struct sockaddr *)&p_in, &len);
}


// send an error packet, containing an error code, to a host
void proto_send_error(int id, char *code)
{
   struct mypacket_t p;
   
   p.id = 0;
   p.header = ERR;
   p.size = sizeof(code);
   strcpy(p.data, code);
   
   //send the error
   sendto (sockfd, (struct mypacket_t *)&p, sizeof(p),0,(struct sockaddr *)&p_out,sizeof(p_out));
   
}

// finishes the file transfer
void proto_finish()
{
   // signal that transfer has succesully completed, to the harness
   proto_disconnect();
   close(sockfd);
}
