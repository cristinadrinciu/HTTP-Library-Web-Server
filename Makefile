CC=g++
CCFLAGS=-Wall -Werror -Wno-error=unused-variable -g

build: client.cpp buffer.cpp helpers.cpp commands.cpp requests.cpp
	$(CC) $(CCFLAGS) -o client client.cpp buffer.cpp helpers.cpp commands.cpp requests.cpp

clean:
	rm -f client

pack:
	zip -r 324CA_Drinciu_Cristina_Tema4PC.zip Makefile client.cpp README.md buffer.cpp buffer.hpp helpers.cpp helpers.hpp commands.cpp commands.hpp requests.cpp requests.hpp