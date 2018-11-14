/*
	Copyright (C) 2018  lavajoe <joe89gunnar@gmail.com)

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <winsock2.h>
#include <curses.h>
#include <stdbool.h>
#include <ctype.h>

#include "tunnel.h"

//Protocol functions:

void tunnelTeleport(short x, short y, short z, char yaw, char pitch) { //Send a 'teleport' packet to the client
  unsigned char buf[6];

  x = htons(x);
  y = htons(y);
  z = htons(z);

  memcpy(&buf[0], &x, sizeof(short));
  memcpy(&buf[2], &y, sizeof(short));
  memcpy(&buf[4], &z, sizeof(short));

  unsigned char p = 0x08;

  sendW(serverSock, &p, 1, 0);
  p = 255;
  sendW(serverSock, &p, 1, 0);
  sendW(serverSock, &buf[0], 2, 0);
  sendW(serverSock, &buf[2], 2, 0);
  sendW(serverSock, &buf[4], 2, 0);
  sendW(serverSock, &yaw, 1, 0);
  sendW(serverSock, &pitch, 1, 0);   
}

void tunnelMove(short x, short y, short z, char yaw, char pitch) { //Position update
  unsigned char buf[6];

  x = htons(x);
  y = htons(y);
  z = htons(z);

  memcpy(&buf[0], &x, sizeof(short));
  memcpy(&buf[2], &y, sizeof(short));
  memcpy(&buf[4], &z, sizeof(short));

  unsigned char p = 0x08;
  sendW(tunnelSock, &p, 1, 0);
  p = 255;
  sendW(tunnelSock, &p, 1, 0);
  sendW(tunnelSock, &buf[0], 2, 0);
  sendW(tunnelSock, &buf[2], 2, 0);
  sendW(tunnelSock, &buf[4], 2, 0);
  sendW(tunnelSock, &yaw, 1, 0);
  sendW(tunnelSock, &pitch, 1, 0);   
}

void tunnelSetBlock(short x, short y, short z, char bid) { //Place a block, clientside
  unsigned char buf[6];

  x = htons(x);
  y = htons(y);
  z = htons(z);
  memcpy(&buf[0], &x, sizeof(short));
  memcpy(&buf[2], &y, sizeof(short));
  memcpy(&buf[4], &z, sizeof(short));

  unsigned char p = 0x06;
  sendW(serverSock, &p, 1, 0);
  sendW(serverSock, &buf[0], 2, 0);
  sendW(serverSock, &buf[2], 2, 0);
  sendW(serverSock, &buf[4], 2, 0);
  sendW(serverSock, &bid, 1, 0);
}

void setBlock(short x, short y, short z, char bid, bool sendpos) { //Place a block
  unsigned char buf[6];
  if (sendpos) {
	tunnelSetBlock(x,y,z,bid);
	short cunt = y*32;
	cunt + 64;
	Sleep(5);
    tunnelMove(x*32, cunt, z*32, 0,0);
	tunnelTeleport(x*32, cunt, z*32, 0,0);
    //Sleep(2); //Wait until he thinks you're next to the block
  }
  //printw("Z: %hd, %hd, %hd.\n",x,y,z);
  x = htons(x);
  y = htons(y);
  z = htons(z);
  memcpy(&buf[0], &x, sizeof(short));
  memcpy(&buf[2], &y, sizeof(short));
  memcpy(&buf[4], &z, sizeof(short));

  unsigned char p = 0x05;
  sendW(tunnelSock, &p, 1, 0);
  sendW(tunnelSock, &buf[0], 2, 0);
  sendW(tunnelSock, &buf[2], 2, 0);
  sendW(tunnelSock, &buf[4], 2, 0);
  if (bid == 0) {
    p = 0;
    bid = 1;
  } else { 
    p = 0x01;
  }
  sendW(tunnelSock, &p, 1, 0);
  sendW(tunnelSock, &bid, 1, 0);
  
  //send the placed block back to the client
					/*char morty = 0x05;
                    sendW(tunnelSock, &morty, 1, 0);
                    sendW(tunnelSock, &buf[0], 2, 0);
                    sendW(tunnelSock, &buf[2], 2, 0);
                    sendW(tunnelSock, &buf[4], 2, 0);
                    sendW(tunnelSock, &p, 1, 0);
                    sendW(tunnelSock, &bid, 1, 0);*/
	
}

