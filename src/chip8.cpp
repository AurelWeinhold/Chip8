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
#include <regex>

#include "Chip8Config.h"

typedef struct chip8 {
    uint16_t *memory;
} chip8;

// TODO(aurel): Do I really want to save the instructions like this? Probably not.
typedef struct instruction {
    int f0,f1,f2,f3 : 4;
} instruction;

// TODO(aurel): Return the instruction.
void detectInstruction(uint16_t instruction) {
    std::string instr_str = "";

    // TODO(aurel): Probably rewrite to only check the first 4 bit in the outermost switch
    //  ((instruction & 0xf000) >> 3). This makes it more readable and avoids the ranged cases.
    switch (instruction) {
        case 0x0000 ... 0x0fff:
            switch (instruction & 0x0fff){
                case 0x0e0:
                    instr_str = "disp_clear()";
                    break;
                case 0x0ee:
                    instr_str = "Return";
                    break;
                default:
                    instr_str = "Call RCA at NNN";
                    break;
            }
            break;

        case 0x1000 ... 0x1fff:
            instr_str = "Jump to";
            break;

        case 0x2000 ... 0x2fff:
            instr_str = "Call subroutine";
            break;

        case 0x3000 ... 0x3fff:
            instr_str = "Skip if Vx == NN";
            break;

        case 0x4000 ... 0x4fff:
            instr_str = "Skip if Vx != NN";
            break;

        case 0x5000 ... 0x5ff0:
            instr_str = "Skip if Vx == Vy";
            break;

        case 0x6000 ... 0x6fff:
            instr_str = "Vx = NN";
            break;

        case 0x7000 ... 0x7fff:
            instr_str = "Vx += NN";
            break;

        case 0x8000 ... 0x8ffe:
            switch (instruction & 0x000f) {
                case 0x0:
                    instr_str = "Vx = Vy";
                    break;
                case 0x1:
                    instr_str = "Vx = Vx | Vy (or)";
                    break;
                case 0x2:
                    instr_str = "Vx = Vx & Vy (and)";
                    break;
                case 0x3:
                    instr_str = "Vx = Vx ^ Vy (xor)";
                    break;
                case 0x4:
                    instr_str = "Vx += Vy";
                    break;
                case 0x5:
                    instr_str = "Vx -= Vy";
                    break;
                case 0x6:
                    instr_str = "Vx >>= 1 (rshift)";
                    break;
                case 0x7:
                    instr_str = "Vx = Vy - Vx";
                    break;
                case 0xe:
                    instr_str = "Vx <<= Vy (lshift)";
                    break;
            }
            break;

        case 0x9000 ... 0x9ff0:
            instr_str = "Skip if Vx != Vy";
            break;

        case 0xa000 ... 0xafff:
            instr_str = "I = NNN";
            break;

        case 0xb000 ... 0xbfff:
            instr_str = "Jump to NNN + V0";
            break;


        case 0xc000 ... 0xcfff:
            instr_str = "Vx = rand() & NN";
            break;

        case 0xd000 ... 0xdfff:
            instr_str = "draw(Vx, Vy, N)";
            break;

            // EX9E and EXA1
        case 0xe091 ... 0xefae:
            switch (instruction & 0x00ff) {
                case 0x91:
                    instr_str = "Skip if key() == Vx";
                    break;
                case 0xa1:
                    instr_str = "Skip if key() != Vx";
                    break;
            }
            break;

        case 0xf000 ... 0xff65:
            switch (instruction & 0x00ff) {
                case 0x07:
                    instr_str = "Vx = getDelay()";
                    break;
                case 0x0a:
                    instr_str = "Vx = getKey()";
                    break;
                case 0x15:
                    instr_str = "delayTimer(Vx)";
                    break;
                case 0x18:
                    instr_str = "soundTimer(Vx)";
                    break;
                case 0x1e:
                    instr_str = "I += Vx";
                    break;
                case 0x29:
                    instr_str = "I = sprite_addr[Vx]";
                    break;
                case 0x33:
                    instr_str = "set_BCD(Vx)";
                    break;
                case 0x55:
                    instr_str = "reg_dump(Vx, &I)";
                    break;
                case 0x65:
                    instr_str = "reg_load(Vx, &I)";
                    break;
            }
            break;
    }

    if (instr_str == "") {
        instr_str = "\033[31mUnknown\e[m";
    }

    std::cout << std::hex << std::setw(4) << std::setfill('0') << instruction << " " << instr_str << '\n';
}

int main(int argc, char *args[]) {
    std::cout << "Chip 8 Emulator version " << Chip8_VERSION_MAJOR << "." << Chip8_VERSION_MINOR << "\n\n";
    if (argc < 2) {
        std::cout << "Usage: ./chip8 ROM_Path\n";
        std::exit(EXIT_FAILURE);
    }

    // TODO(aurel): Don't do direct access of the memory but rather hide it behind an interface.
    // TODO(aurel): Initialize memory
    // NOTE(aurel): Chars are 8-bit wide!
    char *memory = new (std::nothrow) char[4096];

    // TODO(aurel): load the ROM into memory
    std::ifstream rom;
    std::streampos size;

    // NOTE(aurel): Open file in read mode (in), binary mode (binary) and set pointer to end of file (ate)
    std::string rom_path = "c8games/";
    rom_path.append(args[1]);

    rom.open(rom_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (rom.is_open()) {
        size = rom.tellg();
        rom.seekg(0, std::ios::beg);
        rom.read(memory, size);
        rom.close();
    }

    for (int i = 0; i < size; i += 2) {

        uint8_t upper = memory[i];
        uint8_t lower = memory[i+1];

        uint16_t instruction = (upper << 8) | lower;

        detectInstruction(instruction);
    }

    // TODO(aurel): emulate the program counter

    // TODO(aurel): fetch the current opcode

    return 0;
}
