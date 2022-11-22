T = token
P = parse
C = codegen

CFLAGS = -g -fno-common 

OBJS = \
	$T/token.o \
	$P/type.o \
	$P/parse.o \
	$C/codegen.o \
	main.o 
	
MinimumC: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f MinimumC *.s res && find . -type f -name '*.o' -delete

test:
	sudo ./test.sh