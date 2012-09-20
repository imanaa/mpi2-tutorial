# This is the compiler to use
CC=g++
# The output directory
OUTPUT_DIR=output
# The Compiler Flags for Windows
FLAGS=-I "C:\Program Files (x86)\MPICH2\include" -L "C:\Program Files (x86)\MPICH2\lib" -lmpi
# The Compiler Flags for Linux
#FLAGS=-I "/usr/include/mpich2/" -L "/usr/lib" -lmpich

all: example01.exe example02.exe example03.exe example04.exe example05.exe example06.exe example07.exe example08.exe example09.exe example10.exe example11.exe example12.exe example13.exe example14.exe example15.exe example16.exe 

example01.exe: example01.c
	$(CC) example01.c -o $(OUTPUT_DIR)/example01.exe $(FLAGS)
	
example02.exe: example02.c
	$(CC) example02.c -o $(OUTPUT_DIR)/example02.exe $(FLAGS)
	
example03.exe: example03.c
	$(CC) example03.c -o $(OUTPUT_DIR)/example03.exe $(FLAGS)
	
example04.exe: example04.c
	$(CC) example04.c -o $(OUTPUT_DIR)/example04.exe $(FLAGS)
	
example05.exe: example05.c
	$(CC) example05.c -o $(OUTPUT_DIR)/example05.exe $(FLAGS)
	
example06.exe: example06.c
	$(CC) example06.c -o $(OUTPUT_DIR)/example06.exe $(FLAGS)
	
example07.exe: example07.c
	$(CC) example07.c -o $(OUTPUT_DIR)/example07.exe $(FLAGS)
	
example08.exe: example08.c
	$(CC) example08.c -o $(OUTPUT_DIR)/example08.exe $(FLAGS)
	
example09.exe: example09.c
	$(CC) example09.c -o $(OUTPUT_DIR)/example09.exe $(FLAGS)
	
example10.exe: example10.c
	$(CC) example10.c -o $(OUTPUT_DIR)/example10.exe $(FLAGS)
	
example11.exe: example11.c
	$(CC) example11.c -o $(OUTPUT_DIR)/example11.exe $(FLAGS)
	
example12.exe: example12.c
	$(CC) example12.c -o $(OUTPUT_DIR)/example12.exe $(FLAGS)
	
example13.exe: example13.c
	$(CC) example13.c -o $(OUTPUT_DIR)/example13.exe $(FLAGS)
	
example14.exe: example14.c
	$(CC) example14.c -o $(OUTPUT_DIR)/example14.exe $(FLAGS)
	
example15.exe: example15.c
	$(CC) example15.c -o $(OUTPUT_DIR)/example15.exe $(FLAGS)

example16.exe: example16.c
	$(CC) example16.c -o $(OUTPUT_DIR)/example16.exe $(FLAGS)

clean:
	rm -rf $(OUTPUT_DIR)/*.o $(OUTPUT_DIR)/*.exe