all:
	brew install gcc
	gcc -o host Keyboard.h Keyboard.c main.c

run:
	$(MAKE) all
	sudo  ./host
