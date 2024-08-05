all: server.c socket.c socket.h game.c game.c board.* player.*
	gcc *.c -lpthread -o server

debug: server.c socket.c socket.h game.c game.c board.* player.*
	gcc *.c -g -lpthread -o server
