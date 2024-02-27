COMPILER=clang++
FLAGS=-Wall -Werror -pedantic -std=c++20

compile:
	@$(COMPILER) $(FLAGS) src/main.cpp src/server.cpp -o bin/server

run: compile
	@./bin/server
