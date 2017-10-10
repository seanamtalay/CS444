#build pdf from latex
CC:=gcc
CFLAGS:= -Wall -pthread
all: CS444P1.tex clean
	latex --output-format=pdf CS444P1.tex
	$(CC) concurrency1.c $(CFLAGS) -o concurrency1
	
	
clean:
	$(RM) CS444P1.aux CS444P1.pdf CS444P1.log concurrency1
