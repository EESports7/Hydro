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
    std::string cleanMacro;
    std::cout << "\n" << "Clean Macro/Remove Redunant Releases (y/n): ";
    std::cin >> cleanMacro;
    
    if(cleanMacro == "y"){ globalReplay.cleanMacro(); }
    
    globalReplay.calculateInputCount();
    globalReplay.prepareDeltaVector();
    
    uint8_t exportLog = globalReplay.exportFile();
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
    globalReplay.macroMetadata("Friend List Challenge","EESports","this is the first ever hydro macro");
    globalReplay.levelMetadata("Friend List Challeng",70823189,false,false,false,false,false);
    globalReplay.botMetadata("xdBot",h2o::SemVer(2,4,0));
    globalReplay.changeTPS(240);
    globalReplay.timeMetadata(5163,1239);
    
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
    
    globalReplay.calculateInputCount();
    globalReplay.prepareDeltaVector();

    uint8_t exportLog = globalReplay.exportFile();
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
    std::cout << "\n" << "Macro Name: " << globalReplay.viewMacroName() << "\n"
    << "Macro Creator: " << globalReplay.viewMacroCreator() << "\n"
    << "Macro Description: " << globalReplay.viewMacroDescription() << "\n"
    << "Game Version: " << globalReplay.viewGameVersion() << "\n"
    << "Bot Name: " << globalReplay.viewBotName() << "\n"
    << "Bot Version: " 
    << globalReplay.viewBotVersion().major 
    << "." << globalReplay.viewBotVersion().minor
    << "." << globalReplay.viewBotVersion().patch
    << globalReplay.viewBotVersion().prNote << "\n"
    << "Level Name: " << globalReplay.viewLevelName() << "\n"
    << "Level ID: " << globalReplay.viewLevelID() << "\n"
    << "Replay Time: " << globalReplay.viewReplayTime() << " ms\n"
    << "Total Frames: " << globalReplay.viewTotalFrames() << " frames\n"
    << "Death Count: " << globalReplay.viewDeathCount() << " deaths\n"
    << "Player 1 Input Count: " << globalReplay.viewP1InputCount() << " inputs\n"
    << "Player 2 Input Count: " << globalReplay.viewP2InputCount() << " inputs\n"
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

void editMetadata(){
    std::string mName, mCreator, mDescription;
    std::cout << "\n" << "Macro Name: ";
    std::getline(std::cin >> std::ws,mName);
    std::cout << "\n" << "Macro Creator: ";
    std::getline(std::cin >> std::ws,mCreator);
    std::cout << "\n" << "Macro Description (n for none): ";
    std::getline(std::cin >> std::ws,mDescription);
    if(mDescription == "n"){ mDescription = ""; }
    globalReplay.macroMetadata(mName,mCreator,mDescription);
    
    std::string lvlName, lvlID, temp;
    std::cout << "\n" << "Level Name: ";
    std::getline(std::cin >> std::ws,lvlName);
    std::cout << "\n" << "Level ID: ";
    std::getline(std::cin >> std::ws,lvlID);
    
    std::string botName, verMajor, verMinor, verPatch, verPRNote;
    std::cout << "\n" << "Bot Name: ";
    std::getline(std::cin >> std::ws,botName);
    std::cout << "\n" << "Bot Version Major: ";
    std::getline(std::cin >> std::ws,verMajor);
    std::cout << "\n" << "Bot Version Minor: ";
    std::getline(std::cin >> std::ws,verMinor);
    std::cout << "\n" << "Bot Version Patch: ";
    std::getline(std::cin >> std::ws,verPatch);
    std::cout << "\n" << "Bot Version Pre-Release Note (n for none): ";
    std::getline(std::cin >> std::ws,verPRNote);
    if(verPRNote == "n"){ verPRNote = ""; }
    globalReplay.botMetadata(botName,h2o::SemVer(std::stoi(verMajor),std::stoi(verMinor),std::stoi(verPatch),verPRNote));
    
    bool LDM, platformer, coin1, coin2, coin3;
    LDM = platformer = coin1 = coin2 = coin3 = false;
    std::cout << "\n" << "Low Detail Mode (y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "y"){ LDM = true; temp = ""; }
    std::cout << "\n" << "Platformer Mode (y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "y"){ platformer = true; temp = ""; }
    std::cout << "\n" << "Coin 1 Collected (y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "y"){ coin1 = true; temp = ""; }
    std::cout << "\n" << "Coin 2 Collected (y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "y"){ coin2 = true; temp = ""; }
    std::cout << "\n" << "Coin 3 Collected (y/n): ";
    std::getline(std::cin >> std::ws,temp);
    if(temp == "y"){ coin3 = true; temp = ""; }
    globalReplay.levelMetadata(lvlName,std::stoi(lvlID),LDM,platformer,coin1,coin2,coin3);
    
    std::string replayTime, totalFrames;
    std::cout << "\n" << "Replay Time (ms): ";
    std::getline(std::cin >> std::ws,replayTime);
    std::cout << "\n" << "Total Frames: ";
    std::getline(std::cin >> std::ws,totalFrames);
    globalReplay.timeMetadata(std::stoi(replayTime),std::stoi(totalFrames));

    std::cout << "\n\n" << "Metadata Editing Complete" << "\n\n";
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
        << "6. Edit Metadata (Header)\n"
        << "7. Clear Memory\n"
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
            editMetadata();
            break;
        case 7:
            clearMemory();
            break;
        default:
            std::cout << "The following input is not valid, please try again\n";
            break;
        }
    }
}