void sendMessage(char pid, char *msg) { //Send a message to the server
  char mcMsg[64];
  toMCString(mcMsg, msg); //Convert to 'MC string'

  char p = 0x0d;
  sendW(tunnelSock, &p, 1, 0);
  sendW(tunnelSock, &pid, 1, 0);
  sendW(tunnelSock, mcMsg, 64, 0);
}

//Wrappers:

int recvW(int socket, char *buffer, int size, int flags) { //A wrapper for recv() that fixes recv() not downloading all of a packet
														                               
  int return_value;

  int bytes_so_far = 0;

  while (bytes_so_far < size) {

    return_value = recv(socket, buffer + bytes_so_far, size - bytes_so_far, flags);
    if (return_value < 0) {

      //Socket error, network error, programming error, etc.

      return -1;

    } else if (return_value == 0) {

      //Socket was closed by server.

      return bytes_so_far;

    } else {

      bytes_so_far += return_value;

    };

  };

  return bytes_so_far;

};

int sendW(int socket, char *buffer, int size, int flags) {
  int return_value;

  int bytes_so_far = 0;

  while (bytes_so_far < size) {

    return_value = send(socket, buffer + bytes_so_far, size - bytes_so_far, flags);
    if (return_value < 0) {

      //Socket error, network error, programming error, etc.

      return -1;

    } else if (return_value == 0) {

      //Socket was closed by server.

      return bytes_so_far;

    } else {
      bytes_so_far += return_value;

    };

  };

  return bytes_so_far;

};

int calculateOffset(short x, short y, short z) {
	return y*(lvlx * lvlz) + z*lvlz + x;
}

//String manipulation:

void toMCString(char *destination, char *source) { //Makes string 64bytes long to please Mojang
  memset(destination, ' ', 64);
  int length = strlen(source);
  if (length > 64) length = 64;
  strncpy(destination, source, length);
};

void strLower(char *destination, char *source) { //Converts string to lowercase
  int i = 0;
  for (i = 0; i < strlen(source); i++) {
    destination[i] = tolower(source[i]);
  }
}

void nullTerm(char *source) {
  int i = 0;
  int j = 0;

  for (i = 0; i < strlen(source); i++) {
    if (source[i] == ' ' && source[i+1] == ' ') {
      source[i] = '\0';
      break;
    }
  }
}

void cleanString(char *destination, char *source) { //Removes color codes and null-terminates string

	int i = 0;
	int j = 0;
	int size = 0;

	for (i = 0; i < 64; i++) {
		if (source[i] == ' ' && source[i+1] == ' ') {
			size = i;
			break;
		}
	}

	for (i = 0; i < size; i++) {
		if (source[i] == '&') {
			i++;
		} else {
			destination[j] = source[i];
			j++;
		}
	}

	destination[size] = '\0';
}

bool strEqual(char *equals, char *str, bool caseSensitive) { //Checks if the beginning of str is equal to equals
    char equalTo[256];
    char str2[256];

    if (caseSensitive) {
      strcpy(equalTo, equals);
      strcpy(str2, str);
    } else {
      strLower(equalTo, equals);
      strLower(str2, str);
    }

    int i = 0;
    // i < strlen(equalTo) - 1
    for (i = 0; i < strlen(equalTo); i++) {
      //printw("%c, %c\n",str2[i],equalTo[i]);
      //refresh();
        if (str2[i] != equalTo[i]) {
            return false;
        }
    }

    return true;
}

void lavaCuck(char *destination, char *source) { //replaces - with +, making stuff like -hax turn into +hax lmao

  int i = 0;
  int j = 0;
  int size = 0;

  for (i = 0; i < size; i++) {
    if (source[i] == '-') {
      destination[i] = '+';
    } else {
      destination[i] = source[i];
    }
  }

}