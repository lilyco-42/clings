CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 46a_init_graph 46b_dijkstra 46c_print_path

46a_init_graph: 46a_init_graph.c
	$(CC) $(CFLAGS) $< -o $@

46b_dijkstra: 46b_dijkstra.c
	$(CC) $(CFLAGS) $< -o $@

46c_print_path: 46c_print_path.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 46a_init_graph 46b_dijkstra 46c_print_path
