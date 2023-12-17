CC?=gcc
CFLAGS?=-Wextra -Wall -pedantic -std=c99 -g
LDFLAGS?=-lm

all: sort-example knn-bruteforce knn-svg knn-kdtree knn-genpoints

sort-example: sort-example.o sort.o
	$(CC) -o $@ $^ $(LDFLAGS)

knn-bruteforce: knn-bruteforce.o bruteforce.o io.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

knn-kdtree: knn-kdtree.o bruteforce.o io.o util.o kdtree.o sort.o
	$(CC) -o $@ $^ $(LDFLAGS)

knn-genpoints: knn-genpoints.o io.o
	$(CC) -o $@ $^ $(LDFLAGS)

knn-svg: knn-svg.o io.o util.o kdtree.o sort.o
	$(CC) -o $@ $^ $(LDFLAGS)

# Add a new build rule for correct
correct: correct.o io.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

# A general rule that tells us how to generate an .o file from a .c
# file.  This cuts down on the boilerplate.
%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -rf knn-genpoints knn-bruteforce knn-svg knn-kdtree *.o *.dSYM
	rm -rf points queries indexes points.svg

# Testing rules

# Medium
NUM_POINTS_MEDIUM=1000
NUM_QUERIES_MEDIUM=500
DIM_MEDIUM=3
K_MEDIUM=5

# Large
NUM_POINTS_LARGE=10000
NUM_QUERIES_LARGE=1000
DIM_LARGE=4
K_LARGE=10


# Define a rule for each test set

test_medium: 
	@echo "TESTING STARTING! WARNING MAY TAKE A WHILE\n"
	@echo "Bruteforce testing\n"
	./knn-genpoints $(NUM_POINTS_MEDIUM) $(DIM_MEDIUM) > points
	./knn-genpoints $(NUM_QUERIES_MEDIUM) $(DIM_MEDIUM) > queries
	./knn-bruteforce points queries $(K_MEDIUM) indexes
	./correct points queries indexes

test_large: 
	./knn-genpoints $(NUM_POINTS_LARGE) $(DIM_LARGE) > points
	./knn-genpoints $(NUM_QUERIES_LARGE) $(DIM_LARGE) > queries
	./knn-bruteforce points queries $(K_LARGE) indexes
	./correct points queries indexes

# Update the 'test' rule to run all tests
test: test_medium test_large
	@echo "\nComparing bruteforce with kdtree test\n"
	./test_knn.sh
	@echo "\nLook in the directory for the svg files"