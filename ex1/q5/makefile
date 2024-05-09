.PHONY: all run clean

CC = gcc -pg

all: main1 main2 main3

SEED = 5
N = 100

# we create (if not exists) all the programs and then run them with the seed and N
# in each run we create a gmon.out file
# we will store the output of gprof in a file called output$(N).txt
# and then we will rename the gmon.out file to gmon.out.main1, gmon.out.main2, gmon.out.main3
run: all
	./main1 $(SEED) $(N) >> /dev/null
	echo "~ maximum subarray O(n) ~" > output$(N).txt
	gprof --flat-profile main1 | head >> output$(N).txt
	mv gmon.out gmon_outputs/gmon.out.main1_$(N)

	./main2 $(SEED) $(N) >> /dev/null
	echo "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" >> output$(N).txt
	echo "~ maximum subarray O(n^2) ~\n" >> output$(N).txt
	gprof --flat-profile main2 | head >> output$(N).txt
	mv gmon.out gmon_outputs/gmon.out.main2_$(N)

	./main3 $(SEED) $(N) >> /dev/null
	echo "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" >> output$(N).txt
	echo "~ maximum subarray O(n^3) ~\n" >> output$(N).txt
	gprof --flat-profile main3 | head >> output$(N).txt
	mv gmon.out gmon_outputs/gmon.out.main3_$(N)


main1: main.c max_sub_array_n.o
	$(CC) -c -Wall $< -o main1.o
	$(CC) -o $@ main1.o max_sub_array_n.o

main2: main.c max_sub_array_n2.o
	$(CC) -c -Wall $< -o main2.o
	$(CC) -o $@ main2.o max_sub_array_n2.o

main3: main.c max_sub_array_n3.o
	$(CC) -c -Wall $< -o main3.o
	$(CC) -o $@ main3.o max_sub_array_n3.o


max_sub_array_n3.o: max_sub_array_n3.c max_sub_array.h
	$(CC) -c -Wall -o $@ $<

max_sub_array_n2.o: max_sub_array_n2.c max_sub_array.h
	$(CC) -c -Wall -o $@ $<

max_sub_array_n.o: max_sub_array_n.c max_sub_array.h
	$(CC) -c -Wall -o $@ $<
	
clean:
	rm -f *.o main[123] gmon.out*