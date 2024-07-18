#ifndef PROACTOR_REACTOR_HPP
#define PROACTOR_REACTOR_HPP

#include <iostream>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <poll.h>
#include <atomic>
#include <cstring>
#include <algorithm>

// Type definitions for function pointers
typedef void *(*proactorFunc)(int);
using reactorFunc = void *(*)(int fd);

// Proactor class
class Proactor {
public:
    Proactor();
    ~Proactor();

    static pthread_t startProactor(int sockfd, proactorFunc threadFunc);
    static int stopProactor(pthread_t tid);

private:
    static void *proactorThread(void *arg);

    static std::unordered_map<pthread_t, Proactor *> proactors;
    static pthread_mutex_t proactorsMutex;

    int sockfd;
    proactorFunc threadFunc;
    std::atomic<bool> running;
    pthread_mutex_t graphMutex;
};

// Reactor class
class Reactor {
public:
    Reactor();
    ~Reactor();

    static void *startReactor();
    static int addFdToReactor(void *reactor, int fd, reactorFunc func);
    static int removeFdFromReactor(void *reactor, int fd);
    static int stopReactor(void *reactor);

    void run();

private:
    std::unordered_map<int, reactorFunc> handlers;
    std::vector<struct pollfd> pollfds;
    bool running;
};

// Static member initializations for Proactor
std::unordered_map<pthread_t, Proactor *> Proactor::proactors;
pthread_mutex_t Proactor::proactorsMutex = PTHREAD_MUTEX_INITIALIZER;

// Proactor method implementations
Proactor::Proactor() : running(false) {
    pthread_mutex_init(&graphMutex, nullptr);
}

Proactor::~Proactor() {
    pthread_mutex_destroy(&graphMutex);
}

pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc) {
    Proactor *proactor = new Proactor();
    proactor->sockfd = sockfd;
    proactor->threadFunc = threadFunc;
    proactor->running = true;

    pthread_t tid;
    pthread_create(&tid, nullptr, proactorThread, proactor);

    pthread_mutex_lock(&proactorsMutex);
    proactors[tid] = proactor;
    pthread_mutex_unlock(&proactorsMutex);

    return tid;
}

int Proactor::stopProactor(pthread_t tid) {
    pthread_mutex_lock(&proactorsMutex);
    auto it = proactors.find(tid);
    if (it == proactors.end()) {
        pthread_mutex_unlock(&proactorsMutex);
        return -1;  // Proactor not found
    }
    Proactor *proactor = it->second;
    proactors.erase(it);
    pthread_mutex_unlock(&proactorsMutex);

    proactor->running = false;
    pthread_join(tid, nullptr);
    delete proactor;
    return 0;
}

void *Proactor::proactorThread(void *arg) {
    Proactor *proactor = static_cast<Proactor *>(arg);

    while (proactor->running) {
        int clientSock = accept(proactor->sockfd, nullptr, nullptr);
        if (clientSock < 0) {
            if (proactor->running) {
                std::cerr << "Accept error" << std::endl;
            }
            continue;
        }

        pthread_t clientThread;
        pthread_create(&clientThread, nullptr,
                       [](void *arg) -> void * {
                           int *clientSockPtr = static_cast<int *>(arg);
                           int clientSock = *clientSockPtr;
                           delete clientSockPtr;

                           Proactor *proactor;
                           pthread_mutex_lock(&Proactor::proactorsMutex);
                           for (const auto &pair: Proactor::proactors) {
                               if (pthread_equal(pthread_self(), pair.first)) {
                                   proactor = pair.second;
                                   break;
                               }
                           }
                           pthread_mutex_unlock(&Proactor::proactorsMutex);

                           pthread_mutex_lock(&proactor->graphMutex);
                           void *result = proactor->threadFunc(clientSock);
                           pthread_mutex_unlock(&proactor->graphMutex);

                           close(clientSock);
                           return result;
                       },
                       new int(clientSock)
        );
        pthread_detach(clientThread);
    }

    return nullptr;
}

// Reactor method implementations
Reactor::Reactor() : running(false) {}

Reactor::~Reactor() {
    if (running) {
        stopReactor(this);
    }
}

void *Reactor::startReactor() {
    Reactor *reactor = new Reactor();
    reactor->running = true;
    return static_cast<void *>(reactor);
}

int Reactor::addFdToReactor(void *reactorPtr, int fd, reactorFunc func) {
    Reactor *reactor = static_cast<Reactor *>(reactorPtr);

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

int Reactor::removeFdFromReactor(void *reactorPtr, int fd) {
    Reactor *reactor = static_cast<Reactor *>(reactorPtr);

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

int Reactor::stopReactor(void *reactorPtr) {
    std::cout << "Stopping reactor" << std::endl;
    Reactor *reactor = static_cast<Reactor *>(reactorPtr);
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

#endif // PROACTOR_REACTOR_HPP
