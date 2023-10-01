build:
	gcc game.c -Wall -pthread -lSDL2 -lSDL2main -o game
run:
	./game
clean:
	rm game