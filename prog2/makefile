CC = g++
CFLAGS = -std=c++11 -pg
TARGET = kosaraju_deque_adj_mat kosaraju_deque_linked_list kosaraju_stack_linked_list



.PHONY: all clean run profile



kosaraju_deque_adj_mat: kosaraju_deque_adj_mat.o

kosaraju_deque_linked_list:kosaraju_deque_linked_list.o

kosaraju_stack_linked_list: kosaraju_stack_linked_list.o


%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<



profile1: run
	gprof kosaraju_deque_adj_mat gmon.out > analysis1.txt

profile2: run
	gprof kosaraju_deque_linked_list gmon.out > analysis2.txt

profile3: run
	gprof kosaraju_stack_linked_list gmon.out > analysis3.txt		

clean:
	rm -f $(TARGET) $(OBJ) *.o gmon.out analysis.txt