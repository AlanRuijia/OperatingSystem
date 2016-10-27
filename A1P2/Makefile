CC=gcc 
CFLAGS=
DEPS = job.h mysignal.h
OBJ = my_shell.o job.o mysignal.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

my_shell: $(OBJ)
	gcc -o $@ $^

clean:
	rm my_shell my_shell.o
run: my_shell
	./my_shell
