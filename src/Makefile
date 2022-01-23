# Set to use Gnu debugger
GDB_FLAG = -g -O3

CFG_INC = -I../include -I/usr/include -I/usr/local/include
LIB_PATH = -L/usr/local/lib -L/usr/lib64

# Initialize other variables
OUTDIR  = ../Release
OBJDIR  = $(OUTDIR)
OUTFILE = $(OUTDIR)/ged188

DEBUG_FLAG = -Wall -DLINUX -DMY_TCP_DEBUG -DVAS_ENABLED -DCTI_ENABLED -DPFS_ENABLED	# 	# -DDEBUG

LIB = -lpthread

OBJ =	$(OBJDIR)/log.o           \
		$(OBJDIR)/util.o          \
		$(OBJDIR)/cfg.o           \
		$(OBJDIR)/pktdef.o        \
		$(OBJDIR)/posixsem.o      \
		$(OBJDIR)/mysocket.o      \
		$(OBJDIR)/mythread.o      \
		$(OBJDIR)/ctisocketif.o   \
		$(OBJDIR)/ctiqueue.o      \
		$(OBJDIR)/ctiifhandler.o  \
		$(OBJDIR)/vasifhandler.o  \
		$(OBJDIR)/vasextra.o      \
		$(OBJDIR)/pfsifhandler.o  \
		$(OBJDIR)/messagetype.o   \
		$(OBJDIR)/agentstate.o    \
		$(OBJDIR)/statuscode.o    \
		$(OBJDIR)/pgstatuscode.o  \
		$(OBJDIR)/tagvalue.o      \
		$(OBJDIR)/tcpserver.o     \
		$(OBJDIR)/CKeepAlivedServer.o     \
		$(OBJDIR)/CKeepAlivedClient.o     \
		$(OBJDIR)/main.o

COMPILE = g++ -w -c $(GDB_FLAG) $(DEBUG_FLAG) -o "$(OUTDIR)/$(*F).o" $(CFG_INC) \
			"$<"
LINK = g++ $(GDB_FLAG) -o "$(OUTFILE)" $(OBJ) $(CFG_LIB) $(LIB_PATH) $(LIB)

# Pattern rules

$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTFILE): $(OUTDIR) $(OBJ) $(INC)
	$(LINK)

# Create the release dir if doesn't exist
$(OUTDIR):
	mkdir -p "$(OUTDIR)"

# Build rules
all:
	$(MAKE) $(OUTFILE)

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	rm -f $(OUTFILE)
	rm -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
