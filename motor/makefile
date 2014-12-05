CC=arm-linux-gcc
motor: motor.o
	$(CC) motor.o -s -o motor
motor.o: motor.c
	$(CC) -c motor.c -o motor.o
clean:
	rm -rf *.o motor
