#include <string.h>

#include "Robot.hpp"

Robot* setupRobotFromArgv(int argc, char* argv[])
{
    if (argc != 5)
    {
        return new Robot();
    }
    char* ip = argv[1];
    int port;
    sscanf(argv[2], "%d", &port);
    char* name = argv[3];
    int wait_to_n;
    sscanf(argv[4], "%d", &wait_to_n);
    return new Robot(ip, port, name, wait_to_n);
}

int main(int argc, char* argv[])
{
    Robot* r;
    r = setupRobotFromArgv(argc, argv);
    r->connectToGame();
    r->run();

    return 0;
}
