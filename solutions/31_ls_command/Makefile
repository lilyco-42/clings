CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_31a test_31b test_31c clean

test_31a: 31a_file_size
	@printf 'hello' > _test_31a.txt
	./31a_file_size _test_31a.txt | grep -q "size: 5"
	@rm -f _test_31a.txt
	@echo "PASS: 31a_file_size"

31a_file_size: 31a_file_size.c
	$(CC) $(CFLAGS) 31a_file_size.c -o 31a_file_size

test_31b: 31b_file_time
	./31b_file_time Makefile | grep -qE "^mtime: [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$$"
	@echo "PASS: 31b_file_time"

31b_file_time: 31b_file_time.c
	$(CC) $(CFLAGS) 31b_file_time.c -o 31b_file_time

test_31c: 31c_ls_l
	./31c_ls_l Makefile | grep -q "Access mode:"
	./31c_ls_l Makefile | grep -q "file uid ="
	./31c_ls_l Makefile | grep -q "file size ="
	./31c_ls_l Makefile -i | grep -qE "file st_ino ="
	./31c_ls_l Makefile -s | grep -qE "file st_blocks ="
	./31c_ls_l Makefile -T | grep -qE "The time is"
	@echo "PASS: 31c_ls_l"

31c_ls_l: 31c_ls_l.c
	$(CC) $(CFLAGS) 31c_ls_l.c -o 31c_ls_l

clean:
	-rm -f 31a_file_size 31b_file_time 31c_ls_l _test_31a.txt
