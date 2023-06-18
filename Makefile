target: gen read
gen: gen.c
	gcc gen.c -o gen -lrt -pthread
read: read.c
	gcc read.c -o read -lrt -pthread