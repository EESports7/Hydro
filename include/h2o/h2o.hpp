#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <cmath>
#include <array>
#include "util.hpp"

/*
    Reference Encoder/Decoder For Hydro v2.0.0

    Use for reference only
    Not intended for actual use

    Currently doesn't support Zstandard
*/

namespace h2o{
    enum class InputType : uint8_t{
        P1JUMPCLICK = 0,
        P1JUMPREL = 1,
        P2JUMPCLICK = 2,
        P2JUMPREL = 3,
        TPSCHANGE = 4,
        DEATH = 5,
        SETPHYSICS = 6,
        RESERVED = 7,
        P1LEFTCLICK = 8,
        P1LEFTREL = 9,
        P1RIGHTCLICK = 10,
        P1RIGHTREL = 11,
        P2LEFTCLICK = 12,
        P2LEFTREL = 13,
        P2RIGHTCLICK = 14,
        P2RIGHTREL = 15
    };

    struct InputPair{
        uint64_t frame;
        uint8_t input;

        InputPair() = default;
        InputPair(uint64_t a, uint8_t b){
            frame = a;
            input = b;
        }
    };

    struct DeltaPair{
        uint32_t delta;
        uint8_t input;

        DeltaPair() = default;
        DeltaPair(uint32_t a, uint8_t b){
            delta = a;
            input = b;
        }
    };

    struct SemVer{ // good enough
        uint16_t major;
        uint16_t minor;
        uint16_t patch;

        SemVer() = default;
        SemVer(uint16_t a, uint16_t b, uint16_t c){
            major = a;
            minor = b;
            patch = c;
        }
    };

    struct PhysicsState{
        double xPos;
        double yPos;
        double xVel;
        double yVel;
        double rot;

        PhysicsState() = default;
        PhysicsState(double a, double b, double c, double d, double e){
            xPos = a;
            yPos = b;
            xVel = c;
            yVel = d;
            rot = e;
        }
    };

    constinit std::array<uint8_t,6> magicNumber = {0x48,0x32,0x4F,0x47,0x4D,0x44};

    const SemVer hydroVersion = SemVer(2,0,0);

    class Replay{
        uint32_t maxDelta = 0;

        uint16_t bitmask = 0;

        std::vector<int32_t> seeds;
        std::vector<double> tps = {0.0};

        std::vector<InputPair> playerInputs;
        std::vector<DeltaPair> playerInputsDelta;

        uint8_t physicsFreq = 1;
        std::vector<PhysicsState> player1States;
        std::vector<PhysicsState> player2States;
    public:
        void configurePlatformer(bool plat){
            bitmask &= 0xFDFF;
            if(plat){
                bitmask |= 0x0200;
            }
        }
        
        void configurePhysics(uint8_t mode, uint8_t freq = 1){
            bitmask &= 0x9FFF;
            switch (mode){
            case 1:
                bitmask |= 0x2000;
                physicsFreq = freq;
                break;
            case 2:
                bitmask |= 0x4000;
                break;
            case 3:
                bitmask |= 0x6000;
                break;
            default:
                break;
            }
        }

        // initial tps is frame 0
        void changeTPS(double TPS, uint64_t frame = 0){
            if(frame == 0){
                tps[0] = TPS;
            }else{
                playerInputs.emplace_back(frame,static_cast<uint8_t>(InputType::TPSCHANGE));
                tps.emplace_back(TPS);
            }
        }

        void addInput(uint64_t frame, uint8_t input){
            playerInputs.emplace_back(frame,input);
        }

        void addInput(uint64_t frame, InputType input){
            playerInputs.emplace_back(frame,static_cast<uint8_t>(input));
        }

        void savePlayerPhysics(const PhysicsState& player1, const PhysicsState& player2){
            player1States.emplace_back(player1.xPos,player1.yPos,player1.xVel,player1.yVel,player1.rot);
            player2States.emplace_back(player2.xPos,player2.yPos,player2.xVel,player2.yVel,player2.rot);
        }

        void popInput(){
            if(playerInputs.back().input == static_cast<uint8_t>(InputType::TPSCHANGE)){
                tps.pop_back();
            }else if(playerInputs.back().input == static_cast<uint8_t>(InputType::SETPHYSICS)){
                player1States.pop_back();
                player2States.pop_back();
            }
            playerInputs.pop_back();
        }

