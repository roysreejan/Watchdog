#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

int main(){
    // Read the config file: config.txt
    ifstream config_file("config.txt");
    string line;
    vector<string> config;
    while(getline(config_file, line)){
        config.push_back(line);
    }

    // foreach over config vector
    for (auto &i : config){
        // TODO: further processing of the line 
        // typecast i to string
        string process_name = i;

        // issue pidof -s <process_name> command and read the output into a string
        string command = "pidof -s " + process_name;
        FILE *in;
        char buff[512];
        if(!(in = popen(command.c_str(), "r"))){
            return 1;
        }
        string result = "";
        while(fgets(buff, sizeof(buff), in)!=NULL){
            result += buff;
        }
        pclose(in);

        // if the output is empty, then the process is not running
        if (result == ""){
            cout << "Process " << process_name << " is not running" << endl;
        }

        // if the output is not empty, then the process is running
        else{
            cout << "Process " << process_name << " is running" << endl;
            // print the pid of the process
            cout << "PID: " << result << endl;
        }

    }

    
}
