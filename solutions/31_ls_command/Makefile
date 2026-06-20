CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_31a test_31b test_31c clean

test_31a: 31a_file_size
	@printf 'hello' > _test_31a.txt
	@printf '' > _test_31a_empty.txt
	@echo "--- 31a: size of 5-byte file ---"
	./31a_file_size _test_31a.txt
	./31a_file_size _test_31a.txt | grep -q "size: 5"
	@echo "--- 31a: size of empty file ---"
	./31a_file_size _test_31a_empty.txt
	./31a_file_size _test_31a_empty.txt | grep -q "size: 0"
	@echo "--- 31a: nonexistent file (expect error) ---"
	! ./31a_file_size __nonexistent_file__ 2>/dev/null
	@rm -f _test_31a.txt _test_31a_empty.txt
	@echo "PASS: 31a_file_size"

31a_file_size: 31a_file_size.c
	$(CC) $(CFLAGS) 31a_file_size.c -o 31a_file_size

test_31b: 31b_file_time
	@echo "--- 31b: file modification time ---"
	./31b_file_time Makefile
	./31b_file_time Makefile | grep -qE "^mtime: [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$$"
	@echo "PASS: 31b_file_time"

31b_file_time: 31b_file_time.c
	$(CC) $(CFLAGS) 31b_file_time.c -o 31b_file_time

test_31c: 31c_ls_l
	@echo "--- 31c: ls -l default ---"
	./31c_ls_l Makefile
	./31c_ls_l Makefile | grep -q "Access mode:"
	./31c_ls_l Makefile | grep -q "file uid ="
	./31c_ls_l Makefile | grep -q "file size ="
	@echo "--- 31c: ls -l -i (inode) ---"
	./31c_ls_l Makefile -i
	./31c_ls_l Makefile -i | grep -qE "file st_ino ="
	@echo "--- 31c: ls -l -s (blocks) ---"
	./31c_ls_l Makefile -s
	./31c_ls_l Makefile -s | grep -qE "file st_blocks ="
	@echo "--- 31c: ls -l -T (full time) ---"
	./31c_ls_l Makefile -T
	./31c_ls_l Makefile -T | grep -qE "The time is"
	@echo "PASS: 31c_ls_l"

31c_ls_l: 31c_ls_l.c
	$(CC) $(CFLAGS) 31c_ls_l.c -o 31c_ls_l

clean:
	-rm -f 31a_file_size 31b_file_time 31c_ls_l _test_31a.txt _test_31a_empty.txt
