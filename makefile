# I'm no make expert, so this will have to do. :p

build:
	mkdir -p bin
	gcc -std=gnu99 src/* -I include -o ./bin/scheduler 
