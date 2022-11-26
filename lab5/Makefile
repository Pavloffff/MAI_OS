compile:
	gcc -c -Wall -Werror -fpic lib1.c
	gcc -c -Wall -Werror -fpic lib2.c
	gcc -shared -o liblib1.so lib1.o -lm
	gcc -shared -o liblib2.so lib2.o -lm
	gcc -L. -Wall -o main1.out main1.c -llib1
	gcc -L. -Wall -o main2.out main1.c -llib2
	gcc -L. -Wall -o main.out main2.c -llib2 -llib1

clean:
	rm *.o
	rm *.so
	rm *.out