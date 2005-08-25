LIBS = -lglut -lGL -lGLU -lpthread -lreadline \
       -lSDL -lncurses
CYGWIN_LIBS = -L/lib/w32api -L/usr/local/lib -lglut32 -lopengl32 -lreadline \
	-lSDL -lncurses -lglu32
PROG = summon
OBJS = drawCommands.o \
       DrawController.o \
       DrawModel.o \
       DrawView.o \
       Graphic.o \
       GroupTable.o \
       Transform.o \
       Summon.o 

ifdef PROFILE
    LIBS := $(LIBS) -pg
endif


#
# Conditional linking
#
#ifeq ($(VISTOOLS_SCRIPT),$(VISTOOLS_SCHEME))
#	LIBS := $(LIBS) `guile-config link`
#        CYGWIN_LIBS := $(CYGWIN_LIBS) `guile-config link`
#endif
#ifeq ($(VISTOOLS_SCRIPT),$(VISTOOLS_PYTHON))
	LIBS := $(LIBS) -lpython2.4  -lutil
        CYGWIN_LIBS := $(CYGWIN_LIBS) -L/lib/python2.4/config -lpython2.4
#endif


all: $(PROG)

include ../Makefile.in


$(PROG): $(OBJS) $(MY_LIBS)
	$(CC) -o $(PROG) $(OBJS) $(MY_LIBS) $(LIBS)

$(PROG).exe: $(OBJS) $(MY_LIBS)
	$(CC) -o $(PROG).exe $(OBJS) $(MY_LIBS) $(CYGWIN_LIBS) 

install: $(PROG)
	cp $(PROG) ../../bin
