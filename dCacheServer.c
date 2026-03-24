#include"communication/message.h"
#include"storage/datastorage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9095
#define MAX_QUEUE 4
#define BUFFER_SIZE 1024
#define BUCKETSNUM 997

typedef struct {
    int client_socket_fd;
    struct sockaddr_in client_address;
} ConnectionInfo;

ClientEntry *clientArray;

ObjectNode* getObject(char *clientId, char key[17], ClientEntry* clientArray);

void *serveRequest(void *connectionInfo) {
  char buffer[BUFFER_SIZE];
  int valread = read(clientSocket, buffer, BUFFER_SIZE);

  if (valread > 0) {
    buffer[valread] = '\0';
    struct sockaddr_in peerAddr;
    socklen_t peerAddrlen = sizeof(peerAddr);
    char clientIp[16], clientPort[6];

    if (getpeername(clientSocket, (struct sockaddr*)&peerAddr, &peerAddrlen) == 0) {
      strcpy(clientIp, inet_ntoa(peerAddr.sin_addr));
      sprintf(clientPort, "%d", ntohs(peerAddr.sin_port));
    } else {
      exit(1);
    }

    char* clientId = buildClientId(clientIp, clientPort);

    Message rcvdmsg = unwindMessage(buffer);
    switch (rcvdmsg.type) {
      case 'a':
        storeObject(clientArray, clientId, rcvdmsg.key, rcvdmsg.data, BUCKETSNUM);
        printf("stored object '%s'\n", rcvdmsg.key);
        break;

      case 'b':
        ObjectNode* node = getObject(clientId, rcvdmsg.key, clientArray);
        if (node == NULL) {
          strcpy(sendmsg, buildMessage(createMessage('b', rcvdmsg.id, rcvdmsg.key, "")));
          printf("No object found.");
        } else {
          strcpy(sendmsg, buildMessage(createMessage('b', rcvdmsg.id, rcvdmsg.key, node->data)));
          printf("Sending data...");
        }
        send(clientSocket, sendmsg, strlen(sendmsg), 0);
        break;

      case 'c':
        deleteObject(clientArray, clientId, rcvdmsg.key, BUCKETSNUM);
        break;

      case 'z':
        int index = hashClientId(clientId, BUCKETSNUM);
        while (clientArray[index].objectList != NULL) {
          deleteObject(clientArray, clientId, clientArray[index].objectList->key, BUCKETSNUM);
        }
        break;
    }
  } else if (valread == 0) {
    printf("connection unsuccessful...\n");
  } else {
    perror("read error");
  }


}

int main(){
  //initialization steps
  clientArray = malloc(BUCKETSNUM * sizeof(ClientEntry));                               // initializes clients array

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0), clientSocket;      // sets up the server socket at port 9095
  struct sockaddr_in server_addr, client_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  int addrlen = sizeof(server_addr);

  bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr));                    // bind serverSocket

  // starts listening for clients
  listen(serverSocket, MAX_QUEUE);
  printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
            perror("Connection unsuccessful.");
        }
        printf("connection established... passing to thread.\n");

        ConnectionInfo* info = malloc(sizeof(ConnectionInfo));
        if (info == NULL){
            perror("Could not initialize thread.");
            continue;
        }
        info->client_socket_fd = clientSocket;
        info->client_address = client_addr;

        pthread_t tid;
        pthread_create(&tid, NULL, &serveRequest, info);

        //while (1) {
      //   int valread = read(clientSocket, buffer, BUFFER_SIZE);
      //   if (valread > 0) {
      //     buffer[valread] = '\0';
      //     struct sockaddr_in peerAddr;
      //     socklen_t peerAddrlen = sizeof(peerAddr);
      //     char clientIp[16], clientPort[6];
      //
      //     if (getpeername(clientSocket, (struct sockaddr*)&peerAddr, &peerAddrlen) == 0) {
      //       strcpy(clientIp, inet_ntoa(peerAddr.sin_addr));
      //       sprintf(clientPort, "%d", ntohs(peerAddr.sin_port));
      //     } else {
      //       // handle error
      //       exit(1);
      //     }
      //
      //     char* clientId = buildClientId(clientIp, clientPort);
      //
      //     Message rcvdmsg = unwindMessage(buffer);
      //     switch (rcvdmsg.type) {
      //       case 'a':
      //         storeObject(clientArray, clientId, rcvdmsg.key, rcvdmsg.data, BUCKETSNUM);
      //         printf("stored object '%s'\n", rcvdmsg.key);
      //         break;
      //
      //       case 'b':
      //         ObjectNode* node = getObject(clientId, rcvdmsg.key, clientArray);
      //         if (node == NULL) {
      //           strcpy(sendmsg, buildMessage(createMessage('b', rcvdmsg.id, rcvdmsg.key, "")));
      //           printf("No object found.");
      //         } else {
      //           strcpy(sendmsg, buildMessage(createMessage('b', rcvdmsg.id, rcvdmsg.key, node->data)));
      //           printf("Sending data...");
      //         }
      //         send(clientSocket, sendmsg, strlen(sendmsg), 0);
      //         break;
      //
      //       case 'c':
      //         deleteObject(clientArray, clientId, rcvdmsg.key, BUCKETSNUM);
      //         break;
      //
      //       case 'z':
      //         int index = hashClientId(clientId, BUCKETSNUM);
      //         while (clientArray[index].objectList != NULL) {
      //           deleteObject(clientArray, clientId, clientArray[index].objectList->key, BUCKETSNUM);
      //         }
      //         break;
      //     }
      //   } else if (valread == 0) {
      //     printf("connection unsuccessful...\n");
      //   } else {
      //     perror("read error");
      //   }
      //
      //   memset(buffer, 0, BUFFER_SIZE);
      // }
    }

  return 0;
}

ObjectNode* getObject(char *clientId, char key[17], ClientEntry* clientArray) {
  int index = hashClientId(clientId, BUCKETSNUM);

  if (strcmp(clientArray[index].clientId, clientId) == 0) {
    for (ObjectNode* node = clientArray[index].objectList; node != NULL; node = node->next) {
      if (strcmp(node->key, key) == 0) {
        return node;
      }
    }
    return NULL;
  } else {
    for (int i = index + 1; i < BUCKETSNUM; i++) {
      if (strcmp(clientArray[i].clientId, clientId) == 0) {
        for (ObjectNode* node = clientArray[i].objectList; node != NULL; node = node->next) {
          if (strcmp(node->key, key) == 0) {
            return node;
          }
        }
        return NULL;
      }
    }
  }

  return NULL;
}