CXX = g++
CXXFLAGS = -g -Wall -Werror -std=c++03 -pedantic
SRCMODULS = Robot.cpp TradingLogList.cpp PlayersStatesList.cpp
OBJMODULS = $(SRCMODULS:.c=.o)
EXECUTABLE = robot.out

%.o: %.c %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): main.cpp $(OBJMODULS)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(EXECUTABLE) 
	./$(EXECUTABLE)

clean:
	rm -f *.o $(EXECUTABLE) 