// http://johnnie.jerrata.com/winsocktutorial/
// gcc server.c -o server.exe -lws2_32

#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

int  MessageEventListener();
void ErrorPrompt(int, const char *);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow) {

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
    ErrorPrompt(nret, "socket()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  SOCKADDR_IN serverInfo;
  serverInfo.sin_family      = AF_INET;
  serverInfo.sin_addr.s_addr = inet_addr("192.168.85.99");
  serverInfo.sin_port        = htons(16162); //htons() translates an unsigned short integer into network byte order.
                                             //https://www.ibm.com/docs/ja/zvm/7.2?topic=domains-network-byte-order-host-byte-order

  nret = bind(listeningSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));

  if (nret == SOCKET_ERROR) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "bind()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  nret = listen(listeningSocket, 10);

  if (nret == SOCKET_ERROR) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "listen()");

    WSACleanup();
    return NETWORK_ERROR;

  } else {
    printf("\nServer listening on port: %hu", serverInfo.sin_port);
  }

  SOCKET server;
  server = accept(listeningSocket, 
                  NULL,   // Optional, address of a SOCKADDR_IN struct
                  NULL);  // Optional, address of variable containing sizeof(struct SOCKADDR_IN)
  
  if (server == INVALID_SOCKET) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "accept()");

    WSACleanup();
    return INVALID_SOCKET;
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
    ErrorPrompt(*nret, "recv()");

    WSACleanup();
    return SOCKET_ERROR;

  } else {
    printf("\nReceived message: %s\r", h_str_buffer);
    MessageEventListener(nret, server);
  }

}

void ErrorPrompt(int errorCode, const char *func) {

  char errorMsg[92];

  ZeroMemory(errorMsg, 92);
  sprintf(errorMsg, "Call to %s returned error %d", (char *)func, errorCode);

  MessageBox(NULL, errorMsg, "socketIndication_Server", MB_OK);
}