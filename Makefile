CC = g++
NAME = minesweeper

all: compile run

compile:
	$(CC)  main.cpp minesweeper.cpp -I include -L lib -o2 -lsfml-graphics -lsfml-window -lsfml-system -o $(NAME)

run:
	.\$(NAME)