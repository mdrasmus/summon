SRCPREFIX = ../..
OBJS = ScriptCommand.o \
       Summon.o \
       summonCommands.o \
       SummonController.o \
       SummonModel.o \
       SummonView.o

all:: $(MODULE)

include ../../Makefile.in

# extra targets
$(MODULE): $(OBJS) $(SUMMON_LIBS)
	$(CC) -shared $(OBJS) $(SUMMON_LIBS) $(LDFLAGS) -o $(MODULE)

