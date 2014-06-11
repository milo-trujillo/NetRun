PROGNAME = netrun
CXX= g++
# -std=c++0x is needed to enable C++ 11 features
# -stdlib=libc++ forces clang to use real libraries instead of hijacking gcc
#CFLAGS += -stdlib=libc++ -std=c++0x
# I suppose the opposite of this would be enabling optimization
CFLAGS += -g
LIBS += -lncurses -lm

OBJS = bsp.o io.o main.o map.o monster.o player.o rand.o save.o message.o status.o entity.o util.o

all: $(OBJS)
	$(CXX) $(CFLAGS) $(LIBS) -o $(PROGNAME) $(OBJS)

$(OBJS): %.o: %.C
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(PROGNAME)
