INCLUDEDIRS = \

LIBDIRS = \

LIBS = \
	-pthread \
	-lmysqlclient \



CPPFLAGS = -Wall -O2 -std=c++11 -g 
LDFLAGS =  $(LIBDIRS) $(LIBS)

bin = judge

t1 = main
t2 = submit_checker
t3 = database
t4 = query

obj = $(t1).o $(t2).o $(t3).o $(t4).o

all: $(bin)
$(bin): $(obj)
		$(CXX) $(CPPFLAGS) $(obj) $(LDFLAGS) -o $@

clean:
		rm -f $(bin) *.o
