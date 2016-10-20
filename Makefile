all: test loopf lab3-pipefull lab3-pipeerr lab3-pipesig lab3-shell

lab3-pipefull: lab3-pipefull.c
	gcc $^ -o $@ -Wall

lab3-pipeerr: lab3-pipeerr.c
	gcc $^ -o $@ -Wall

lab3-pipesig: lab3-pipesig.c
	gcc $^ -o $@ -Wall

lab3-shell: lab3-shell.c
	gcc $^ -o $@ -Wall

test: test.c
	gcc $^ -o $@ -Wall

loopf: loopever.c
	gcc $< -D FOREVER=0 -o $@

clean:
	rm -f test loopf lab3-pipefull lab3-pipeerr lab3-pipesig lab3-shell
