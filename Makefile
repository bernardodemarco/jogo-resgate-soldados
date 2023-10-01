build:
	gcc -Wall -pthread game.c -o game
run:
	./game
clean:
	rm game