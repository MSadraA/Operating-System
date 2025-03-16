CXX = g++
CXXFLAGS = -std=c++11 -Wall -Ishared -Isockets

SRC_SERVER = $(wildcard server/*.cpp shared/*.cpp sockets/*.cpp)
SRC_CLIENT = $(wildcard client/*.cpp shared/*.cpp sockets/*.cpp)

OBJ_SERVER = $(SRC_SERVER:.cpp=.o)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)

BIN_SERVER = server.out
BIN_CLIENT = client.out

all: $(BIN_SERVER) $(BIN_CLIENT)

$(BIN_SERVER): $(OBJ_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN_CLIENT): $(OBJ_CLIENT)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_SERVER) $(OBJ_CLIENT) $(BIN_SERVER) $(BIN_CLIENT)

run:
	tmux new-session -d -s my_session
	tmux rename-window -t my_session "Terminal 1"
	tmux split-window -h -t my_session
	tmux rename-window -t my_session "Terminal 2"
	tmux split-window -v -t my_session:0.1
	tmux rename-window -t my_session "Terminal 3"
	tmux split-window -v -t my_session:0.1
	tmux attach-session -t my_session
