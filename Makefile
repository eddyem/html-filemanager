PROGRAM = df
LIBS = -lsqlite3 -lcrypt
OBJS = del_file.o CGI_auth.o
all : $(PROGRAM) clean
$(PROGRAM) : $(OBJS)
	gcc $(LIBS) $(OBJS) -o $(PROGRAM)
	gcc mkdir.c -o mkdir
	gcc getfile.c -o gf
	gcc filemanager.c -o fm
clean:
	/bin/rm -f *.o *~
depend:
	$(CXX) -MM $(CXX.SRCS)

### <DEPENDENCIES ON .h FILES GO HERE> 
# name1.o : header1.h header2.h ...