        void cleanMacro(){ // removes redundant releases
            bool p1Up = false;
            bool p1Left = false;
            bool p1Right = false;
            bool p2Up = false;
            bool p2Left = false;
            bool p2Right = false;

            size_t vectorSize = playerInputs.size();

            for(size_t i = 0; i < vectorSize; i++){
                uint8_t currInput = playerInputs[i].input;
                switch (currInput){
                case static_cast<uint8_t>(InputType::P1JUMPCLICK):
                    p1Up = true;
                    break;
                case static_cast<uint8_t>(InputType::P1JUMPREL):
                    if(!p1Up){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p1Up = false;
                    }
                    break;
                case static_cast<uint8_t>(InputType::P2JUMPCLICK):
                    p2Up = true;
                    break;
                case static_cast<uint8_t>(InputType::P2JUMPREL):
                    if(!p2Up){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p2Up = false;
                    }
                    break;
                case static_cast<uint8_t>(InputType::P1LEFTCLICK):
                    p1Left = true;
                    break;
                case static_cast<uint8_t>(InputType::P1LEFTREL):
                    if(!p1Left){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p1Left = false;
                    }
                    break;
                case static_cast<uint8_t>(InputType::P1RIGHTCLICK):
                    p1Right = true;
                    break;
                case static_cast<uint8_t>(InputType::P1RIGHTREL):
                    if(!p1Right){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p1Right = false;
                    }
                    break;
                case static_cast<uint8_t>(InputType::P2LEFTCLICK):
                    p2Left = true;
                    break;
                case static_cast<uint8_t>(InputType::P2LEFTREL):
                    if(!p2Left){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p2Left = false;
                    }
                    break;
                case static_cast<uint8_t>(InputType::P2RIGHTCLICK):
                    p2Right = true;
                    break;
                case static_cast<uint8_t>(InputType::P2RIGHTREL):
                    if(!p2Right){
                        playerInputs.erase(playerInputs.begin()+i);
                        vectorSize--;
                        i--;
                    }else{
                        p2Right = false;
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void prepareDeltaVector(){
            playerInputsDelta.clear();
            uint64_t currFrame = 0;
            for(const auto& i : playerInputs){
                uint32_t delta = i.frame - currFrame;
                playerInputsDelta.emplace_back(delta,i.input);
                currFrame = i.frame;
                if(maxDelta < delta){
                    maxDelta = delta;
                }
            }
        }

        uint8_t calculateDeltaSize(){
            uint8_t maxSize;
            for(uint8_t i = 4; i <= 32; i+=4){
                if(pow(2,i) > maxDelta){
                    maxSize = i;
                    break;
                }
            }
            bitmask &= 0xE3FF;
            switch (maxSize){
            case 8:
                bitmask |= 0x0400;
                break;
            case 12:
                bitmask |= 0x0800;
                break;
            case 16:
                bitmask |= 0x0C00;
                break;
            case 20:
                bitmask |= 0x1000;
                break;
            case 24:
                bitmask |= 0x1400;
                break;
            case 28:
                bitmask |= 0x1800;
                break;
            case 32:
                bitmask |= 0x1C00;
                break;
            }
            return maxSize;
        }
        
        uint8_t exportFile(std::string macroName){ // no compression for now
            std::filesystem::path fileName = macroName + ".hydro";
            std::filesystem::path directory = "hydro";
            
            std::filesystem::path filePath = directory / fileName;
            
            if(std::filesystem::exists(filePath)){
                return 1;
            }
            
            std::ofstream outFile(filePath, std::ios::binary | std::ios::app);
            
            // store macro data block
            util::BitWriter macroStream;
            
            macroStream << static_cast<uint32_t>(seeds.size());
            for(const auto& i : seeds){
                macroStream << i;
            }
            
            macroStream << static_cast<uint32_t>(tps.size());
            for(const auto& i : tps){
                macroStream << i;
            }
            
            uint8_t bitSize = calculateDeltaSize();
            uint8_t inputSize = ((bitmask & 0x0200) >> 9) + 3; // check for platformer mode
            
            macroStream << static_cast<uint32_t>(playerInputsDelta.size());
            for(const auto& i : playerInputsDelta){
                macroStream.writeBits(i.delta,bitSize);
                macroStream.writeBits(i.input,inputSize);
            }
            
            if(bitmask & 0x6000){ // check for physics mode
                macroStream << physicsFreq;
                
                macroStream << static_cast<uint32_t>(player1States.size());
                for(const auto& i : player1States){
                    macroStream << i.xPos << i.yPos << i.xVel << i.yVel << i.rot;
                }
                
                macroStream << static_cast<uint32_t>(player2States.size());
                for(const auto& i : player2States){
                    macroStream << i.xPos << i.yPos << i.xVel << i.yVel << i.rot;
                }
            }
            
            // store header block
            util::BitWriter headerStream;
            headerStream 
            << magicNumber[0] << magicNumber[1] << magicNumber[2] << magicNumber[3] << magicNumber[4] << magicNumber[5]
            << hydroVersion.major << hydroVersion.minor << hydroVersion.patch
            << util::crc32(macroStream.data())
            << bitmask;
            
            // write header and macro streams
            outFile.write(reinterpret_cast<const char*>(headerStream.data().data()),headerStream.data().size());
            outFile.write(reinterpret_cast<const char*>(macroStream.data().data()),macroStream.data().size());

            outFile.close();
            return 0;
        }
        
        uint8_t importHydro(std::filesystem::path fileName, bool doCRC){
            std::filesystem::path directory = "hydro";
            std::filesystem::path filePath = directory / fileName;
            
            if (!std::filesystem::exists(filePath)) {
                return 1;
            }

            util::BitReader readFile = util::BitReader(filePath);

            std::array<uint8_t,6> magicCheck;
            for(uint8_t i = 0; i < 6; i++){
                magicCheck[i] = readFile.readBits(8);
            }
            
            if(magicCheck != magicNumber){
                return 2;
            }
            
            std::array<uint16_t,3> verRef = {hydroVersion.major,hydroVersion.minor,hydroVersion.patch};
            std::array<uint16_t,3> verCheck;
            for(uint8_t i = 0; i < 3; i++){
                verCheck[i] = readFile.readBits(16);
            }
            
            if(verCheck != verRef){
                return 3;
            }
            
            uint32_t crc32 = readFile.readBits(32);

            bitmask = readFile.readBits(16);

            uint8_t deltaSize = (((bitmask & 0x1C00) >> 10) + 1) << 2;
            uint8_t inputSize = ((bitmask & 0x0200) >> 9) + 3;

            uint32_t temp = readFile.readBits(32);
            for(uint32_t i = 0; i < temp; i++){
                seeds.emplace_back(readFile.readBits(32));
            }

            tps.pop_back(); // remove initial zero
            temp = readFile.readBits(32);
            for(uint32_t i = 0; i < temp; i++){
                double tickRate = readFile.readDouble();
                tps.emplace_back(tickRate);
            }

            temp = readFile.readBits(32);
            for(uint32_t i = 0; i < temp; i++){
                uint32_t delta = readFile.readBits(deltaSize);
                uint8_t action = readFile.readBits(inputSize);
                playerInputsDelta.emplace_back(delta,action);
            }

            uint64_t currFrame = 0;
            for(const auto& i : playerInputsDelta){
                playerInputs.emplace_back(currFrame+i.delta,i.input);
                currFrame += i.delta;
            }

            if(bitmask & 0x6000){
                physicsFreq = readFile.readBits(8);

                temp = readFile.readBits(32);
                for(uint32_t i = 0; i < temp; i++){
                    double xPos = readFile.readDouble();
                    double yPos = readFile.readDouble();
                    double xVel = readFile.readDouble();
                    double yVel = readFile.readDouble();
                    double rot = readFile.readDouble();
                    player1States.emplace_back(xPos,yPos,xVel,yVel,rot);
                }

                temp = readFile.readBits(32);
                for(uint32_t i = 0; i < temp; i++){
                    double xPos = readFile.readDouble();
                    double yPos = readFile.readDouble();
                    double xVel = readFile.readDouble();
                    double yVel = readFile.readDouble();
                    double rot = readFile.readDouble();
                    player2States.emplace_back(xPos,yPos,xVel,yVel,rot);
                }
            }

            if(doCRC){
                // reconstruct macro data block
                util::BitWriter macroStream;
                
                macroStream << static_cast<uint32_t>(seeds.size());
                for(const auto& i : seeds){
                    macroStream << i;
                }
                
                macroStream << static_cast<uint32_t>(tps.size());
                for(const auto& i : tps){
                    macroStream << i;
                }
                
                uint8_t bitSize = (((bitmask & 0x1C00) >> 10) + 1) << 2;
                uint8_t inputSize = ((bitmask & 0x0200) >> 9) + 3; // check for platformer mode
                
                macroStream << static_cast<uint32_t>(playerInputsDelta.size());
                for(const auto& i : playerInputsDelta){
                    macroStream.writeBits(i.delta,bitSize);
                    macroStream.writeBits(i.input,inputSize);
                }
                
                if(bitmask & 0x6000){ // check for physics mode
                    macroStream << physicsFreq;
                    
                    macroStream << static_cast<uint32_t>(player1States.size());
                    for(const auto& i : player1States){
                        macroStream << i.xPos << i.yPos << i.xVel << i.yVel << i.rot;
                    }
                    
                    macroStream << static_cast<uint32_t>(player2States.size());
                    for(const auto& i : player2States){
                        macroStream << i.xPos << i.yPos << i.xVel << i.yVel << i.rot;
                    }
                }

                if(util::crc32(macroStream.data()) != crc32){
                    return 4;
                }
            }

            return 0;
        }

        uint8_t importPlaintext(std::filesystem::path fileName){
            std::filesystem::path directory = "plaintext";
            std::filesystem::path filePath = directory / fileName;
            
            std::ifstream readFile(filePath);
            if (!readFile.is_open()) {
                return 1;
            }

            std::string tempTPS;
            readFile >> tempTPS;
            tps[0] = std::stod(tempTPS);

            uint64_t frame;
            bool holding;
            bool player2;
            while(readFile >> frame >> holding >> player2){
                if(holding){
                    if(player2){
                        addInput(frame,InputType::P2JUMPCLICK);
                    }else{
                        addInput(frame,InputType::P1JUMPCLICK);
                    }
                }else{
                    if(player2){
                        addInput(frame,InputType::P2JUMPREL);
                    }else{
                        addInput(frame,InputType::P1JUMPREL);
                    }
                }
            }

            readFile.close();
            return 0;
        }

        // getters
        uint16_t viewBitmask(){ return bitmask; }
        
        std::vector<int32_t> viewSeeds(){ return seeds; }
        std::vector<double> viewTPS(){ return tps; }
        std::vector<InputPair> viewInputBlock(){ return playerInputs; }
        uint8_t viewPhysicsFreq(){ return physicsFreq; }
    };
}