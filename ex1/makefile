# ~~~
# The -C option in the make command tells make to change to the directory specified before reading the makefiles.
# ~~~

.PHONY: all clean q1 q2 q3 q4 q5 q6

all: q1 q2 q3 q4 q5 q6

# run make in each subdirectory
q*:
	make -C $@ all
	
clean:
	make -C q1 clean
	make -C q2 clean
	make -C q3 clean
	make -C q4 clean
	make -C q5 clean
	make -C q6 clean