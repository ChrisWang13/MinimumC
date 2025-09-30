T = token
P = parse
C = codegen
V = visualize

CFLAGS = -g -fno-common

OBJS = \
	$T/token.o \
	$P/type.o \
	$P/parse.o \
	$C/codegen.o \
	$V/visualize.o \
	main.o 
	
MinimumC: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f MinimumC *.s res && find . -type f -name '*.o' -delete

test:
	make && sudo ./test.sh