CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean test_48d

all: 48a_city_graph 48b_shortest_path 48c_print_route

48a_city_graph: 48a_city_graph.c
	$(CC) $(CFLAGS) $< -o $@

48b_shortest_path: 48b_shortest_path.c
	$(CC) $(CFLAGS) $< -o $@

48c_print_route: 48c_print_route.c
	$(CC) $(CFLAGS) $< -o $@

test_48d: 48d_gps_ncurses.c
ifeq ($(shell uname -s),Linux)
	$(CC) $(CFLAGS) $< -o 48d_gps_ncurses -lncurses
else
	$(CC) $(CFLAGS) $< -o 48d_gps_ncurses
endif
	@echo "PASS: 48d_gps_ncurses compiled"

clean:
	-rm -f 48a_city_graph 48b_shortest_path 48c_print_route 48d_gps_ncurses
