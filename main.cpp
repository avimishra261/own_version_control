#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "zstr.hpp"

int main(int argc, char * argv[]){
    if(argc < 2){
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[1];

    if(command == "init"){
        try{
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");

            std::ofstream headFile(".git/HEAD");
            if(headFile.is_open()){
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            }else{
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }
            std::cout << "Initialised git directory\n";
        }catch(const std::filesystem::filesystem_error& e){
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }else if(command == "cat-file"){
        if(argc <= 3){
            std::cerr << "Invalid arguments, required '-p <blob_sha>'\n";
            return EXIT_FAILURE;
        }

        const std::string flag = argv[2]; //  Getting flag like -p for cat-file command
        if(flag != "-p"){
            std::cerr << "Invalid flag for cat-file, expected '-p' flag\n";
            return EXIT_FAILURE;
        }
        const std::string value = argv[3]; // Getting the sha value of blob
        const std::string dir_name = value.substr(0,2); // In git dir is made up of starting 2 chars of sha value
        const std::string blob_sha = value.substr(2); // Rest of the chars in sha becomes the file name in the dir

        std::string path = ".git/objects" + dir_name + "/" + blob_sha;
        zstr::ifstream input(path, std::ofstream::binary);
        if(!input.is_open()){
            std::cerr << "Failed to open the object file\n";
            return EXIT_FAILURE;
        }
        std::string object_str(std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>());
        input.close();
        const auto object_content = object_str.substr(object_str.find('\0')+1);
        std::cout << object_content << std::flush;

    }else{
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}