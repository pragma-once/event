# Event

A simple and elegant header-only event class for C++.

# Usage

The test files contain some example usages.

Include:

```
#include "path/to/event.h"
```

Include with custom namespace:

```
#define PRAGMAONCE_EVENT_NAMESPACE_OVERRIDE my::custom::namespace
#include "path/to/event.h"
```

Include without any namespace:

```
#define PRAGMAONCE_EVENT_NAMESPACE_REMOVE
#include "path/to/event.h"
```

Create an event with no parameters:

```
Event my_event_with_no_params;
```

Create an event with an int and a float parameter:

```
Event<int, float> my_event;
```

Handle `my_event`:

```
Event<int, float>::Handler my_handler; // Keep the object to listen.
// Alternatively:
//std::shared_ptr<EventHandler<int, float>> my_handler;
...
my_handler = my_event.handle([](int a, float b) { /* do something */ });
...
my_handler = nullptr; // Stop listening manually by destructing the handler,
                      // or it could be destructed as a member of a class automatically.
```

Fire event:

```
my_event.call(1, 0.5);
```
