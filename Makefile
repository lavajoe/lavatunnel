main: main.c
	cls
	gcc CPE.h tunnel.h main.c command.c helper.c build.c client.c server.c -o lavatunnel -std=c11 -lcurl.dll -lz -lws2_32 -lwldap32 -lpthreadGC-3 -lpdcurses -lconfig.dll -ljsmn