# you can put include files in this directory

CC=gcc
CFLAGS=-I .



# create string with <header filename> in our case myHeaderfile.h
DEPS = header.h


# create string with obj/<object filename> in our case fun.o 
LIBOBJ = registration.o login.o addCourse.o enroll.o coursesRead.o
LIBSRC = registration.c login.c addCourse.c enroll.c coursesRead.c
APPOBJ = client.o
APPOBJ1 = server.o
#Question1

registration.o: registration.c
	$(CC) -c -o $@ $< $(CFLAGS)
login.o: login.c
	$(CC) -c -o $@ $< $(CFLAGS)
addCourse.o: addCourse.c
	$(CC) -c -o $@ $< $(CFLAGS)
enroll.o: enroll.c
	$(CC) -c -o $@ $< $(CFLAGS)
coursesRead.o: coursesRead.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(APPOBJ1): server.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
$(APPOBJ): client.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)	
	
server.out: $(APPOBJ1) $(LIBOBJ)
	$(CC) -o $@ $^ $(CFLAGS)
client.out: $(APPOBJ) $(LIBOBJ)
	$(CC) -o $@ $^ $(CFLAGS)




.PHONY: all
all: client.out server.out
