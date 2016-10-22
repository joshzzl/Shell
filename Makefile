all: myshell loopf forever
myshell: myshell.c helper.c
	gcc $^ -o $@ -Wall -ggdb

loopf: loopever.c
	gcc $< -D FOREVER=0 -o $@

forever: loopever.c
	gcc $< -D FOREVER=1 -o $@

clean:
	rm -f myshell loopf forever
