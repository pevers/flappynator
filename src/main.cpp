#include <iostream>
#include "engine.h"
using namespace std;

int main()
{
    Engine engine;
    if (!engine.init()) {
        cerr << "could not initialize game" << endl;
        return -1;
    }

    engine.mainLoop();
}
