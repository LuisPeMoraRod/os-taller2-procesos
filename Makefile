all: search_word

search_word: search.c
	gcc -o search search.c

run: search
	./search

clean:
	rm -f search
	
