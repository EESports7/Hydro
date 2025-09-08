#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <chrono>
#include <bitset>
#include "h2o/h2o.hpp"

h2o::Replay globalReplay;

void createDirectories(){
    std::filesystem::path folder1 = "hydro";
    if (!std::filesystem::exists(folder1)) {
        std::filesystem::create_directory(folder1);
    }
    
    std::filesystem::path folder2 = "plaintext";
    if (!std::filesystem::exists(folder2)) {
        std::filesystem::create_directory(folder2);
    }
}

void plainToHydro(){
    std::string fileName;

    std::cout << "\n" << "Enter the file name: ";
    
    std::getline(std::cin >> std::ws,fileName);
    
    
    globalReplay = h2o::Replay();
    uint8_t r = globalReplay.importPlaintext(fileName);
    switch(r){
        case 0:
        std::cout << "\n" << "File Has Been Imported And Loaded Into Memory" << "\n\n";
        break;
        case 1:
        std::cout << "\n" << "An Error Occurred (File Open Failed)" << "\n\n";
        default:
        std::cout << "\n" << "An Error Occurred (Unknown)" << "\n\n";
        break;
    }
    
}

void exportMacro(){
    std::string macroName;
    std::cout << "\n" << "Macro Name: ";
    std::getline(std::cin >> std::ws,macroName); // 
    
    std::string plat;
    std::cout << "\n" << "Platformer? (y/n): ";
    std::cin >> plat;

    std::string cleanMacro;
    std::cout << "\n" << "Clean Macro/Remove Redunant Releases (y/n): ";
    std::cin >> cleanMacro;
    
    if(cleanMacro == "y"){ globalReplay.cleanMacro(); }

    if(plat == "y"){
        globalReplay.configurePlatformer(true);
    }else{
        globalReplay.configurePlatformer(false);
    }
    
    globalReplay.prepareDeltaVector();
    
    uint8_t exportLog = globalReplay.exportFile(macroName);
    switch (exportLog){
        case 0:
        std::cout << "\nMacro Has Been Exported \n\n";
        break;
        case 1:
        std::cout << "\nMacro Has Already Been Exported/File Exists With The Same Name\n\n";
        break;
    }
}

void importMacro(){
    std::string fileName;
    
    std::cout << "\n" << "Enter the file name: ";
    
    std::getline(std::cin >> std::ws,fileName);
    
    bool crc = true;
    std::string temp;
    std::cout << "\n" << "Check For Corruption(y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "n"){ crc = false; }

    globalReplay = h2o::Replay();
    uint8_t r = globalReplay.importHydro(fileName,crc);
    
    switch(r){
        case 0:
        std::cout << "\n" << "File Has Been Imported And Loaded Into Memory" << "\n\n";
        break;
        case 1:
            std::cout << "\n" << "An Error Occurred (File Open Failed)" << "\n\n";
            break;
        case 2:
            std::cout << "\n" << "An Error Occurred (Magic Number Doesn't Match)" << "\n\n";
            break;
        case 3:
            std::cout << "\n" << "An Error Occurred (Format Version Isn't Supported)" << "\n\n";
            break;
        case 4:
            std::cout << "\n" << "File Has Been Imported And Loaded Into Memory But Corruption Has Been Detected" << "\n\n";
            break;
        default:
            std::cout << "\n" << "An Error Occurred (Unknown)" << "\n\n";
            break;
    }
}

