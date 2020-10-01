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

      case TextID_Timer: return "Timer";
      case TextID_Options: return "Options";
      case TextID_Styles: return "Styles";
      case TextID_About: return "About";

      case TextID_Rounds: return "Rounds";
      case TextID_ResetTimerConfig: return "Reset Defaults";

      case TextID_Pomodoro: return "Pomodoro";
      case TextID_AboutDescription: return "Pomodoro technique simplified";

      case TextID_StyleAshes: return "Ashes";
      case TextID_StyleBluish: return "Bluish";
      case TextID_StyleCandy: return "Candy";
      case TextID_StyleCherry: return "Cherry";
      case TextID_StyleCyber: return "Cyber";
      case TextID_StyleDefault: return "Default";
      case TextID_StyleJungle: return "Jungle";
      case TextID_StyleLavanda: return "Lavanda";
      case TextID_StyleTerminal: return "Terminal";

      case TextID_OptionAlwaysOnTop: return "Always On Top";
      case TextID_OptionAutoStartWorkTimer: return "Auto-start Work Timer";
      case TextID_OptionAutoStartBreakTimer: return "Auto-start Break Timer";
      case TextID_OptionTickSounds: return "Tick Sounds";
      case TextID_OptionDesktopNotifications: return "Desktop Notifications";
      case TextID_OptionMinimizeToTray: return "Minimize to Tray";
      case TextID_OptionMinimizeToTrayOnClose: return "Minimize to Tray on Close";

      default: return "TODO TEXT";
   }
}

const char* GetStyleFile(Style style) {
   switch (style) {
      case Style_Ashes: return "../../res/styles/ashes/ashes.rgs";
      case Style_Bluish: return "../../res/styles/bluish/bluish.rgs";
      case Style_Candy: return "../../res/styles/candy/candy.rgs";
      case Style_Cherry: return "../../res/styles/cherry/cherry.rgs";
      case Style_Cyber: return "../../res/styles/cyber/cyber.rgs";
      case Style_Default: return "../../res/styles/default/default.rgs";
      case Style_Jungle: return "../../res/styles/jungle/jungle.rgs";
      case Style_Lavanda: return "../../res/styles/lavanda/lavanda.rgs";
      case Style_Terminal: return "../../res/styles/terminal/terminal.rgs";

      default: return NULL;
   }
}

TimerConfig MakeDefaultTimerConfig() {
   TimerConfig result = {0};
   result.focus = 25.0f;
   result.shortBreak = 5.0f;
   result.longBreak = 15.0f;
   result.rounds = 4.0f;
   return result;
}

