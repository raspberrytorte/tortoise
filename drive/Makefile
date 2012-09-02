#
# Makefile:
#	tortoise drive - Wiring Compatable library for the Raspberry Pi
#

#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LIBS    = -lwiringPi

# Should not alter anything below this line
###############################################################################

SRC	=	drive.c ledEnable.c softPwmTest.c softDrive.c readEncoders.c softDriveEncoder.c

OBJ	=	drive.o ledEnable.o softPwmTest.o softDrive.o readEncoders.o softDriveEncoder.o

all:		drive ledEnable softPwmTest softDrive readEncoders softDriveEncoder

drive:	drive.o
	@echo [link]
	$(CC) -o $@ drive.o $(LDFLAGS) $(LIBS)
	
ledEnable:	ledEnable.o
	@echo [link]
	$(CC) -o $@ ledEnable.o $(LDFLAGS) $(LIBS)

softPwmTest:	softPwmTest.o
	@echo [link]
	$(CC) -o $@ softPwmTest.o $(LDFLAGS) $(LIBS) -lpthread

softDrive:	softDrive.o
	@echo [link]
	$(CC) -o $@ softDrive.o $(LDFLAGS) $(LIBS) -lpthread
	
readEncoders:   readEncoders.o
	@echo [link]
	$(CC) -o $@ readEncoders.o $(LDFLAGS) $(LIBS) -lpthread

softDriveEncoder:   softDriveEncoder.o
	@echo [link]
	$(CC) -o $@ softDriveEncoder.o $(LDFLAGS) $(LIBS) -lpthread

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) *~ core tags drive ledEnable softPwmTest softDrive readEncoders softDriveEncoder

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)

# DO NOT DELETE
