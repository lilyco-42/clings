CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 48a_city_graph 48b_shortest_path 48c_print_route

48a_city_graph: 48a_city_graph.c
	$(CC) $(CFLAGS) $< -o $@

48b_shortest_path: 48b_shortest_path.c
	$(CC) $(CFLAGS) $< -o $@

48c_print_route: 48c_print_route.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 48a_city_graph 48b_shortest_path 48c_print_route 48d_gps_ncurses
