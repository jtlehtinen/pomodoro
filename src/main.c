#pragma warning(push)

#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4267) // 'initializing': conversion from 'size_t' to 'int', possible loss of data

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

#pragma warning(pop)

#include <crtdbg.h>
#include <stdio.h>

int main() {
   #if defined(DEBUG) || defined(_DEBUG)
      const int currentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
      _CrtSetDbgFlag(currentFlags | _CRTDBG_LEAK_CHECK_DF);
   #endif

   const int width = 360;
   const int height = 450;
   const char* title = "Pomodoro";

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
   InitWindow(width, height, title);
   InitAudioDevice();
   //SetWindowIcon(LoadImage(/* something crap here */));
   SetTargetFPS(30);
   //SetExitKey(/* do I need this?  */);

   while (!WindowShouldClose()) {
      const Color bgColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

      BeginDrawing();
      ClearBackground(bgColor);
      EndDrawing();
   }

   CloseAudioDevice();
   CloseWindow();

   return 0;
}
