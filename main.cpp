#include "Robot.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Specify robot's name\n");
        return 1;
    }
    Robot r;
    r.setName(argv[1]);
    r.connectToGame(4774);
    r.run();
    // r.updatePlayersStates();
    // r.updatePlayersStates();
    // r.updatePlayersStates();
    // r.updatePlayersStates();

    return 0;
}
