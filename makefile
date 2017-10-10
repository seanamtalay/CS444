#build pdf from latex
all: CS444P1.tex clean
	latex --output-format=pdf CS444P1.tex
	
	
clean:
	$(RM) CS444P1.pdf CS444P1.log
