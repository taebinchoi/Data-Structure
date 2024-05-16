all: compile run

compile: rb.c 
	gcc rb.c -o assignment2_20233719

run: assignment2_20233719
	./assignment2_20233719 input.txt output.txt

clean: assignment2_20233719
	rm assignment2_20233719 output.txt
