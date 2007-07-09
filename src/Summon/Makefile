SRCPREFIX = ..
OBJS = summonCommands.o \
       SummonController.o \
       SummonModel.o \
       SummonView.o \
       Element.o \
       Graphic.o \
       GroupTable.o \
       Transform.o \
       TransformMatrix.o \
       Summon.o 

all:: $(MODULE)

include ../Makefile.in


$(MODULE): $(OBJS) $(MY_LIBS)
	$(CC) -shared $(OBJS) $(MY_LIBS) $(LIBS) -o $(MODULE)

