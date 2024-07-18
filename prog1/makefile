CC = g++
CFLAGS = -std=c++11 -pg
TARGET = kosaraju_stack_adj_mat



.PHONY: all clean run profile

all: $(TARGET)

$(TARGET) : kosaraju_stack_adj_mat.o


%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<



profile: run
	gprof  kosaraju_stack_adj_mat gmon.out > analysis.txt

clean:
	rm -f $(TARGET) *.o gmon.out analysis.txt