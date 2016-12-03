/* 
 * 
 * S.Bradley 2003
 * 
 * CNET308 File transfer protol header
 *
 */

// includes
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Headers
#define REQ   100
#define ACK   101
#define ERR   102
#define END   103
#define DATA  104

#define DEFAULT_PORT 2008        // port number to use for communication
#define CHUNK_SIZE 1024          // 1kilobyte chunks


// error defines
#define CANTOPENFILE  1
#define CANTREADFILE  2
#define CANTWRITEFILE 3


// function prototypes
int  proto_connect(char *host, int port);
void proto_disconnect();
int  proto_request_file(char *file);
int  proto_serve_file();
void proto_send_packet(int id, int header, int size, char *data);
void proto_recv_packet(int id, int header, int size, char *data);
void proto_send_error(int id, char *code);
void proto_finish();


// my packet structure design
struct mypacket_t {
  int id;                    // packet ID
  int header;                // packet Header
  int size;                  // size of the data field below
  
  char data[CHUNK_SIZE];     // data buffer
};



