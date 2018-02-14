build:
	mkdir -p bin
	gcc -std=gnu99 src/* -I include -o ./bin/scheduler

debug:
	mkdir -p bin
	gcc -std=gnu99 src/* -I include -o ./bin/scheduler -g
