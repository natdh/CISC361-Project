CC = g++
CFLAGS = -std=c++98 -g
OBJS = system.o job.o process.o
TESTS = system_test intake_test

default: intake.o

test: $(TESTS)

system_test: system_test.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	./$@

intake_test: input_reader.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	./$@ test.txt

%.o: %.cc %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o D*.json
