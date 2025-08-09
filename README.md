# Hydro Macro Format

> [!NOTE]
> I've decided that I will release the documentation first and later complete the reference decoder/encoder for both Hydro and [Oxy](https://github.com/EESports7/Oxy).   
> I might make a FAQ section if I feel the need to.   
> If you have any questions, feel free to make an issue request.   

There exists mainly **2 types** of botters, ones who bot levels on the ILL and ones who bot levels like [Suffer](https://www.youtube.com/watch?v=Hq45gXcHfoE). This format is designed for the latter because as macro size increases, so does parsing time to a noticeable amount. This is easily solved by using possibly the simplest macro structure possible, also allowing easy multithreading. This format accounts for GDR metadata along with a few new features to QOL features. If you bot levels casually, I recommend using [Oxy](https://github.com/EESports7/Oxy) instead because many of Hydro's advantages are useless at lower macro sizes.
## Structure

Version 1.0.0

Files should end in ".hydro"
### Header (Metadata)
```
std::string magicNumber = "H2OGMD";                // 
h2o::SemVar hydroVersion = h2o::SemVer(1,0,0);     // 
uint32_t errorCheck;                               // Uses CRC32 for integrity checks

std::string macroName;                             // 
std::string macroCreator;                          // 
std::string macroDescription;                      // Optional

uint32_t gameVersion;                              // 2.2704 -> 22704

std::string botName;                               // 
h2o::SemVer botVersion;                            // 

std::string levelName;                             // 
uint32_t levelID;                                  // 

uint64_t replayTime;                               // Contains the time it takes to replay the entire macro

uint32_t deathCount;                               // Contains how many times the intentional death feature is used

uint32_t p1InputCount;                             // 
uint32_t p2InputCount;                             // 

std::vector<int32_t> seeds;                        // If there is only one seed, that seed will be use for all attempts

uint16_t bitmask;                                  // Information can be found below
```

### Macro Data (Compressed)

This should be the only data compressed if compression is used

```
std::vector<PlayerPos> playerSpawn;          // First position is the starting position, contains where the player spawns for each attempt on replay
std::vector<double> tps;                     // First TPS is the starting TPS

std::vector<InputPair> pairs;                //

// These variables should only be read if indicated by the bitmask
uint8_t physicsFreq = 1;                     // Defaults to 1
std::vector<PhysicsState> player1States;     //
std::vector<PhysicsState> player2States;     //
```

## Data Structures

> [!IMPORTANT] 
> Vectors should be encoded starting with their length in 32 bits, followed by their contents.   
> Strings should be null-terminated.   
### Bitmask

| Size   | Data                                                                        |
| ------ | --------------------------------------------------------------------------- |
| 1 Bit  | Compression Mode (None or Zstandard)                                        |
| 2 Bits | Physics Mode (None, every n frames, every input, or explicitly stated only) |
| 3 Bits | Delta Size (4, 8, 12, 16, 20, 24, 28, or 32 bits)                           |
| 1 Bit  | Platform Mode                                                               |
| 1 Bit  | Low Detail Mode                                                             |
| 3 Bits | Coin Collected                                                              |
| 5 Bits | Reserved (Ignore)                                                           |

### h2o::SemVer
```
uint16_t major;
uint16_t minor;
uint16_t patch;
std::string prNote;
```

### h2o::PlayerPos
```
double xPos;
double yPos;
```

### h2o::PhysicsState
```
double xPos;
double yPos;
double xVel;
double yVel;
double rot;
```

### h2o::InputPair
```
uint32_t delta;     // When encoded, the variable size depends on the size specified in the bitmask
uint8_t input;      // When encoded, the variable size depends if platformer mode is enabled (4 bit if on, 3 bits if off)
```

## Input Values

Which number corresponds to which input

| Number | Action                      |
| ------ | --------------------------- |
| 0      | Player 1 Jump Click         |
| 1      | Player 1 Jump Release       |
| 2      | Player 2 Jump Click         |
| 3      | Player 2 Jump Release       |
| 4      | TPS Change                  |
| 5      | Intentional Death           |
| 6      | Set Player Physics/Position |
| 7      | Reserved                    |
| 8      | Player 1 Left Click         |
| 9      | Player 1 Left Release       |
| 10     | Player 1 Right Click        |
| 11     | Player 1 Right Release      |
| 12     | Player 2 Left Click         |
| 13     | Player 2 Left Release       |
| 14     | Player 2 Right Click        |
| 15     | Player 2 Right Release      |

# Special Thanks

[GDR](https://github.com/maxnut/GDReplayFormat/tree/gdr2) - Despite its flaws, it is the first successful universal format with lots of metadata   
[SLC](https://github.com/silicate-bot/slc) - Very amazing format that balances size and performance   
any format that used compression (e.g. [Rush - LZMA](http://discord.gg/Pj9nyfTMWh), [OBot - LZMA](https://discord.com/invite/2PaSqR92Dv), RBot - Gzip) - Honestly the most underrated feature I have ever seen used   
