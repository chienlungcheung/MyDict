MyDict : MyDict.o
	gcc -o MyDict MyDict.o

MyDict.o : MyDict.c 
	gcc -c MyDict.c 

clean:
	rm  MyDict.o

