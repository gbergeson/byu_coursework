CC=gcc
DEPS = aesMethods.h
OBJ = main.o aesMethods.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

aes: $(OBJ)
	$(CC) -o $@ $^
