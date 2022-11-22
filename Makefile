T = token
P = parse
C = codegen

CFLAGS = -g  -fno-common 

OBJS = \
	$T/token.o \
	$P/type.o \
	$P/parse.o \
	main.o 
	
MinimumC: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f MinimumC *.o 
