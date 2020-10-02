#if !defined(DEBUG) && !defined(_DEBUG)
   // Hide console in release build.
   #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "pomodoro.h"
#include <crtdbg.h>

/*
@TODO:

- Fix leak in style loading. raygui does not provide a way to unload a style.
- Handle error cases
- Custom themes (load from a folder)
- Blocking based message polling + a timer, or reduce frame rate if no focus
- Adjust gui when resized
- Implement options
- Save and load settings
- Resize raygui icons
- Tight bounding boxes for gui controls
- Handle 'reset' properly when timer config changes while the timer is running
- Copy strings in PomodoroAddStyle
- Strings to files
- Find res folder

*/

int main() {
   #if defined(DEBUG) || defined(_DEBUG)
      const int currentFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
      _CrtSetDbgFlag(currentFlags | _CRTDBG_LEAK_CHECK_DF);
   #endif

   const int width = 280;
   const int height = 350;
   const char* title = "Pomodoro";

   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
   InitWindow(width, height, title);
   InitAudioDevice();

   Pomodoro pomodoro;
   PomodoroInit(&pomodoro);

   PomodoroAddStyle(&pomodoro, "Ashes", "res/styles/ashes/ashes.rgs");
   PomodoroAddStyle(&pomodoro, "Bluish", "res/styles/bluish/bluish.rgs");
   PomodoroAddStyle(&pomodoro, "Candy", "res/styles/candy/candy.rgs");
   PomodoroAddStyle(&pomodoro, "Cherry", "res/styles/cherry/cherry.rgs");
   PomodoroAddStyle(&pomodoro, "Cyber", "res/styles/cyber/cyber.rgs");
   PomodoroAddStyle(&pomodoro, "Default", "res/styles/default/default.rgs");
   PomodoroAddStyle(&pomodoro, "Jungle", "res/styles/jungle/jungle.rgs");
   PomodoroAddStyle(&pomodoro, "Lavanda", "res/styles/lavanda/lavanda.rgs");
   PomodoroAddStyle(&pomodoro, "Terminal", "res/styles/terminal/terminal.rgs");

   int fps = IsWindowFocused() ? 30 : 3;

   while (!WindowShouldClose()) {
      // @TODO: Get rid of this and use something more proper.
      const int newFPS = IsWindowFocused() ? 30 : 3;
      if (fps != newFPS) {
         fps = newFPS;
         SetTargetFPS(fps);
      }

      PomodoroUpdate(&pomodoro);

      BeginDrawing();
      if (!IsWindowMinimized()) {
         PomodoroDraw(&pomodoro);
      }
      EndDrawing();
   }

   PomodoroCleanup(&pomodoro);

   CloseAudioDevice();
   CloseWindow();

   return 0;
}
