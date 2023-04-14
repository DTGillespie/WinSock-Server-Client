#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

int  MessageEventListener();
void LogError(int, const char *);

int main() {

  WORD sockVersion;
  WSADATA wsaData;
  int nret;

  sockVersion = MAKEWORD(1, 1);
  WSAStartup(sockVersion, &wsaData);

  SOCKET listeningSocket;
  listeningSocket = socket(AF_INET, 
                           SOCK_STREAM, 
                           IPPROTO_TCP);
  
  if (listeningSocket == INVALID_SOCKET) {

    nret = WSAGetLastError();
    LogError(nret, "socket()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  SOCKADDR_IN serverInfo;

  serverInfo.sin_family      = AF_INET;

  serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");  // INADDR_ANY | inet_addr("127.0.0.1") | htonl(INADDR_LOOPBACK);

  serverInfo.sin_port        = htons(16162);            // htons() translates an unsigned short integer into network byte order.
                                                        // https://www.ibm.com/docs/ja/zvm/7.2?topic=domains-network-byte-order-host-byte-order

  nret = bind(listeningSocket, (LPSOCKADDR) &serverInfo, sizeof(struct sockaddr));

  if (nret == SOCKET_ERROR) {

    nret = WSAGetLastError();
    LogError(nret, "bind()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  nret = listen(listeningSocket, 10);

  if (nret == SOCKET_ERROR) {

    nret = WSAGetLastError();
    LogError(nret, "listen()");

    WSACleanup();
    return NETWORK_ERROR;

  }
  
  printf("Server listening on port: %hu\n", serverInfo.sin_port);

  SOCKET server;
  server = accept(listeningSocket, 
                  NULL,   // Optional, address of a SOCKADDR_IN struct
                  NULL);  // Optional, address of variable containing sizeof(struct SOCKADDR_IN)

  if (server == INVALID_SOCKET) {

    nret = WSAGetLastError();
    LogError(nret, "accept()");

    WSACleanup();
    return (int) INVALID_SOCKET;
  } 
  
  int status = MessageEventListener(&nret, &server);

  closesocket(server);
  closesocket(listeningSocket);

  WSACleanup();

  if (status != 0) {
    return status;
  } else {
    return NETWORK_OK;
  }
}

int MessageEventListener(int *nret, SOCKET *server) {

  char *h_str_buffer;
  h_str_buffer = (char *) malloc(256 * sizeof(char));

  *nret = recv(*server, 
              h_str_buffer, 
              256, 
              0);

  if (*nret == SOCKET_ERROR) {

    *nret = WSAGetLastError();
    LogError(*nret, "recv()");

    WSACleanup();
    return SOCKET_ERROR;

  } else {
    printf("\nReceived message: %s\r", h_str_buffer);
    MessageEventListener(nret, server);
  }
}

void LogError(int errorCode, const char *func) {
  printf("Call to %s returned error %d", (char *)func, errorCode);
}

// http://johnnie.jerrata.com/winsocktutorial/
// gcc server.c -o server.exe -lws2_32