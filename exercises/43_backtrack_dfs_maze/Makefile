CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 43a_dfs_visit 43b_dfs_path 43c_dfs_all_paths

43a_dfs_visit: 43a_dfs_visit.c
	$(CC) $(CFLAGS) $< -o $@

43b_dfs_path: 43b_dfs_path.c
	$(CC) $(CFLAGS) $< -o $@

43c_dfs_all_paths: 43c_dfs_all_paths.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 43a_dfs_visit 43b_dfs_path 43c_dfs_all_paths