void testMacro(){
    globalReplay = h2o::Replay();
    globalReplay.changeTPS(240);
    
    using InputType = h2o::InputType;
    globalReplay.addInput(49,InputType::P1JUMPCLICK);
    globalReplay.addInput(60,InputType::P1JUMPREL);
    globalReplay.addInput(205,InputType::P1JUMPCLICK);
    globalReplay.addInput(213,InputType::P1JUMPREL);
    globalReplay.addInput(233,InputType::P1JUMPCLICK);
    globalReplay.addInput(291,InputType::P1JUMPREL);
    globalReplay.addInput(420,InputType::P1JUMPCLICK);
    globalReplay.addInput(428,InputType::P1JUMPREL);
    globalReplay.addInput(439,InputType::P1JUMPCLICK);
    globalReplay.addInput(447,InputType::P1JUMPREL);
    globalReplay.addInput(464,InputType::P1JUMPCLICK);
    globalReplay.addInput(470,InputType::P1JUMPREL);
    globalReplay.addInput(484,InputType::P1JUMPCLICK);
    globalReplay.addInput(490,InputType::P1JUMPREL);
    globalReplay.addInput(551,InputType::P1JUMPCLICK);
    globalReplay.addInput(606,InputType::P1JUMPREL);
    globalReplay.addInput(676,InputType::P1JUMPCLICK);
    globalReplay.addInput(683,InputType::P1JUMPREL);
    globalReplay.addInput(893,InputType::P1JUMPCLICK);
    globalReplay.addInput(905,InputType::P1JUMPREL);
    globalReplay.addInput(1049,InputType::P1JUMPCLICK);
    globalReplay.addInput(1060,InputType::P1JUMPREL);
    globalReplay.addInput(1226,InputType::P1JUMPCLICK);
    globalReplay.addInput(1239,InputType::P1JUMPREL);
    
    globalReplay.prepareDeltaVector();

    uint8_t exportLog = globalReplay.exportFile("Friend List Challenge");
    switch (exportLog){
    case 0:
        std::cout << "\nTest Macro Has Been Exported And Loaded Into Memory\n\n";
        break;
    case 1:
        std::cout << "\nTest Macro Has Already Been Exported But Has Been Loaded Into Memory\n\n";
        break;
    }
}

void viewMacroSnippet(){
    std::cout << "\n"
    << "Player 2 Input Count: " << globalReplay.viewInputBlock().size() << " inputs\n"
    << "Raw Hydro Bitmask: " << std::bitset<16>(globalReplay.viewBitmask()) << "\n\n";

    std::cout << "Initial TPS: " << globalReplay.viewTPS()[0] << " TPS\n\n";

    std::vector<h2o::InputPair> inputVector = globalReplay.viewInputBlock();
    uint8_t iterations = std::min(50,static_cast<int32_t>(inputVector.size()));

    for(uint16_t i = 0; i < iterations; i++){
        std::string playerInput;
        switch (inputVector[i].input){
        case 0:
            playerInput = "Player 1 Jump Click";
            break;
        case 1:
            playerInput = "Player 1 Jump Release";
            break;
        case 2:
            playerInput = "Player 2 Jump Click";
            break;
        case 3:
            playerInput = "Player 2 Jump Release";
            break;
        case 4:
            playerInput = "TPS Change";
            break;
        case 5:
            playerInput = "Intentional Death";
            break;
        case 6:
            playerInput = "Set Player Physics/Position";
            break;
        case 7:
            playerInput = "Reserved";
            break;
        case 8:
            playerInput = "Player 1 Left Click";
            break;
        case 9:
            playerInput = "Player 1 Left Release";
            break;
        case 10:
            playerInput = "Player 1 Right Click";
            break;
        case 11:
            playerInput = "Player 1 Right Release";
            break;
        case 12:
            playerInput = "Player 2 Left Click";
            break;
        case 13:
            playerInput = "Player 2 Left Release";
            break;
        case 14:
            playerInput = "Player 2 Right Click";
            break;
        case 15:
            playerInput = "Player 2 Right Release";
            break;
        }

        std::cout << "Input #" << i+1 << "\n"
        << "Frame: " << inputVector[i].frame << "\n"
        << "Input Type: " << playerInput << "\n\n";
    }
}

void clearMemory(){
    globalReplay = h2o::Replay();
    std::cout << "\n" << "Memory Has Been Cleared" << "\n\n";
}

int main(){
    createDirectories();
    
    while(true){
        std::cout << "Select one of the following\n" 
        << "1. Import Plaintext\n"
        << "2. Export Hydro Macro\n"
        << "3. Import Hydro Macro\n"
        << "4. Hydro Test Macro\n"
        << "5. List Macro Metadata & 50 Inputs\n"
        << "6. Clear Memory\n"
        << "\n";

        std::string option;

        std::cin >> option;

        switch (std::stoi(option)){
        case 0:
            return 0;
            break;
        case 1:
            plainToHydro();
            break;
        case 2:
            exportMacro();
            break;
        case 3:
            importMacro();
            break;
        case 4:
            testMacro();
            break;
        case 5:
            viewMacroSnippet();
            break;
        case 6:
            clearMemory();
            break;
        default:
            std::cout << "The following input is not valid, please try again\n";
            break;
        }
    }
}