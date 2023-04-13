#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <unistd.h>

#define NETWORK_ERROR -1
#define NETWORK_OK     0

void ErrorPrompt(int, const char *);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow) {

  WORD sockVersion;
  WSADATA wsaData;
  int nret;

  sockVersion = MAKEWORD(1, 1);
  WSAStartup(sockVersion, &wsaData);

  LPHOSTENT hostEntry;
  //hostEntry = gethostbyname("localhost"); // Specify server by name
                                               // Alternative: gethostbyaddr(), see example below

  /* Host by address example */

  IN_ADDR iaHost;
  iaHost.s_addr = inet_addr("127.0.0.1");

  hostEntry = gethostbyaddr((const char *) &iaHost, 
                            sizeof(struct in_addr), 
                            AF_INET);

  if (!hostEntry) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "gethostname()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  SOCKET client;
  client = socket(AF_INET, 
                  SOCK_STREAM, 
                  IPPROTO_TCP);

  if (client == INVALID_SOCKET) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "socket()");

    WSACleanup();
    return NETWORK_ERROR;
  }

  SOCKADDR_IN serverInfo;
  serverInfo.sin_family = AF_INET;
  serverInfo.sin_addr   = *((LPIN_ADDR) *hostEntry->h_addr_list);
  serverInfo.sin_port   = htons(16162);

  nret = connect(client, 
                (LPSOCKADDR)&serverInfo, 
                sizeof(struct sockaddr));

  if (nret == SOCKET_ERROR) {

    nret = WSAGetLastError();
    ErrorPrompt(nret, "connect()");

    WSACleanup();
    return NETWORK_ERROR;
  } else {
    printf("Successfully connected to %s", hostEntry->h_name);
  }

  /* Stack allocation

  char buffer[256];
  ZeroMemory(buffer, 256);
  strcpy(buffer, "Test Data");

  nret = send(client, 
              buffer, 
              strlen(buffer), 
              0);

  */

  /* Heap allocation
  char *h_str_buffer;
  h_str_buffer = (char *) malloc(256 * sizeof(char));
  */

  int i;
  for (i = 1; i <= 100; i++) {

    char str_buffer[256];
    ZeroMemory(str_buffer, 256);
    sprintf(str_buffer, "Test Data: %d", i);

    nret = send(client, 
                str_buffer, 
                256, 
                0);
    
    if (nret == SOCKET_ERROR) {

      nret = WSAGetLastError();
      ErrorPrompt(nret, "send()");

      WSACleanup();
      return SOCKET_ERROR;
    }
  }

  sleep(1); // Hacky way of keeping connection open after application has ended
  
  //free(h_str_buffer);

  //closesocket(client); // Server-side recv() will receive 0 bytes if connection is closed, reference: https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recv
  //WSACleanup(); 
}

void ErrorPrompt(int errorCode, const char *func) {
  
  char errorMsg[92];

  ZeroMemory(errorMsg, 92);

  sprintf(errorMsg, "Call to %s returned error %d", (char *)func, errorCode);

  MessageBox(NULL, errorMsg, "socketIndication", MB_OK);
}

// gcc client.c -o client.exe -lws2_32