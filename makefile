all: compile run

compile: stack.c 
	gcc stack.c -o assignment1

run: assignment1
	./assignment1 input.txt output.txt

clean: assignment1 
	rm assignment1 output.txt
