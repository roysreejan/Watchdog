#include "watchdog.hpp"

int main(int argc, char* argv[]) {
    Watchdog watchdog("config/config.json", std::chrono::seconds(10)); // Create a watchdog object
    watchdog.start(); // Start the watchdog
    return 0;
}