set -xe
# cc main.c -o fold
cc -nostdlib -fno-pie -Os test.c -o test.o -c
tcc -run main.c test.o
