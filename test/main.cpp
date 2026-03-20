#include "Engine/Engine.h"
#include "TestState.h"

int main(int argc, char* argv[])
{
    Engine* eng = new Engine({.title = "Test", .appID = "com.tyshi.test"});
    eng->addState(TestState::instance());
    eng->run();

    return 0;
}