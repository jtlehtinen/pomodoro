#pragma warning(push)

#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4267) // 'initializing': conversion from 'size_t' to 'int', possible loss of data

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"

#pragma warning(pop)

#include "types.h"
#include <assert.h>
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

int Max(int a, int b) { return b < a ? a : b; }
float Maxf(float a, float b) { return b < a ? a : b; }

int Min(int a, int b) { return a < b ? a : b; }
float Minf(float a, float b) { return a < b ? a : b; }

int RoundToInt(float value) {
   return (int)roundf(value);
}

const char* GetTextByID(TextID id) {
   switch (id) {
      case TextID_Focus: return "Focus";
      case TextID_LongBreak: return "Long Break";
      case TextID_ShortBreak: return "Short Break";

      default: return "TODO TEXT";
   }
}

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

Duration GetDurationToGo(Timer* timer) {
   int left = RoundToInt(timer->duration - timer->consumed);
   if (left < 0) {
      left = 0;
   }

   const int seconds = left % 60;
   const int minutes = left / 60;

   Duration result;
   result.minutes = minutes;
   result.seconds = seconds;

   return result;
}

Vector2 MakeVector2(float x, float y) {
   Vector2 result = {x, y};
   return result;
}

Rectangle MakeRectangle(int x, int y, int width, int height) {
   Rectangle result = {(float)x, (float)y, (float)width, (float)height};
   return result;
}

void DrawTimerRings(Timer* timer) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();

   const float minDimension = Minf(sw, sh);
   const float inner = minDimension * 0.8f * 0.5f;
   const float outer = minDimension * 0.9f * 0.5f;

   const int angle = RoundToInt((timer->duration - timer->consumed) / timer->duration * 360.0f);

   const float diff = outer - inner;
   DrawRing(MakeVector2(sw * 0.5f, sh * 0.5f), inner + diff / 3.0f, outer - diff / 3.0f, 0, 360, 360, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));
   DrawRing(MakeVector2(sw * 0.5f, sh * 0.5f), inner, outer, 180, 180 + angle, angle + 1, RED);
}

void DrawTimerText(Timer* timer, Font* timeFont, Font* timerTypeFont, Color color) {
   const Duration duration = GetDurationToGo(timer);

   GuiSetFont(*timeFont);
   const char* timeText = EasyPrint("%d:%02d", duration.minutes, duration.seconds);
   GuiDrawText(timeText, MakeRectangle(0, 0, GetScreenWidth(), GetScreenHeight()), GUI_TEXT_ALIGN_CENTER, color);

   const char* timerTypeText = NULL;

   if (timer->type == TimerType_Focus) {
      timerTypeText = GetTextByID(TextID_Focus);
   } else if (timer->type == TimerType_LongBreak) {
      timerTypeText = GetTextByID(TextID_LongBreak);
   } else if (timer->type == TimerType_ShortBreak) {
      timerTypeText = GetTextByID(TextID_ShortBreak);
   }
   assert(timerTypeText);

   GuiSetFont(*timerTypeFont);
   GuiDrawText(timerTypeText, MakeRectangle(0, GetScreenHeight() / 10, GetScreenWidth(), GetScreenHeight()), GUI_TEXT_ALIGN_CENTER, color);
}

View DrawChangeViewButton(View view) {
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);

   const guiIconName icon = (view == View_Main ? RICON_GEAR : RICON_ARROW_LEFT);
   if (GuiLabelButton(MakeRectangle(5, 5, 20, 20), GuiIconText(icon, NULL))) {
      view = (view == View_Main ? View_Settings : View_Main);
   }

   return view;
}

int main() {
   #if defined(DEBUG) || defined(_DEBUG)
      const int currentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
      _CrtSetDbgFlag(currentFlags | _CRTDBG_LEAK_CHECK_DF);
   #endif

   const int width = 360;
   const int height = 450;
   const char* title = "Pomodoro";

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
   InitWindow(width, height, title);
   InitAudioDevice();
   //SetWindowIcon(LoadImage(/* something crap here */));
   SetTargetFPS(30);
   //SetExitKey(/* do I need this?  */);

   Font font = LoadFontEx("../../res/nokiafc22.ttf", 18, 0, 0);
   Font bigFont = LoadFontEx("../../res/nokiafc22.ttf", 32, 0, 0);
   Font extraBigFont = LoadFontEx("../../res/nokiafc22.ttf", 64, 0, 0);

   Timer timer = MakeTimer(TimerType_Focus);
   View currentView = View_Main;

   while (!WindowShouldClose()) {
      AdvanceTimer(&timer, GetFrameTime());
      
      const Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
      const Color bgColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

      BeginDrawing();
      ClearBackground(bgColor);
      DrawTimerRings(&timer);
      DrawTimerText(&timer, &extraBigFont, &bigFont, textColor);
      currentView = DrawChangeViewButton(currentView);

      EndDrawing();
   }

   UnloadFont(extraBigFont);
   UnloadFont(bigFont);
   UnloadFont(font);

   CloseAudioDevice();
   CloseWindow();

   return 0;
}
