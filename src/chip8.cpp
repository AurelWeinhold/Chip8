/**
 * Author: Aurel Weinhold
 * File: src/chip8.c
 * Created: 10:18 19.05.2020
 * License: MIT
 *
 * Information is fetched from the [chip-8 Wikipedia article](https://en.wikipedia.org/wiki/CHIP-8)
 */

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "Chip8Config.h"

typedef struct chip8 {
    uint16_t *memory;
} chip8;

int main(int argc, char *args[]) {
    std::cout << "Hello world! Version: " << Chip8_VERSION_MAJOR << "." << Chip8_VERSION_MINOR << "\n\n";

    // NOTE(aurel): Memory:
    //   - 4096 Bytes = 4KB
    //   - fist 512 Bytes reserved for the chip-8 interpreter itself -> most programs begin at memory
    //     location 512 (0x200) --> Today common to store font data here.
    //   - uppermost 256 bytes (0xF00 - 0xFFF) reserved for display refresh
    //   - the 96 bytes below (0xEA0 - 0xEFF) reserved for the call stack, internal use, and other variables.

    // NOTE(aurel): Registers:
    //  - 16 8-bit registers named V0 - VF
    //  - VF acts as a flag for some operations -> should be avoided
    //  - address register, called I, is 16 bits wide and is used with several opcodes that involve memory
    //    operations

    // NOTE(aurel): Stack:
    //  - only used to store return addresses when subroutines are called

    // NOTE(aurel): Timers:
    //  - Delay timer: intended to be used to time events of games
    //  - Sound timer: used for sound effects. When its value is non-zero, a beeping sound is made.




    // TODO(aurel): Initialize memory
    // NOTE(aurel): Chars are 8-bit wide!
    char *memory = new (std::nothrow) char[4096];

    // TODO(aurel): load the ROM into memory
    std::ifstream rom;
    std::streampos size;

    // NOTE(aurel): Open file in read mode (in), binary mode (binary) and set pointer to end of file (ate)
    rom.open("c8games/PONG", std::ios::in | std::ios::binary | std::ios::ate);
    if (rom.is_open()) {
        size = rom.tellg();
        rom.seekg(0, std::ios::beg);
        rom.read(memory, size);
        rom.close();
    }

    //std::cout << size;
    for (int i = 0; i < size; i += 2) {
        uint16_t instruction = (memory[i] << 8) | memory[i + 1];
        uint16_t instruction2 = (memory[i + 1] << 8) | memory[i];
        switch (instruction) {
            case (0x00e0):
                std::cout << "Display" << '\n';
            case (0x00ee):
                std::cout << "return" << '\n';
            default:
                std::cout << "Unknown instruction: " << std::hex << std::setw(4) << std::setfill('0') << instruction << " - ";
                std::cout << std::hex << std::setw(4) << std::setfill('0') << instruction2 << '\n';
        }
    }

    // TODO(aurel): emulate the program counter

    // TODO(aurel): fetch the current opcode

    return 0;
}
