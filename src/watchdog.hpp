#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

class Watchdog { 
public:
    Watchdog(const std::string& config_file_path, std::chrono::milliseconds config_reload_interval); // Create a watchdog object
    void start(); // Start the watchdog

private:
    void reload_config(); // Reload the configuration file

    std::string config_file_path; // The path to the configuration file
    nlohmann::json config; // The configuration
    std::chrono::milliseconds config_reload_interval; // The interval at which to check for changes in the configuration file
};