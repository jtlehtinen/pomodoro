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

const char* EasyPrint(const char* format, ...) {
   // @TODO: This is temporary.
   static char buf[1024] = {0};

   va_list args;
   va_start(args, format);
   vsprintf_s(buf, sizeof(buf), format, args);
   va_end(args);

   return buf;
}

typedef enum {
   TimerType_Focus,
   TimerType_ShortBreak,
   TimerType_LongBreak,
} TimerType;

typedef struct {
   TimerType type;
   float duration;
   float consumed;
   bool paused;
} Timer;

Timer MakeTimer(TimerType type) {
   const float secondsPerMinute = 60.0f;

   Timer result = {0};
   result.type = type;
   result.duration = 25.0f * secondsPerMinute;
   result.paused = false;

   return result;
}

void AdvanceTimer(Timer* timer, float seconds) {
   if (!timer->paused) {
      timer->consumed += seconds;
   }
}


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

   Timer timer = MakeTimer(TimerType_Focus);

   while (!WindowShouldClose()) {
      AdvanceTimer(&timer, GetFrameTime());

      const Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
      const Color bgColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

      BeginDrawing();
      ClearBackground(bgColor);
      DrawText(EasyPrint("Consumed: %.1f s", timer.consumed), 20, 20, 16, textColor);
      EndDrawing();
   }

   CloseAudioDevice();
   CloseWindow();

   return 0;
}
