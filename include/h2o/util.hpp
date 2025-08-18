#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <cstdint>
#include <type_traits>
#include <cstring> // for std::memcpy
#include <fstream>
#include <stdexcept>

namespace util{
    class BitWriter {
    public:
        BitWriter() = default;

        template <class T, class = std::enable_if_t<std::is_integral_v<T>>>
        void writeBits(T value, size_t bitCount) {
            using U = std::make_unsigned_t<T>;
            if (bitCount == 0) return;

            const size_t maxBits = sizeof(U) * 8;
            if (bitCount > maxBits) bitCount = maxBits; // clamp

            U u = static_cast<U>(value);
            for (size_t i = 0; i < bitCount; ++i) {
                ensureByte();
                size_t shift = bitCount - 1 - i;
                uint8_t bit = static_cast<uint8_t>((u >> shift) & U{1});
                buffer_.back() |= static_cast<uint8_t>(bit << (7 - bitPos_));
                ++bitPos_;
            }
        }

        template <class T, class = std::enable_if_t<std::is_integral_v<T>>>
        BitWriter& operator<<(T value) {
            writeBits(value, sizeof(T) * 8);
            return *this;
        }

        BitWriter& operator<<(double value) {
            static_assert(sizeof(double) == 8, "Unexpected double size");
            uint64_t bits;
            std::memcpy(&bits, &value, 8);
            writeBits(bits, 64);
            return *this;
        }
        BitWriter& operator<<(float value) {
            static_assert(sizeof(float) == 4, "Unexpected float size");
            uint32_t bits;
            std::memcpy(&bits, &value, 4);
            writeBits(bits, 32);
            return *this;
        }

        BitWriter& operator<<(std::string_view s) {
            for (unsigned char ch : s) writeBits(ch, 8);
            writeBits(0, 8); // null terminated
            return *this;
        }
        BitWriter& operator<<(const std::string& s) { return (*this) << std::string_view{s}; }

        const std::vector<uint8_t>& data() const { return buffer_; }

        size_t bitSize() const {
            if (buffer_.empty()) return 0;
            size_t fullBytes = buffer_.size();
            size_t trailingUnused = (8 - (bitPos_ % 8)) % 8;
            return fullBytes * 8 - trailingUnused;
        }

    private:
        std::vector<uint8_t> buffer_;
        uint8_t bitPos_ = 0;

        void ensureByte() {
            if (buffer_.empty() || bitPos_ == 8) {
                buffer_.push_back(0);
                bitPos_ = 0;
            }
        }
    };

    uint32_t crc32(const std::vector<uint8_t>& data) {
        static uint32_t table[256];
        static bool tableInitialized = false;

        if (!tableInitialized) {
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t crc = i;
                for (int j = 0; j < 8; j++) {
                    if (crc & 1)
                        crc = (crc >> 1) ^ 0xEDB88320;
                    else
                        crc >>= 1;
                }
                table[i] = crc;
            }
            tableInitialized = true;
        }

        uint32_t crc = 0xFFFFFFFF;
        for (uint8_t byte : data) {
            crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF];
        }

        return crc ^ 0xFFFFFFFF;
    }

    class BitReader{
        std::vector<uint8_t> data;
        size_t bitPos = 0;

        public:
        BitReader(const std::filesystem::path& filepath){
            std::ifstream file(filepath, std::ios::binary | std::ios::ate);
            const std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            data.resize(static_cast<size_t>(size));
            file.read(reinterpret_cast<char*>(data.data()), size);
        }

        uint64_t readBits(size_t count){
            uint64_t value = 0;
            for (size_t i = 0; i < count; ++i) {
                size_t byteIndex = (bitPos + i) / 8;
                size_t bitIndex  = 7 - ((bitPos + i) % 8); // MSB first
                uint8_t bit = (data[byteIndex] >> bitIndex) & 1;
                value = (value << 1) | bit;
            }
            bitPos += count;
            return value;
        }

        float readFloat() {
            uint32_t raw = static_cast<uint32_t>(readBits(32));
            float f;
            std::memcpy(&f, &raw, 4);
            return f;
        }

        double readDouble() {
            uint64_t raw = readBits(64);
            double d;
            std::memcpy(&d, &raw, 8);
            return d;
        }
    };
}