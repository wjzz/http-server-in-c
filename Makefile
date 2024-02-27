COMPILER=clang
FLAGS=-Wall -Werror -pedantic -std=c2x

compile:
	@$(COMPILER) $(FLAGS) src/main.c src/server.c -o bin/server

run: compile
	@./bin/server
