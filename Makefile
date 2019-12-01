all:
	export LD_LIBRARY_PATH=.
	
	clang -std=c99 -Wall -pedantic *.c -o pavt -L. -lruntime

	./pavt -p 2 10 20
	
clean:
	rm -f pavt

#export LD_LIBRARY_PATH="$(PWD)";
