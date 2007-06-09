SRCPREFIX = ..
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

all: $(PROG)

include ../Makefile.in


$(PROG): $(OBJS) $(MY_LIBS)
	$(CC) -shared $(OBJS) $(MY_LIBS) $(LIBS) -o $(PROG)
	cp $(PROG) ../../lib
