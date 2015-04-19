all: modbus-test

modbus-test: modbus.o main.o
	gcc -o modbus-test modbus.o main.o -lapr-0

modbus.o: modbus.h modbus.c
	gcc -g -I/usr/include/apr-0 -c -o modbus.o modbus.c

main.o: main.c modbus.h
	gcc -g -I/usr/include/apr-0 -c -o main.o main.c

