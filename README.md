# Hydro Macro Format

> [!NOTE]
> I've decided that I will release the documentation first and later complete the reference decoder/encoder for both Hydro and [Oxy](https://github.com/EESports7/Oxy).   
> I might make a FAQ section if I feel the need to.   
> If you have any questions, feel free to make an issue request.   

There exists mainly **2 types** of botters, ones who bot levels on the ILL and ones who bot levels like [Suffer](https://www.youtube.com/watch?v=Hq45gXcHfoE). This format is designed for the latter because as macro size increases, so does parsing time to a noticeable amount. This is easily solved by using possibly the simplest macro structure possible, also allowing easy multithreading. If you bot levels casually, I recommend using [Oxy](https://github.com/EESports7/Oxy) instead because many of Hydro's advantages are useless at lower macro sizes.
## Structure

Version 2.0.0

Files should end in ".hydro"
### Header (Metadata)
```
std::string magicNumber = "H2OGMD";                // 
h2o::SemVar hydroVersion = h2o::SemVer(1,1,1);     // 
uint32_t errorCheck;                               // Uses CRC32 on macro data for integrity checks

uint16_t bitmask;                                  // Information can be found below
```

### Macro Data (Compressed)

This should be the only data compressed if compression is used

```
std::vector<int32_t> seeds;                  // If there is only one seed, that seed will be use for all attempts
std::vector<double> tps;                     // First TPS is the starting TPS

std::vector<InputPair> playerInputs;         //

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
| 1 Bit  | Compression Mode (None or LZMA2)                                            |
| 2 Bits | Reserved                                                                    |
| 3 Bits | Delta Size (4, 8, 12, 16, 20, 24, 28, or 32 bits)                           |
| 1 Bit  | Platform Mode                                                               |
| 9 Bits | Reserved (Ignore)                                                           |

### h2o::SemVer
```
uint16_t major;
uint16_t minor;
uint16_t patch;
std::string prNote;
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

[GDR](https://github.com/maxnut/GDReplayFormat/tree/gdr2) - Despite its flaws, it is the first successful universal format   
[SLC](https://github.com/silicate-bot/slc) - Very amazing format that balances size and performance   
any format that used compression (e.g. [Rush - LZMA](http://discord.gg/Pj9nyfTMWh), [OBot - LZMA](https://discord.com/invite/2PaSqR92Dv), RBot - Gzip) - Honestly the most underrated feature I have ever seen used   
[ZCB](https://github.com/zeozeozeo/zcb3) - Great source of public domain macro encoders/decoders   
