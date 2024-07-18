#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <poll.h>
#include <iostream>
#include <algorithm>
#include <cstring>

using reactorFunc = void* (*)(int fd);

class Reactor {
public:
    Reactor();
    ~Reactor();

    static void* startReactor();
    static int addFdToReactor(void* reactor, int fd, reactorFunc func);
    static int removeFdFromReactor(void* reactor, int fd);
    static int stopReactor(void* reactor);

    void run();

private:
    std::unordered_map<int, reactorFunc> handlers;
    std::vector<struct pollfd> pollfds;
    bool running;
};

Reactor::Reactor() : running(false) {}

Reactor::~Reactor() {
    if (running) {
        stopReactor(this);
    }
}

void* Reactor::startReactor() {
    Reactor* reactor = new Reactor();
    reactor->running = true;
    return static_cast<void*>(reactor);
}

int Reactor::addFdToReactor(void* reactorPtr, int fd, reactorFunc func) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);

    if (reactor->handlers.find(fd) != reactor->handlers.end()) {
        return -1; // fd already exists
    }

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    reactor->pollfds.push_back(pfd);
    reactor->handlers[fd] = func;

    return 0;
}

int Reactor::removeFdFromReactor(void* reactorPtr, int fd) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);

    auto it = reactor->handlers.find(fd);
    if (it == reactor->handlers.end()) {
        return -1; // fd not found
    }

    reactor->handlers.erase(it);
    reactor->pollfds.erase(std::remove_if(reactor->pollfds.begin(), reactor->pollfds.end(),
                                          [fd](struct pollfd &pfd) { return pfd.fd == fd; }),
                           reactor->pollfds.end());

    return 0;
}

int Reactor::stopReactor(void* reactorPtr) {
    std::cout << "Stopping reactor" << std::endl;
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    reactor->running = false;
    delete reactor;
    std::cout << "Reactor stopped" << std::endl;
    return 0;
}

void Reactor::run() {
    running = true;
    while (running) {
        int ret = poll(pollfds.data(), pollfds.size(), -1);
        if (ret < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            continue;
        }

        for (auto &pfd : pollfds) {
            if (pfd.revents & POLLIN) {
                auto it = handlers.find(pfd.fd);
                if (it != handlers.end()) {
                    it->second(pfd.fd);
                }
            }
        }
    }
}

#endif // REACTOR_HPP
