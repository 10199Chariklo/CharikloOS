/* Standard C11 Libraries */
#include <stdbool.h>
#include <string.h>

/* Chariklo Subsystem */
#include "CKpalette_int.h" // Local module header

// The hidden truecolor master interface registry instance definition
CKpaletteInterface CKpalette = {
    .header = { .isA = false, .isMono = false, .isBlended = false },
    .white      = { .keyName = "white",     .colorID = 0x0001,    .blitChar = '0', .hexColor = "#F0F0F0" },
    .orange     = { .keyName = "orange",    .colorID = 0x0002,    .blitChar = '1', .hexColor = "#F2B233" },
    .magenta    = { .keyName = "magenta",   .colorID = 0x0004,    .blitChar = '2', .hexColor = "#E57FD8" },
    .lightBlue  = { .keyName = "lightBlue", .colorID = 0x0008,    .blitChar = '3', .hexColor = "#99B2F2" },
    .yellow     = { .keyName = "yellow",    .colorID = 0x0010,    .blitChar = '4', .hexColor = "#DEDE6C" },
    .lime       = { .keyName = "lime",      .colorID = 0x0020,    .blitChar = '5', .hexColor = "#7FCC19" },
    .pink       = { .keyName = "pink",      .colorID = 0x0040,    .blitChar = '6', .hexColor = "#F2B2CC" },
    .grey       = { .keyName = "grey",      .colorID = 0x0080,    .blitChar = '7', .hexColor = "#4C4C4C" },
    .lightGrey  = { .keyName = "lightGrey", .colorID = 0x0100,    .blitChar = '8', .hexColor = "#999999" },
    .cyan       = { .keyName = "cyan",      .colorID = 0x0200,    .blitChar = '9', .hexColor = "#4C99B2" },
    .purple     = { .keyName = "purple",    .colorID = 0x0400,    .blitChar = 'a', .hexColor = "#B266E5" },
    .blue       = { .keyName = "blue",      .colorID = 0x0800,    .blitChar = 'b', .hexColor = "#3366CC" },
    .brown      = { .keyName = "brown",     .colorID = 0x1000,    .blitChar = 'c', .hexColor = "#7F664C" },
    .green      = { .keyName = "green",     .colorID = 0x2000,    .blitChar = 'd', .hexColor = "#57A64E" },
    .red        = { .keyName = "red",       .colorID = 0x4000,    .blitChar = 'e', .hexColor = "#CC4C4C" },
    .black      = { .keyName = "black",     .colorID = 0x8000,    .blitChar = 'f', .hexColor = "#111111" }
};

unsigned int CKcolors_ExecuteCombineRuntime(int argCount, const char* args[]) {
    unsigned int combinedMask = 0;
    for (int i = 0; i < argCount; i++) {
        for (int j = 0; j < 16; j++) {
            if (strcmp(CKpalette.data[j].keyName, args[i]) == 0) {
                combinedMask |= CKpalette.data[j].colorID;
                break;
            }
        }
    }
    return combinedMask;
}