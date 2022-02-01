#include <cstdlib>
#include <exception>
#include <iostream>

void EventsFixtureMain();
void StatemachineFixtureMain();
void SimpleStatemachineExampleMain();

int main(int, const char**)
{
    try
    {
        StatemachineFixtureMain();
        EventsFixtureMain();
        SimpleStatemachineExampleMain();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
