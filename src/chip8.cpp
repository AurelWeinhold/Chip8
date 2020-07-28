/**
 * Author: Aurel Weinhold
 * File: src/chip8.cpp
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
    //  ((instruction & 0xF000) >> 12). This makes it more readable and avoids the ranged cases.
    //  12 bits = 3 * 4 Bits. 4 Bits represent one hexadecimal digit.
    switch ((instruction & 0xF000) >> 12) {
        case 0x0:
            switch (instruction & 0x0FFF){
                case 0x0E0:
                    instr_str = "disp_clear()";
                    break;
                case 0x0EE:
                    instr_str = "Return";
                    break;
                default:
                    // TODO(aurel): Change this to not be the default, if possible. I don't want to find the
                    // 0x0000 instruction here probably.
                    instr_str = "Call RCA at NNN";
                    break;
            }
            break;

        case 0x1:
            instr_str = "Jump to";
            break;

        case 0x2:
            instr_str = "Call subroutine";
            break;

        case 0x3:
            instr_str = "Skip if Vx == NN";
            break;

        case 0x4:
            instr_str = "Skip if Vx != NN";
            break;

        case 0x5:
            switch (instruction & 0x000F) {
                case 0:
                    instr_str = "Skip if Vx == Vy";
                    break;
            }
            break;

        case 0x6:
            instr_str = "Vx = NN";
            break;

        case 0x7:
            instr_str = "Vx += NN";
            break;

        case 0x8:
            switch (instruction & 0x000F) {
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
                case 0xE:
                    instr_str = "Vx <<= Vy (lshift)";
                    break;
            }
            break;

        case 0x9:
            instr_str = "Skip if Vx != Vy";
            break;

        case 0xA:
            instr_str = "I = NNN";
            break;

        case 0xB:
            instr_str = "Jump to NNN + V0";
            break;


        case 0xC:
            instr_str = "Vx = rand() & NN";
            break;

        case 0xD:
            instr_str = "draw(Vx, Vy, N)";
            break;

        case 0xE:
            switch (instruction & 0x00FF) {
                case 0x9E:
                    instr_str = "Skip if key() == Vx";
                    break;
                case 0xA1:
                    instr_str = "Skip if key() != Vx";
                    break;
            }
            break;

        case 0xF:
            switch (instruction & 0x00FF) {
                case 0x07:
                    instr_str = "Vx = getDelay()";
                    break;
                case 0x0A:
                    instr_str = "Vx = getKey()";
                    break;
                case 0x15:
                    instr_str = "delayTimer(Vx)";
                    break;
                case 0x18:
                    instr_str = "soundTimer(Vx)";
                    break;
                case 0x1E:
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
        // NOTE(aurel): Print "Unknown" in red
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

    // TODO(aurel): Get rom dynamically during runtime by the user input.
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
