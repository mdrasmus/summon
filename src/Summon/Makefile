SRCPREFIX = ..
OBJS = Summon.o \
       summonCommands.o \
       SummonController.o \
       SummonModel.o \
       SummonView.o

all:: $(MODULE)

include ../Makefile.in


$(MODULE): $(OBJS) $(MY_LIBS)
	$(CC) -shared $(OBJS) $(MY_LIBS) $(LDFLAGS) -o $(MODULE)

