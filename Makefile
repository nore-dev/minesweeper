all: compile link

compile:
	g++ -I include -c main.cpp -Wall -Wextra -O2

link:
	g++ main.o -o minesweeper -L lib -l sfml-graphics -l sfml-window -l sfml-system