Options MakeDefaultOptions() {
   Options result = {0};
   result.alwaysOnTop = false;
   result.autoStartWorkTimer = true;
   result.autoStartBreakTimer = true;
   result.tickSounds = true;
   result.desktopNotifications = true;
   result.minimizeToTray = false;
   result.minimizeToTrayOnClose = false;
   return result;
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

float DrawTimerConfigTimeSlider(const char* label, float* value, float midx, float midy, float minValue, float maxValue, TimerConfigSliderFlag flag) {
   const float x = midx - midx / 2.0f;
   float y = midy;

   const float w = midx;
   const float h = 20.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   GuiLabel(MakeRectangle(x, y, w, h), label);
   y += h;

   const int rounded = RoundToInt(*value);
   const char* valueLabel = (flag == TimerConfigSliderFlag_Time ? EasyPrint("%d:00", rounded) : EasyPrint("%d", rounded));
   GuiLabel(MakeRectangle(x, y, w, h), valueLabel);
   y += h;

   *value = GuiSlider(MakeRectangle(x, y, w, h), NULL, NULL, *value, minValue, maxValue);
   y += h * 2.0f;

   return y;
}

void DrawTimerConfigPage(Fonts* fonts, TimerConfig* config) {
   TimerConfig oldConfig = *config;

   const int sw = GetScreenWidth();
   const int sh = GetScreenHeight();

   const float midx = sw / 2.0f;
   const float midy = sh / 2.0f;

   float x = midx - midx / 2.0f;
   float y = sh / 24.0f;

   const float w = midx;
   const float h = 20.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   GuiSetFont(fonts->bigFont);
   GuiLabel(MakeRectangle(x, y, w, h), GetTextByID(TextID_Timer));
   GuiSetFont(fonts->font);

   y += sh / 16.0f;

   y = DrawTimerConfigTimeSlider(GetTextByID(TextID_Focus), &config->focus, midx, y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   y = DrawTimerConfigTimeSlider(GetTextByID(TextID_ShortBreak), &config->shortBreak, midx, y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   y = DrawTimerConfigTimeSlider(GetTextByID(TextID_LongBreak), &config->longBreak, midx, y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   y = DrawTimerConfigTimeSlider(GetTextByID(TextID_Rounds), &config->rounds, midx, y, 1.0f, 12.0f, TimerConfigSliderFlag_Int);

   y += sh / 24.0f;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_ResetTimerConfig))) {
      *config = MakeDefaultTimerConfig();
   }

   config->changed = (oldConfig.focus != config->focus ||
                      oldConfig.shortBreak != config->shortBreak ||
                      oldConfig.longBreak != config->longBreak ||
                      oldConfig.rounds != config->rounds);
}

void DrawOptionsPage(Fonts* fonts, Options* options) {
   const int sw = GetScreenWidth();
   const int sh = GetScreenHeight();

   const float midx = sw / 2.0f;
   const float midy = sh / 2.0f;

   float x = midx - midx / 2.0f;
   float y = sh / 24.0f;

   float w = midx;
   const float h = 20.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   GuiSetFont(fonts->bigFont);
   GuiLabel(MakeRectangle(x, y, w, h), GetTextByID(TextID_Options));
   GuiSetFont(fonts->font);
   y += sh / 8.0f;

   w = 20.0f;
   x = midx - midx * 3.0f / 4.0f;
   float dy = sh / 16.0f;

   options->alwaysOnTop = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionAlwaysOnTop), options->alwaysOnTop);
   y += dy;

   options->autoStartWorkTimer = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionAutoStartWorkTimer), options->autoStartWorkTimer);
   y += dy;

   options->autoStartBreakTimer = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionAutoStartBreakTimer), options->autoStartBreakTimer);
   y += dy;

   options->tickSounds = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionTickSounds), options->tickSounds);
   y += dy;

   options->desktopNotifications = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionDesktopNotifications), options->desktopNotifications);
   y += dy;

   options->minimizeToTray = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionMinimizeToTray), options->minimizeToTray);
   y += dy;

   options->minimizeToTrayOnClose = GuiCheckBox(MakeRectangle(x, y, w, h), GetTextByID(TextID_OptionMinimizeToTrayOnClose), options->minimizeToTrayOnClose);
   y += dy;
}

Style DrawStylesPage(Fonts* fonts, Style style) {
   const int sw = GetScreenWidth();
   const int sh = GetScreenHeight();

   const float midx = sw / 2.0f;
   const float midy = sh / 2.0f;

   float x = midx - midx / 2.0f;
   float y = sh / 24.0f;

   const float w = midx;
   const float h = 20.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   GuiSetFont(fonts->bigFont);
   GuiLabel(MakeRectangle(midx - midx / 2.0f, y, midx, 20.0f), GetTextByID(TextID_Styles));
   GuiSetFont(fonts->font);
   y += sh / 8.0f;

   float dy = sh / 16.0f;

   Style newStyle = style;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleAshes))) newStyle = Style_Ashes;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleBluish))) newStyle = Style_Bluish;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleCandy))) newStyle = Style_Candy;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleCherry))) newStyle = Style_Cherry;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleCyber))) newStyle = Style_Cyber;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleDefault))) newStyle = Style_Default;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleJungle))) newStyle = Style_Jungle;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleLavanda))) newStyle = Style_Lavanda;
   y += dy;

   if (GuiLabelButton(MakeRectangle(x, y, w, h), GetTextByID(TextID_StyleTerminal))) newStyle = Style_Terminal;
   y += dy;

   return newStyle;
}

