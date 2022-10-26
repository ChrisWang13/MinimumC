T = token
P = parse
C = codegen

CFLAGS = -g -Wall -Werror -fno-common 

OBJS = \
	$T/token.o \
	main.o 

MinimumC: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f MinimumC *.o 
