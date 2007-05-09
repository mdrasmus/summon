PROG = summon_core.so
OBJS = drawCommands.o \
       DrawController.o \
       DrawModel.o \
       DrawView.o \
       Element.o \
       Graphic.o \
       GroupTable.o \
       Transform.o \
       TransformMatrix.o \
       Summon.o 


ifdef PROFILE
    LIBS := $(LIBS) -pg
endif


LIBS = -lglut -lGL -lSDL -lutil -lpython2.4
CYGWIN_LIBS = -L/lib/w32api -L/usr/local/lib -lglut32 -lopengl32 \
	-lSDL -L/lib/python2.4/config -lpython2.4



all: $(PROG)

include ../Makefile.in


$(PROG): $(OBJS) $(MY_LIBS)
	$(CC) -pthread -shared $(OBJS) $(MY_LIBS) $(LIBS) -o $(PROG)

#$(PROG).exe: $(OBJS) $(MY_LIBS)
#	$(CC) -o $(PROG).exe $(OBJS) $(MY_LIBS) $(CYGWIN_LIBS) 

install: $(PROG)
	cp $(PROG) ../../lib/


test2:
	echo $(PLATFORM)
