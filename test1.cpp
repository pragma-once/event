#include "event.h"

#include <iostream>

class ClassWithEvents
{
    public:
    Utilities::Event<> a_call_event;
    Utilities::Event<float, int, bool> b_call_event;
    void a()
    {
        a_call_event.call();
    }
    void b(float x, int y, bool z)
    {
        b_call_event.call(x, y, z);
    }
};

int main()
{
    {
        std::shared_ptr<Utilities::EventHandler<>> a0; // OK
        //Utilities::Event<>::Handler a0; // OK
        {
            ClassWithEvents instance;
            {
                a0 = instance.a_call_event.handle([]() { std::cout << "A0\n"; });
                auto a1 = instance.a_call_event.handle([]() { std::cout << "A1\n"; });
                auto b1 = instance.b_call_event.handle([](float x, int y, bool z) {
                    std::cout << "B1: " << x << ", " << y << ", " << z << " \n";
                });

                instance.a();
                instance.b(0.5, 0, false);

                {
                    auto a2 = instance.a_call_event.handle([]() { std::cout << "A2\n"; });
                    auto b2 = instance.b_call_event.handle([](float x, int y, bool z) {
                        std::cout << "B2: " << x << ", " << y << ", " << z << " \n";
                    });

                    instance.a();
                    instance.b(1.1, 1, true);
                }

                instance.a();
                instance.b(-2.25, -2, false);
            }
            instance.b(1000, 100, true);
            instance.a();
        }
    }
    std::cout << "Done.\n";
    return 0;
}
