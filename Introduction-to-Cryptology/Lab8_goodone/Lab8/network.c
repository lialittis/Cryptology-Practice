/**************************************************************************/
/*                                                                        */
/* network                                                                */
/*                                                                        */
/* Allows to send and receive text messages over TCP sockets. Each        */
/* message is sent in a separate TCP session, no state is maintained      */
/* between connections. Both sending and receiving are blocking, but      */
/* receiving allows for a timeout.                                        */
/*                                                                        */
/* Original author: Andreas Enge, 17.03.2006                              */
/* Modified: FMorain, 2017                                                */
/*                                                                        */
/**************************************************************************/


#define NG_MAXCONNECTIONS 5
#define NG_PORT 1717
#define NO_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEBUG


static int sock;
   /* socket for listening to incoming connections */


/**************************************************************************/
/*                                                                        */
/* helper functions                                                       */
/*                                                                        */
/**************************************************************************/

static void myerror (const char * message)
{
   perror (message);
   exit (1);
}
      
/**************************************************************************/

static void myherror (const char * message)
{
   herror (message);
   exit (1);
}
      
/**************************************************************************/

static void myrcverror (const char * message, int code)
{
   if (code == -1)
      myerror (message);
   else if (code == 0)
   {
      printf ("Peer closed connection: %s", message);
      exit (1);
   }
}
      
/**************************************************************************/

static int send_bytes (int sock, const char * mesg, int bytes)
   /* sends out the given number of bytes from mesg; returns the number
      of bytes upon success, and -1 upon error */
{
   const char * buffer = mesg;
   int total = bytes, tmp;
   
   while (bytes > 0)
   {
      tmp = send (sock, buffer, bytes, 0);
      if (tmp == -1)
         return -1;
      bytes -= tmp;
      buffer += tmp;
   }
      
   return total;
}

/**************************************************************************/

static int recv_bytes (int sock, char * mesg, int bytes)
   /* receives the given number of bytes in mesg; returns the number
      of bytes upon success, -1 upon error and 0 upon closed connection;
      mesg must already be initialised */
{
   char * buffer = mesg;
   int total = bytes, tmp;
   
   while (bytes > 0)
   {
      tmp = recv (sock, buffer, bytes, 0);
      if (tmp <= 0)
         return -1;
      bytes -= tmp;
      buffer += tmp;
   }
      
   return total;
}

/**************************************************************************/

static int send_message (int sock, const char * mesg)
   /* sends out the message without the '\0' termination prepended by four
      bytes in network byte order giving its length; returns the length of
      the message upon success, and -1 upon error */
{
   unsigned long int length;
   
   length = htonl (strlen (mesg));
      /* the 0 termination is not sent */
   if (send_bytes (sock, (char *) &length, 4) == -1)
      myerror ("send_message send_bytes");
   
   return send_bytes (sock, mesg, ntohl (length));
}

/**************************************************************************/

static int recv_message (int sock, char ** mesg)
   /* receives a message that was prepended by its length; allocates the
      necessary memory and adds a '\0' termination; returns the length
      of the message upon success, -1 upon error and 0 upon closed
      connection */
{
   unsigned long int length;
   int error;
   
   length = 0;
   error = recv_bytes (sock, (char *) &length, 4);
   if (error <= 0)
      myrcverror ("recv_message recv_bytes", error);
   length = ntohl (length);
   
   *mesg = (char *) malloc (length + 1);
   (*mesg) [length] = '\0';
   
   return recv_bytes (sock, *mesg, length);
}

/**************************************************************************/
/*                                                                        */
/* exported functions                                                     */
/*                                                                        */
/**************************************************************************/

void network_init (int nport)
   /* starts to listen for connections; is only needed when receiving
      messages, not for sending
      ATTENTION: Cannot be called by two processes on the same machine,
      since a fixed port is blocked. */

{
   struct sockaddr_in addr;
   int yes;
   
   sock = socket (PF_INET, SOCK_STREAM, 0);
   if (sock == -1)
      myerror ("network_init socket");
   
   yes = 1;
   if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int))
         == -1)
      myerror ("network_init setsockopt");

   addr.sin_family = AF_INET;
   //   addr.sin_port = htons (NG_PORT);
   addr.sin_port = htons (nport);
   addr.sin_addr.s_addr = INADDR_ANY;
   memset (addr.sin_zero, 0, 8);
   
   if (bind (sock, (struct sockaddr *) &addr, sizeof (struct sockaddr))
         == -1)
      myerror ("network_init bind");
   
   if (listen (sock, NG_MAXCONNECTIONS) == -1)
      myerror ("network_init listen");
}

/**************************************************************************/

void network_clear ()
   /* cleans up, must be called after network_init */

{
   close (sock);
}

/**************************************************************************/

void network_send (const char * host, const int port, const char * mesg)
   /* sends the given message to the host of the given name */
      
{
   int sendsock, bytes_sent, yes;
   struct hostent * host_receiver;
   struct sockaddr_in receiver;
   
   sendsock = socket (PF_INET, SOCK_STREAM, 0);
   if (sock == -1)
      myerror ("network_send socket");
   
   yes = 1;
   if (setsockopt (sendsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int))
         == -1)
      myerror ("network_send setsockopt");

   host_receiver = gethostbyname (host);
   if (host_receiver == NULL)
      myherror ("network_send gethostbyname");
   
   receiver.sin_family = AF_INET;
   receiver.sin_port = htons (port == 0 ? NG_PORT : port);
   receiver.sin_addr = *((struct in_addr *) (host_receiver->h_addr_list [0]));
   memset (receiver.sin_zero, 0, 8);
   
   if (connect (sendsock, (struct sockaddr *) &receiver, sizeof (struct
         sockaddr)) == -1)
      myerror ("network_send connect");
#ifdef DEBUG
   printf ("Connection to %s:%hi\n", inet_ntoa (receiver.sin_addr),
         receiver.sin_port);
#endif
   
   bytes_sent = send_message (sendsock, mesg);
   if (bytes_sent == -1)
      myerror ("network_send send_bytes");
   
   close (sendsock);
}

/**************************************************************************/

char * network_recv (int timeout)
   /* waits up to timeout seconds for a message and returns it;
      if no message has arrived, returns NULL. If timeout is
      negative, waits indefinitely for a message */

{
   char * mesg;
   int recsock, bytes_received, err;
   fd_set set;
   struct timeval waiting;
   unsigned int sendersize;
   struct sockaddr_in sender;
   
   FD_ZERO (&set);
   FD_SET (sock, &set);
   if (timeout >= 0)
   {
      waiting.tv_sec = timeout;
      waiting.tv_usec = 0;
      err = select (sock + 1, &set, NULL, NULL, &waiting);
   }
   else
      err = select (sock + 1, &set, NULL, NULL, NULL);
   if (err == -1)
      myerror ("network_receive select");
   if (!FD_ISSET (sock, &set))
      return NULL;
   
   sendersize = sizeof (struct sockaddr_in);
   recsock = accept (sock, (struct sockaddr *) &sender, &sendersize);
   if (recsock == -1)
      myerror ("network_receive accept");
#ifdef DEBUG
      printf ("Incoming connection from %s:%hi\n",
            inet_ntoa (sender.sin_addr), sender.sin_port);
#endif
   
   bytes_received = recv_message (recsock, &mesg);
   if (bytes_received <= 0)
      myrcverror ("network_receive recv_bytes", bytes_received);
   
   close (recsock);
   
   return (mesg);
}

/**************************************************************************/
