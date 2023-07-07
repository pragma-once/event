/*
    MIT License

    Copyright (c) 2023 Majidzadeh (hashpragmaonce@gmail.com)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <set>

// NAMESPACE BEGIN
#ifdef PRAGMAONCE_EVENT_NAMESPACE_OVERRIDE
    namespace PRAGMAONCE_EVENT_NAMESPACE_OVERRIDE {
#else
    #ifndef PRAGMAONCE_EVENT_NAMESPACE_REMOVE // Default
        namespace Utilities {
    #endif
#endif

template <typename... ParamTypes>
class Event;

/// @brief Controls the lifetime of an event handler.
template <typename... ParamTypes>
class EventHandler
{
    friend Event<ParamTypes...>;
public:
    ~EventHandler();

    EventHandler() = delete;
    EventHandler(EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;
    EventHandler& operator=(EventHandler&) = delete;
    EventHandler& operator=(EventHandler&&) = delete;
private:
    EventHandler(Event<ParamTypes...> * event, std::function<void(ParamTypes...)>);

    std::shared_ptr<std::shared_mutex> mutex;

    Event<ParamTypes...> * event;
    std::function<void(ParamTypes...)> handler_function;
};

/// @brief A thread-safe event class providing observer pattern.
template <typename... ParamTypes>
class Event
{
    friend EventHandler<ParamTypes...>;
public:
    Event();
    ~Event();

    Event(Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(Event&) = delete;
    Event& operator=(Event&&) = delete;

    /// @brief Controls the lifetime of an event handler.
    typedef std::shared_ptr<EventHandler<ParamTypes...>> Handler;

    /// @brief Fires the event by calling all the existing handlers.
    void call(ParamTypes...);

    /// @brief Adds a handler. The returned handler object MUST be kept to keep listening.
    /// @return A shared pointer of an object to control the lifetime of the handler.
    Handler handle(std::function<void(ParamTypes...)> handler_function);
private:
    /// Shared between the Event and EventHandler objects.
    std::shared_ptr<std::shared_mutex> mutex;

    std::set<EventHandler<ParamTypes...>*> handlers;
};

template <typename... ParamTypes>
EventHandler<ParamTypes...>::EventHandler(Event<ParamTypes...> * event, std::function<void(ParamTypes...)> func)
{
    this->event = event;
    mutex = event->mutex;
    handler_function = func;
}
template <typename... ParamTypes>
EventHandler<ParamTypes...>::~EventHandler()
{
    std::lock_guard<std::shared_mutex> lock(*mutex.get());
    if (event != nullptr)
        event->handlers.erase(this);
}

template <typename... ParamTypes>
Event<ParamTypes...>::Event()
{
    mutex = std::make_shared<std::shared_mutex>();
}
template <typename... ParamTypes>
Event<ParamTypes...>::~Event()
{
    std::lock_guard<std::shared_mutex> lock(*mutex.get());
    for (auto handler : handlers)
        handler->event = nullptr;
}

template <typename... ParamTypes>
void Event<ParamTypes...>::call(ParamTypes... params)
{
    std::shared_lock<std::shared_mutex> lock(*mutex.get());
    for (auto handler : handlers)
        handler->handler_function(params...);
}

template <typename... ParamTypes>
std::shared_ptr<EventHandler<ParamTypes...>> Event<ParamTypes...>::handle(std::function<void(ParamTypes...)> handler_function)
{
    std::lock_guard<std::shared_mutex> lock(*mutex.get());
    std::shared_ptr<EventHandler<ParamTypes...>> result = std::shared_ptr<EventHandler<ParamTypes...>>(
        new EventHandler<ParamTypes...>(this, handler_function)
    );
    handlers.insert(result.get());
    return result;
}

// NAMESPACE END
#ifdef PRAGMAONCE_EVENT_NAMESPACE_OVERRIDE
    }
#else
    #ifndef PRAGMAONCE_EVENT_NAMESPACE_REMOVE // Default
        }
    #endif
#endif
