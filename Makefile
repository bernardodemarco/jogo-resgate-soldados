build:
	gcc game.c -Wall -pthread -lSDL2 -lSDL2main -o game
run:
	./game
easy:
	./game 0
medium:
	./game 1
hard:
	./game 2
clean:
	rm game