#pragma warning(push)

#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4267) // 'initializing': conversion from 'size_t' to 'int', possible loss of data

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

#pragma warning(pop)

#include <stdio.h>

int main() {
   printf("Hello\n");
   return 0;
}
