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
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ctype.h>
#include <unistd.h>

#include <pthread.h>
#include <curses.h>
#include <stdbool.h>
#include <zlib.h>
#include <curl/curl.h>
#include <libconfig.h>
#include <jsmn.h>

#include "tunnel.h"

char httpbuffer[9024];

void bError(char *msg) {
	//clear();
	attron(COLOR_PAIR(2));
	printw("\n[ERROR] ");
	attron(COLOR_PAIR(1));
	printw("%s\n\n",msg);
	printw("Press any key to the close program...");
	refresh();
	getch();
	endwin();
	exit(1);
}

void simplePrint(char *msg) {
	printw("%s",msg);
	refresh();
}

int fileExists(char *filename) {
	if(access(filename, F_OK) != -1) {
    	return 1;
	} else {
	    return 0;
	}
}

/*int fileExists(char *filename)
{
  	struct stat buffer;   
  	return (stat(filename, &buffer) == 0);
}*/

size_t curl_write( void *ptr, size_t size, size_t nmemb, void *stream)
{
	strcat(httpbuffer,(char*)ptr);
	return size*nmemb;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

CURL *mcLogin(CURL *shit, char *username, char *password)
{
	CURLcode res;

	curl_easy_setopt(shit, CURLOPT_WRITEFUNCTION, curl_write);
	curl_easy_setopt(shit, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(shit, CURLOPT_URL, "http://www.classicube.net/api/login");
	curl_easy_setopt(shit, CURLOPT_COOKIEJAR, "cookie.txt");
	res = curl_easy_perform(shit);

	if(res != CURLE_OK) {
		fprintf(stderr, "CURL ERROR:\t%s", curl_easy_strerror(res));
		exit(1);
	}

	jsmn_parser jp;
	jsmn_init(&jp);
	
	jsmntok_t tokens[256];
	if (jsmn_parse(&jp, httpbuffer, strlen(httpbuffer), tokens, 256) < 0) {
		bError("Failed to get login token");
	}

	//printw("token=%s\n",httpbuffer);
	//refresh();
	//getchar();

	int length = tokens[6].end - tokens[6].start;
	char token[length+1];
	memcpy(&token, &httpbuffer[tokens[6].start], length);
	token[length] = '\0';

	//printw("token: %s\n",token);
	//refresh();
	//getchar();
	//After we get the token we send the login request

	char post_login_str[256];
	sprintf(post_login_str, "username=%s&password=%s&token=%s", username, password, token);
	memset(&httpbuffer, 0, sizeof(httpbuffer));
	curl_easy_setopt(shit, CURLOPT_WRITEFUNCTION, curl_write);
	curl_easy_setopt(shit, CURLOPT_WRITEDATA, NULL);
	//curl_easy_setopt(shit, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(shit, CURLOPT_COOKIE, "cookie.txt");

	curl_easy_setopt(shit, CURLOPT_HTTPPOST, 1);
	curl_easy_setopt(shit, CURLOPT_POSTFIELDS, post_login_str);	
	curl_easy_setopt(shit, CURLOPT_URL, "http://www.classicube.net/api/login");

	res = curl_easy_perform(shit);

	

	if(res != CURLE_OK) {
		fprintf(stderr, "CURL ERROR:\t%s", curl_easy_strerror(res));
		exit(1);
	}

	return shit;
}

void apiInfo(CURL *mc, char *hashc) {
	char url[128], *parseptr, *parseptr_helper;
	static const char *url_fmt = "http://www.classicube.net/api/server/%s/";
	CURLcode res;
	memset(&httpbuffer, 0, sizeof(httpbuffer));

	curl_easy_setopt(mc, CURLOPT_WRITEFUNCTION, curl_write);
	curl_easy_setopt(mc, CURLOPT_HTTPGET, 1);
	//curl_easy_setopt(mc, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(mc, CURLOPT_COOKIE, "cookie.txt");
	sprintf(url, url_fmt, hashc);
	curl_easy_setopt(mc, CURLOPT_URL, url);
	res = curl_easy_perform(mc);

	if(res != CURLE_OK) {
		fprintf(stderr, "CURL ERROR:\t%s", curl_easy_strerror(res));
		exit(1);
	}

	jsmn_parser jp;
	jsmn_init(&jp);
	
	jsmntok_t tokens[256];
	if (jsmn_parse(&jp, httpbuffer, strlen(httpbuffer), tokens, 256) < 0) {
		bError("Failed to parse login info from webpage");
	}

	if (strstr(httpbuffer, "\"ip\": " ) == NULL) {

		bError("Failed to login, is your username/password correct?");
	}


	int length = tokens[3].end - tokens[3].start;
	char more[length+1];
	memcpy(&more, &httpbuffer[tokens[3].start], length);
	more[length] = '\0';

	jsmn_init(&jp);
	
	jsmntok_t kens[256];

	if (jsmn_parse(&jp, more, strlen(more), kens, 256) < 0) {
		bError("Failed to parse login info from webpage");
	}

	/*length = kens[2].end - kens[2].start;
	char hash[length+1];
	memcpy(&hash, &more[kens[2].start], length);
	hash[length] = '\0';
	printw("hash: %s.\n",hash);
	refresh();*/

	length = kens[8].end - kens[8].start;
	char ip[length+1];
	memcpy(&ip, &more[kens[8].start], length);
	ip[length] = '\0';
	strncpy(ipa, ip, strlen(ip));

	length = kens[12].end - kens[12].start;
	char jpass[length+1];
	memcpy(&jpass, &more[kens[12].start], length);
	jpass[length] = '\0';
	strncpy(mppass, jpass, strlen(jpass));

	length = kens[18].end - kens[18].start;
	char jport[length+1];
	memcpy(&jport, &more[kens[18].start], length);
	jport[length] = '\0';
	port = atoi(jport);

	/*printw("\nbuff: %s.\n",httpbuffer);
	refresh();
	getchar();
	printw("\ninfo: %s, %s, %d.\n",ipa, mppass, port);
	refresh();
	getchar();*/

}

int *mcParse(CURL *mc, char *hashc)
{
	char url[128], *parseptr, *parseptr_helper;
	static const char *url_fmt = "http://www.classicube.net/server/play/%s/";
	CURLcode res;
	memset(&httpbuffer, 0, sizeof(httpbuffer));

	curl_easy_setopt(mc, CURLOPT_WRITEFUNCTION, curl_write);
	curl_easy_setopt(mc, CURLOPT_HTTPGET, 1);
	//curl_easy_setopt(mc, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(mc, CURLOPT_COOKIE, "cookie.txt");
	sprintf(url, url_fmt, hashc);
	curl_easy_setopt(mc, CURLOPT_URL, url);
	res = curl_easy_perform(mc);

	if(res != CURLE_OK) {
		fprintf(stderr, "CURL ERROR:\t%s", curl_easy_strerror(res));
		exit(1);
	}

	if (strlen(httpbuffer) < 2) {
		printw("Error: http buffer is NULL!\n");
		exit(1);
	}

	if (strstr(httpbuffer, "<h1>Redirecting...</h1>") != NULL) {
		bError("Failed to login, is your username/password correct?");
	}

	if (strstr(httpbuffer, "502: Bad gateway") != NULL) {
		bError("Invalid server hash!");
	}

	char *mppasst = strstr(httpbuffer,"\"mppass\"");
	char mppassf[128];
	strncpy(mppassf, mppasst, 128);

	char *mppasst2 = strstr(mppassf, "value=");
	char mppass2[128];
	strncpy(mppass2, mppasst2, 128);
	

	char *split = strtok(mppass2, "\"");
	char *split2 = strtok(NULL, "\"");
	strncpy(mppass, split2, 128);

	char *ipt = strstr(httpbuffer,"\"server\"");
	char ip[128];

	strncpy(ip, ipt, 128);
	char *ipt2 = strstr(ip, "value=");
	char ip2[128];

	char *isplit = strtok(ipt2, "\"");
	char *isplit2 = strtok(NULL, "\"");

	char buffer2[9000];
	strncpy(buffer2, httpbuffer, strlen(httpbuffer)); 
	strncpy(ipa, isplit2, 128);  //This last call to strncpy destroys httpbuffer for some odd reason
								 //So I make a new buffer and copy httpbuffer to it

	char *portt = strstr(buffer2,"\"port\""); 
	char portf[128];
	refresh();
	strncpy(portf, portt, 128);


	char *portt2 = strstr(portf, "value=");
	char port2[128];
	strncpy(port2, portt2, 128);


	char *psplit = strtok(port2, "\"");
	char *psplit2 = strtok(NULL, "\"");
	char *portcunt;
	port = atoi(psplit2);


	return NULL;
}

int main() {

	//Start up ncurses and print the splash

	initscr();
	scrollok(stdscr,TRUE);
	start_color();

	init_pair(2, 12, 0); //Red on black
	init_pair(1, 7, 0); //White on black (default)
	init_pair(3, 10, 0); //Green on black
	init_pair(4, 11, 0); //Cyan on black
	
	printw("======================================================================\n");
	attron(COLOR_PAIR(2));
	printw(".____                      ___________                         .__ \n");
	printw("|    |   _____ ___  _______\\__    ___/_ __  ____   ____   ____ |  |\n");
	printw("|    |   \\__  \\  \\/ /\\__  \\  |    | |  |  \\/    \\ /    \\_/ __ \\|  | \n");
	printw("|    |___ / __ \\   /  / __ \\ |    | |  |  /   |  \\   |  \\  ___/|  |_\n");
	printw("|_______ (____  /\\_/  (____  /____| |____/|___|  /___|  /\\___  >____/\n");
	printw("        \\/    \\/           \\/                  \\/     \\/     \\/      \n");
	attron(COLOR_PAIR(1));
	printw("======================================================================\n\n");
	printw("Copyright (C) 2018 lavajoe, see LICENSE file for details regarding the GNU GPL V2 license.\n");
	refresh();

	//Load configuration file
	config_t cfg;

	int loginMethod = 0;

	config_init(&cfg);
	if (config_read_file(&cfg, "config/config.cfg") == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_string(&cfg, "version", &version) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_int(&cfg, "loginMethod", &loginMethod) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_bool(&cfg, "hax", &hax) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_string(&cfg, "haxType", &haxType) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_string(&cfg, "gimp", &gimpF) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}
	if (config_lookup_bool(&cfg, "CPE", &useCPE) == CONFIG_FALSE) {
		printw("Error parsing config file: %s.\n",config_error_text(&cfg));
		refresh();
	}

	char gimpFolder[32];
	strcpy(gimpFolder, gimpF);

	printw("Version %s.\n\n",version);
	refresh();

	//Load text arrays from their respective text files
	FILE *GIMPS;
	if (fileExists(gimpFolder)) {
		char gimpBuffer[128];
		gimpN = 1;
		GIMPS = fopen(gimpFolder, "r");
		char ch;
		while(!feof(GIMPS)) {
  			ch = fgetc(GIMPS);
  			if(ch == '\n') {
    			gimpN++;
  			}
  		}

  		fclose(GIMPS);
	  	GIMPS = fopen(gimpFolder, "r");
	  	int i = 1;
	  	for (i = 1; i <= gimpN; i++) {
			fgets(gimp[i], sizeof(gimp[i]), GIMPS);
			memset(gimp[i]+strlen(gimp[i])-1, '\0', 1);
		}
	} else {
		simplePrint("gimps.txt not found.\n");
		gimpN = -1;
	}

	FILE *IGNOREF;
	if (fileExists("config/ignore.txt")) {
		char gimpBuffer[128];
		ignoreN = 1;
		IGNOREF = fopen("config/ignore.txt", "r");
		char ch;
		while(!feof(IGNOREF)) {
  			ch = fgetc(IGNOREF);
  			if(ch == '\n') {
    			ignoreN++;
  			}
  		}

  		fclose(IGNOREF);

	  	IGNOREF = fopen("config/ignore.txt", "r");
	  	int i = 1;
	  	for (i = 1; i <= ignoreN; i++) {
			fscanf(IGNOREF, "%s\n", &ignores[i]);
		}
		fclose(IGNOREF);
	} else {
		simplePrint("config/ignore.txt not found, creating.\n");
		FILE *IGNOREF = fopen("config/ignore.txt", "ab+");
		ignoreN = 0;
	}

	//Check if info.txt exists then load info from it
	if (!fileExists("config/info.txt")) {
		simplePrint("info.txt does not exist, prompting user for input.\n");
		loginMethod = 1;
	} else {
		FILE *INFO;
		INFO = fopen("config/info.txt", "r");
		fscanf(INFO, "%s\n", &username);
		if (strlen(username) < 1) {
			simplePrint("info.txt is empty, prompting user for input.\n");
			loginMethod = 1;
		} else {
			fscanf(INFO, "%s\n", &password);
			if (strlen(password) < 1) {
				simplePrint("missing password in info.txt, prompting user for input.\n");
				loginMethod = 1;
			} else {
				fscanf(INFO, "%s\n", &hash);
				if (strlen(hash) < 1) {
					simplePrint("missing server hash in info.txt, prompting user for input.\n");
					loginMethod = 1;
				}
			}
		}
		fclose(INFO);
	}

	if (loginMethod == 0) {
		simplePrint("Using connection info from info.txt.\n");
	}

	bool dircon = false;
	if (loginMethod == 2) { //TODO: read from direct.txt
		dircon = true;
		FILE *DIRECT;
		DIRECT = fopen("config/direct.txt", "r");
		fscanf(DIRECT, "%s\n", &ipa);
		if (strlen(ipa) < 1) {
			simplePrint("direct.txt is empty, prompting user for input.\n");
			loginMethod = 1;
			dircon = false;
		} else {
			fscanf(DIRECT, "%d\n", &port);
			if (port == 0) {
				simplePrint("missing port in direct.txt, prompting user for input.\n");
				loginMethod = 1;
				dircon = false;
			} else {
				fscanf(DIRECT, "%s\n", &username);
				if (strlen(username) < 1) {
					simplePrint("missing username in direct.txt, prompting user for input.\n");
					loginMethod = 1;
					dircon = false;
				} else {
					fscanf(DIRECT, "%s\n", &mppass);
					if (strlen(mppass) < 1) {
						simplePrint("missing mppass in direct.txt, prompting user for input.\n");
						loginMethod = 1;
						dircon = false;
					}
				}
			}
		}
		fclose(DIRECT);
	}

	if (loginMethod == 1) {
		char iusername[64];
		char ipassword[64];
		char ihash[64];

		printw("Right click on top of the window and hover over edit to copy/paste in console.\n\n");
		printw("username (%s): ",username);
		refresh();
		getstr(iusername);
		printw("password (%s): ",password);
		refresh();
		getstr(ipassword);
		printw("hash (%s)\n-: ",hash);
		refresh();
		getstr(ihash);

		bool changed = false;

		if (strlen(iusername) > 1) {
			changed = true;
			strcpy(username, iusername);
		}
		if (strlen(ipassword) > 1) {
			changed = true;
			strcpy(password, ipassword);
		}
		if (strlen(ihash) > 1) {
			changed = true;
			strcpy(hash, ihash);
		}

		if (changed) {
			simplePrint("Save to info.txt? (y/n): ");
			char choice = getch();
			choice = tolower(choice);

			if (choice == 'y') {
				if (fileExists("config/info.txt")) {
					remove("config/info.txt");
				}

				char tinfo[strlen(username)+strlen(password)+strlen(hash)+2];
				sprintf(tinfo, "%s\n%s\n%s",username,password,hash);
				FILE *pinfo = fopen("config/info.txt", "wb");
				fwrite(tinfo, strlen(tinfo), 1, pinfo);	
				fclose(pinfo);
			}
		}

		simplePrint("\n\n");
	}

	if (!dircon) {
		simplePrint("Logging in to classicube.net...");
		CURL *mc;
		mc = curl_easy_init();
		mc = mcLogin(mc, username, password);
		apiInfo(mc, hash);
		curl_easy_cleanup(mc);
		simplePrint("Success!\n");
		char direct[70];
		sprintf(direct, "%s\n%d\n%s\n%s",ipa,port,username,mppass);
		FILE *DIRECT = fopen("config/direct.txt", "w");
		fwrite(direct, strlen(direct), 1, DIRECT);
		printw("Wrote direct connect url to config/direct.txt\n");	
	} else {
		simplePrint("Skipping login and direct connecting.\n");
	}

	//After dealing with getting the info (ip port, ect.) we make a thread for the tunnel server which
	//will wait for the player to connect.  The original thread here will be used for console I/O

	pthread_t serverT;
    if (pthread_create(&serverT,NULL,&tunnelServer, 0) != 0) {
        bError("Failed to create server thread!");
    }

    char cInput[64];

    while (1) {

    	if (getch() == 't') {
    		echo();
   			printw("[LavaTunnel]: ");
   			refresh();
   			getstr(cInput);
   			
   			if (cInput[0] == '\'') {
   				char command[64];
                memcpy(command, cInput+1, strlen(cInput) - 1); //Remove the ' at the beginning of the string
                memset(command+strlen(cInput) - 1, '\0', 1);

                if (commandFunc(command, true) == false) {
                   simplePrint("[LavaTunnel] Unknown command.\n");
                }
             }

   			noecho();
    	}

    	Sleep(1);
    }

	return 0;

}