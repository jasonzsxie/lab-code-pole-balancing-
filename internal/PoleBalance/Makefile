CC = /usr/bin/g++
PROG = PoleBalance

SRCS = CartPole.cpp CmdArg.cpp Curvature.cpp GenAlg.cpp main.cpp NeuGen.cpp NeuralNet.cpp Queue.cpp 

LIBS = -L/usr/local/lib

INCLUDE = -I/usr/local/include

all: 
	$(CC) -g $(INCLUDE) $(SRCS) $(LIBS) -o $(PROG)

clean:
	rm *.o
	rm $(PROG)

