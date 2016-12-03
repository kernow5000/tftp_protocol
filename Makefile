# CNET308 File transfer protocol
# 
# Shaun Bradley
# Bsc Multimedia Computing yr 3

CC = gcc


HARNESS_SRC 	= testharness.c
PROTO_SRC	= protocol.c

HARNESS_BIN     = testharness
PROTO_BIN	= protocol.o


ARCHIVE = cnet308-FINAL


all: harness

harness:      
	      $(CC) $(PROTO_SRC) $(HARNESS_SRC) -o $(HARNESS_BIN)
	      
   
clean:	      
	      rm -Rf *.o *~ *.copy $(HARNESS_BIN) $(PROTO_BIN)
	      

backup:
	      tar cf $(ARCHIVE).tar *
	      gzip $(ARCHIVE).tar
	      mv $(ARCHIVE).tar.gz $(HOME)
	      cp $(HOME)/$(ARCHIVE).tar.gz /mnt/storage
	      
	      
	     
      	       
