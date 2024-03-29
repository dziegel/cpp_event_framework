#include <cstdlib>
#include <exception>
#include <iostream>

extern void EventsFixtureMain();
extern void StatemachineFixtureMain();
extern void InterfaceStatemachineExampleMain();
extern void PimplStatemachineExampleMain();
extern void SimpleStatemachineExampleMain();
extern void SignalsStatemachineExampleMain();
extern void ActiveObjectFrameworkMain();
extern void ActiveObjectFrameworkEmbeddedMain();

int main(int, const char**)
{
    try
    {
        StatemachineFixtureMain();
        EventsFixtureMain();
        InterfaceStatemachineExampleMain();
        PimplStatemachineExampleMain();
        SimpleStatemachineExampleMain();
        SignalsStatemachineExampleMain();
        ActiveObjectFrameworkMain();
        ActiveObjectFrameworkEmbeddedMain();
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