void DrawAboutPage(Fonts* fonts, Color color) {
   const int sw = GetScreenWidth();
   const int sh = GetScreenHeight();
   const int ystep = sh / 8;
   int y = sh / 24;

   GuiSetFont(fonts->bigFont);
   GuiDrawText(GetTextByID(TextID_About), MakeRectangle(0, y, sw, 20), GUI_TEXT_ALIGN_CENTER, color);
   y += ystep;

   GuiSetFont(fonts->font);
   GuiDrawText(GetTextByID(TextID_Pomodoro), MakeRectangle(0, y, sw, 20), GUI_TEXT_ALIGN_CENTER, color);
   y += ystep;

   GuiDrawText(GetTextByID(TextID_AboutDescription), MakeRectangle(0, y, sw, 20), GUI_TEXT_ALIGN_CENTER, color);
}

SettingsPage DrawSettingsViewPageTabs(SettingsPage currentPage) {
   const int sw = GetScreenWidth();
   const int sh = GetScreenHeight();

   const int iconWidth = sw / 4;
   const int iconHeight = 30;

   int x = 0;
   const int y = sh - iconHeight;

   if (GuiButton(MakeRectangle(x, y, iconWidth, iconHeight), GuiIconText(RICON_CLOCK, NULL))) {
      currentPage = SettingsPage_TimerConfig;
   }

   x += iconWidth;

   if (GuiButton(MakeRectangle(x, y, iconWidth, iconHeight), GuiIconText(RICON_TOOLS, NULL))) {
      currentPage = SettingsPage_Options;
   }

   x += iconWidth;

   if (GuiButton(MakeRectangle(x, y, iconWidth, iconHeight), GuiIconText(RICON_COLOR_BUCKET, NULL))) {
      currentPage = SettingsPage_Styles;
   }

   x += iconWidth;

   if (GuiButton(MakeRectangle(x, y, iconWidth, iconHeight), GuiIconText(RICON_INFO, NULL))) {
      currentPage = SettingsPage_About;
   }

   return currentPage;
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

   Fonts fonts;
   fonts.font = LoadFontEx("../../res/nokiafc22.ttf", 18, 0, 0);
   fonts.bigFont = LoadFontEx("../../res/nokiafc22.ttf", 32, 0, 0);
   fonts.extraBigFont = LoadFontEx("../../res/nokiafc22.ttf", 64, 0, 0);

   Timer timer = MakeTimer(TimerType_Focus);
   View currentView = View_Main;
   SettingsPage currentSettingsPage = SettingsPage_TimerConfig;
   Style currentStyle = Style_Ashes;
   bool styleChanged = false;

   Options options = {0};
   TimerConfig config = MakeDefaultTimerConfig();

   while (!WindowShouldClose()) {
      if (styleChanged) {
         styleChanged = false;
         
         const char* file = GetStyleFile(currentStyle);
         if (file) {
            GuiLoadStyle(file);
         }
      }

      AdvanceTimer(&timer, GetFrameTime());
      
      const Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
      const Color bgColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));

      BeginDrawing();
      ClearBackground(bgColor);

      if (currentView == View_Main) {

         DrawTimerRings(&timer);
         DrawTimerText(&timer, &fonts.extraBigFont, &fonts.bigFont, textColor);

      } else if (currentView == View_Settings) {

         if (currentSettingsPage == SettingsPage_TimerConfig) {
            DrawTimerConfigPage(&fonts, &config);
         } else if (currentSettingsPage == SettingsPage_Options) {
            DrawOptionsPage(&fonts, &options);
         } else if (currentSettingsPage == SettingsPage_Styles) {
            Style newStyle = DrawStylesPage(&fonts, currentStyle);
            styleChanged = (currentStyle != newStyle);
            currentStyle = newStyle;
         } else if (currentSettingsPage == SettingsPage_About) {
            DrawAboutPage(&fonts, textColor);
         }

         currentSettingsPage = DrawSettingsViewPageTabs(currentSettingsPage);
       }
      
      currentView = DrawChangeViewButton(currentView);

      EndDrawing();
   }

   UnloadFont(fonts.extraBigFont);
   UnloadFont(fonts.bigFont);
   UnloadFont(fonts.font);

   CloseAudioDevice();
   CloseWindow();

   // @TODO: Fix leak in style loading.

   return 0;
}
