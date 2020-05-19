/**
 * Author: Aurel Weinhold
 * File: src/chip8.c
 * Created: 10:18 19.05.2020
 * License: MIT
 *
 * Information is fetched from the [chip-8 Wikipedia article](https://en.wikipedia.org/wiki/CHIP-8)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Chip8Config.h"

typedef struct chip8 {
    uint16_t *memory;
} chip8;


int main(int argc, char *args[]) {

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




    // TODO(aurel): load the ROM into memory
    // TODO(aurel): emulate the program counter
    // TODO(aurel): fetch the current opcode

    printf("Hello world! Version %d.%d\n", Chip8_VERSION_MAJOR, Chip8_VERSION_MINOR);
    return 0;
}
