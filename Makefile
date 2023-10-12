compile:
	gcc ./src/game.c  -Wall -pthread -lSDL2 -lSDL2main -o build/game

easy:
	./build/game 0

medium:
	./build/game 1

hard:
	./build/game 2

clean:
	rm build/game