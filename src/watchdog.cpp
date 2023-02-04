#include "watchdog.hpp"
#include <fstream>
#include <iostream>
#include <thread>

Watchdog::Watchdog(const std::string& config_file_path, std::chrono::milliseconds config_reload_interval) 
    : config_file_path(config_file_path), config_reload_interval(config_reload_interval) { 
    reload_config(); // Load the configuration file
}

void Watchdog::reload_config() {
    std::ifstream config_file(config_file_path); // Open the configuration file
    config_file >> config; // Read the configuration file into the config variable
}

void Watchdog::start() { // Start the watchdog
    while (true) { // Loop forever
        // Do the monitoring logic here
        // Check the status of each process listed in the configuration
        for (const auto& process_config : config["processes"]) { // Loop through each process
            std::string process_name = process_config["name"]; // Get the name of the process
            std::string process_executable = process_config["executable"]; // Get the executable of the process
            std::vector<std::string> process_arguments = process_config["arguments"]; // Get the arguments of the process

            // Use a system call to check if the process is running
            std::string cmd = "ps -C " + process_executable + " > /dev/null"; // Create the command
            int result = system(cmd.c_str()); // Execute the command
            if (result != 0) { // Check if the process is not running
                // The process is not running, restart it
                std::string process_start_command = process_executable; // Create the command to start the process
                for (const auto& argument : process_arguments) { // Add the arguments to the command
                    process_start_command += " " + argument; // Add the argument to the command
                }
                int start_result = system(process_start_command.c_str()); // Execute the command
                if (start_result == 0) { // Check if the process was started successfully
                    std::cout << process_name << " has been restarted." << std::endl; // Log that the process has been restarted
                } else {
                    std::cerr << "Error restarting " << process_name << "." << std::endl; // Log that there was an error restarting the process
                }
            }
        }
        // Check for changes in the configuration file
        std::ifstream config_file(config_file_path); // Open the configuration file
        nlohmann::json new_config; // Create a new variable to store the new configuration
        config_file >> new_config; // Read the configuration file into the new variable
        if (new_config != config) { // Check if the configuration has changed
            config = new_config; // Update the configuration
            // Log the change in the configuration
            std::cout << "Configuration reloaded." << std::endl; // Log the change in the configuration
        }

        // Sleep for the specified interval before checking the configuration again
        std::this_thread::sleep_for(config_reload_interval); // Sleep for the specified interval
    }
}