#include <iostream>
#include <fstream>
#include "../Libraries/json.hpp"
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

class AI_API_Python_Process {
public:
    AI_API_Python_Process() = default;
    std::map<std::string, std::vector<std::string>> task_map;

    std::map<std::string, std::vector<std::string>> test_openai_video_parser(std::string youtube_url) {
        try {
            const std::string command = "python3 Networking/OpenAI/openai_caller.py " + youtube_url;
            execPythonScript(command);

            // Read from the output file
            std::ifstream infile("output.json");
            if (!infile.is_open()) {
                throw std::runtime_error("Failed to open output.json");
            }

            nlohmann::json json_obj;
            infile >> json_obj;

            // Assuming the JSON structure is a map<string, vector<string>>
            task_map = json_obj;

            // for (const auto& [key, values] : task_map) {
            //     std::cout << key << ":\n";
            //     for (const auto& value : values) {
            //         std::cout << "  - " << value << '\n';
            //     }
            // }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return task_map;
        }
        
        return task_map;
    }

private:
    void execPythonScript(const std::string& command) {
        int result = system(command.c_str());
        if (result != 0) {
            throw std::runtime_error("Failed to execute python script");
        }
    }
};