SRCPREFIX = ..
OBJS = Element.o \
       Graphic.o \
       Hotspot.o \
       TextElement.o \
       Transform.o \
       TransformMatrix.o \
       Summon.o \
       summonCommands.o \
       SummonController.o \
       SummonModel.o \
       SummonView.o \
       ZoomClamp.o

all:: $(MODULE)

include ../Makefile.in


$(MODULE): $(OBJS) $(MY_LIBS)
	$(CC) -shared $(OBJS) $(MY_LIBS) $(LIBS) -o $(MODULE)

