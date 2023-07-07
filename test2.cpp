#define PRAGMAONCE_EVENT_NAMESPACE_REMOVE

#include "event.h"

#include <iostream>

int main()
{
    Event event;
    auto handler = event.handle([]() { std::cout << "Test.\n"; });
    event.call();
    return 0;
}
