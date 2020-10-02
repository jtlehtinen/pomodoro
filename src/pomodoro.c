#define RAYGUI_IMPLEMENTATION
#define RICONS_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "pomodoro.h"
#include "raymath.h"
#include "stb/stretchy_buffer.h"
#include <assert.h>
#include <math.h>

static const float SecondsPerMinute = 60.0f;

static int Max(int a, int b) { return b < a ? a : b; }
static int Min(int a, int b) { return a < b ? a : b; }

static float Maxf(float a, float b) { return b < a ? a : b; }
static float Minf(float a, float b) { return a < b ? a : b; }

static int RoundToInt(float value) { return (int)roundf(value); }

static const char* EasyPrint(const char* format, ...) {
   // @TODO: This is temporary.
   static char buf[1024] = {0};

   va_list args;
   va_start(args, format);
   vsprintf_s(buf, sizeof(buf), format, args);
   va_end(args);

   return buf;
}

static Vector2 MakeVector2(float x, float y) {
   Vector2 result = {x, y};
   return result;
}

static Rectangle MakeRectangle(float x, float y, float width, float height) {
   Rectangle result = {x, y, width, height};
   return result;
}

static Style MakeStyle(const char* name, const char* fileName) {
   Style result = {0};
   result.name = name;
   result.fileName = fileName;
   return result;
}

static Options PomodoroMakeDefaultOptions() {
   Options result = {0};
   result.alwaysOnTop = false;
   result.autoStartWorkTimer = true;
   result.autoStartBreakTimer = true;
   result.tickSounds = true;
   return result;
}

static TimerConfig PomodoroMakeDefaultTimerConfig() {
   TimerConfig result = {0};
   result.focus = 25.0f;
   result.shortBreak = 5.0f;
   result.longBreak = 15.0f;
   result.rounds = 4.0f;
   return result;
}

static Timer PomodoroMakeFocusTimer(const TimerConfig* config) {
   Timer result = {0};
   result.type = TimerType_Focus;
   result.duration = roundf(config->focus) * SecondsPerMinute;
   result.paused = false;
   return result;
}

static Timer PomodoroMakeShortBreakTimer(const TimerConfig* config) {
   Timer result = {0};
   result.type = TimerType_ShortBreak;
   result.duration = roundf(config->shortBreak) * SecondsPerMinute;
   result.paused = false;
   return result;
}

static Timer PomodoroMakeLongBreakTimer(const TimerConfig* config) {
   Timer result = {0};
   result.type = TimerType_LongBreak;
   result.duration = roundf(config->longBreak) * SecondsPerMinute;
   result.paused = false;
   return result;
}

static Timer PomodoroMakeTimerByType(const TimerConfig* config, TimerType type) {
   switch (type) {
      case TimerType_Focus: return PomodoroMakeFocusTimer(config);
      case TimerType_ShortBreak: return PomodoroMakeShortBreakTimer(config);
      case TimerType_LongBreak: return PomodoroMakeLongBreakTimer(config);

      default:
         assert(!"Timer type missing");
         return PomodoroMakeFocusTimer(config);
   }
}

static Sound* GetSoundForCurrentTimer(Pomodoro* pomodoro) {
   if (pomodoro->timer.type == TimerType_ShortBreak) {
      return &pomodoro->sounds.shortBreak;
   }

   if (pomodoro->timer.type == TimerType_LongBreak) {
      return &pomodoro->sounds.longBreak;
   }

   return &pomodoro->sounds.work;
}

static bool IsTimerFinished(const Timer* timer) {
   return timer->consumed >= timer->duration;
}

static const char* GetTextByID(TextID id) {
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
      case TextID_AboutDescription: return "Todo description";

      case TextID_OptionAlwaysOnTop: return "Always On Top";
      case TextID_OptionAutoStartWorkTimer: return "Auto-start Work Timer";
      case TextID_OptionAutoStartBreakTimer: return "Auto-start Break Timer";
      case TextID_OptionTickSounds: return "Tick Sounds";

      case TextID_Reset: return "Reset";

      default: return "TODO TEXT";
   }
}

void PomodoroSetFlag(Pomodoro* pomodoro, int flag) {
   pomodoro->flags |= flag;
}

void PomodoroUnsetFlag(Pomodoro* pomodoro, int flag) {
   pomodoro->flags &= ~flag;
}

bool PomodoroIsFlagSet(Pomodoro* pomodoro, int flag) {
   return (pomodoro->flags & flag) != 0;
}

void PomodoroInit(Pomodoro* pomodoro) {
   Image icon = LoadImage("../../res/tomato.png");
   SetWindowIcon(icon);
   UnloadImage(icon);

   Pomodoro zero = {0};
   *pomodoro = zero;

   pomodoro->fonts.font = LoadFontEx("../../res/nokiafc22.ttf", 18, 0, 0);
   pomodoro->fonts.bigFont = LoadFontEx("../../res/nokiafc22.ttf", 32, 0, 0);
   pomodoro->fonts.extraBigFont = LoadFontEx("../../res/nokiafc22.ttf", 64, 0, 0);

   pomodoro->sounds.tick = LoadSound("../../res/sounds/tick.mp3");
   pomodoro->sounds.longBreak = LoadSound("../../res/sounds/alert-long-break.mp3");
   pomodoro->sounds.shortBreak = LoadSound("../../res/sounds/alert-short-break.mp3");
   pomodoro->sounds.work = LoadSound("../../res/sounds/alert-work.mp3");

   pomodoro->options = PomodoroMakeDefaultOptions();
   pomodoro->config = PomodoroMakeDefaultTimerConfig();
   pomodoro->timer = PomodoroMakeFocusTimer(&pomodoro->config);
   pomodoro->round = 1;
   pomodoro->timer.paused = true;

   pomodoro->currentView = View_Main;
   pomodoro->currentSettingsPage = SettingsPage_TimerConfig;
}

void PomodoroCleanup(Pomodoro* pomodoro) {
   sb_free(pomodoro->styles);

   UnloadSound(pomodoro->sounds.work);
   UnloadSound(pomodoro->sounds.shortBreak);
   UnloadSound(pomodoro->sounds.longBreak);
   UnloadSound(pomodoro->sounds.tick);

   UnloadFont(pomodoro->fonts.extraBigFont);
   UnloadFont(pomodoro->fonts.bigFont);
   UnloadFont(pomodoro->fonts.font);

   Pomodoro zero = {0};
   *pomodoro = zero;
}

void PomodoroAddStyle(Pomodoro* pomodoro, const char* name, const char* fileName) {
   sb_push(pomodoro->styles, MakeStyle(name, fileName));

   if (sb_count(pomodoro->styles) == 1) {
      pomodoro->currentStyleIndex = 0;
      GuiLoadStyle(pomodoro->styles[pomodoro->currentStyleIndex].fileName);
   }
}

void PomodoroPlaySound(Pomodoro* pomodoro, Sound* sound) {
   if (!pomodoro->muted && sound) {
      PlaySound(*sound);
   }
}

static void PomodoroCheckStyle(Pomodoro* pomodoro) {
   if (PomodoroIsFlagSet(pomodoro, PomodoroFlag_UpdateStyle)) {
      PomodoroUnsetFlag(pomodoro, PomodoroFlag_UpdateStyle);
      GuiLoadStyle(pomodoro->styles[pomodoro->currentStyleIndex].fileName);
   }
}

static void PomodoroCheckConfigChange(Pomodoro* pomodoro) {
   if (PomodoroIsFlagSet(pomodoro, PomodoroFlag_ConfigChanged)) {
      PomodoroUnsetFlag(pomodoro, PomodoroFlag_ConfigChanged);
      pomodoro->timer = PomodoroMakeTimerByType(&pomodoro->config, pomodoro->timer.type);
   }
}

static void PomodoroMoveToNextTimer(Pomodoro* pomodoro) {
   const bool lastRound = (pomodoro->round >= (int)pomodoro->config.rounds);
   const bool nextIsBreak = (pomodoro->timer.type == TimerType_Focus);

   const TimerType nextTimerType = (nextIsBreak ? (lastRound ? TimerType_LongBreak : TimerType_ShortBreak) : TimerType_Focus);

   if (nextTimerType == TimerType_Focus) {
      pomodoro->round = (lastRound ? 1 : pomodoro->round + 1);
      pomodoro->timer = PomodoroMakeFocusTimer(&pomodoro->config);
      pomodoro->timer.paused = !pomodoro->options.autoStartWorkTimer;
   }

   if (nextTimerType == TimerType_ShortBreak) {
      pomodoro->timer = PomodoroMakeShortBreakTimer(&pomodoro->config);
      pomodoro->timer.paused = !pomodoro->options.autoStartBreakTimer;
   }

   if (nextTimerType == TimerType_LongBreak) {
      pomodoro->timer = PomodoroMakeLongBreakTimer(&pomodoro->config);
      pomodoro->timer.paused = !pomodoro->options.autoStartBreakTimer;
   }

   PomodoroPlaySound(pomodoro, GetSoundForCurrentTimer(pomodoro));
}

static void PomodoroCheckTimer(Pomodoro* pomodoro) {
   if (IsTimerFinished(&pomodoro->timer)) {
      PomodoroMoveToNextTimer(pomodoro);
   }
}

static void PomodoroAdvanceTimer(Pomodoro* pomodoro, float seconds) {
   const int before = RoundToInt(pomodoro->timer.consumed);

   if (!pomodoro->timer.paused) {
      pomodoro->timer.consumed += seconds;
   }

   const int after = RoundToInt(pomodoro->timer.consumed);

   if (pomodoro->options.tickSounds && before != after) {
      PomodoroPlaySound(pomodoro, &pomodoro->sounds.tick);
   }
}

void PomodoroUpdate(Pomodoro* pomodoro) {
   PomodoroCheckStyle(pomodoro);
   PomodoroCheckConfigChange(pomodoro);
   PomodoroAdvanceTimer(pomodoro, GetFrameTime());
   PomodoroCheckTimer(pomodoro);
}

static void DrawTimerRings(Pomodoro* pomodoro) {
   const Timer* timer = &pomodoro->timer;

   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();

   const float minDimension = Minf(sw, sh);
   const float innerRadius = minDimension * 0.8f * 0.5f;
   const float outerRadius = minDimension * 0.9f * 0.5f;
   const float diffRadius = outerRadius - innerRadius;

   const int angle = RoundToInt((timer->duration - timer->consumed) / timer->duration * 360.0f);

   const Vector2 center = {sw * 0.5f, sh * 0.5f};
   DrawRing(center, innerRadius + diffRadius / 3.0f, outerRadius - diffRadius / 3.0f, 0, 360, 360, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));

   const Color color = (pomodoro->timer.type == TimerType_Focus) ? RED : (pomodoro->timer.type == TimerType_ShortBreak) ? GREEN : SKYBLUE;
   DrawRing(center, innerRadius, outerRadius, 180, 180 + angle, angle + 1, color);
}

static const char* GetTimerTypeText(TimerType type) {
   switch (type) {
      case TimerType_Focus: return GetTextByID(TextID_Focus);
      case TimerType_LongBreak: return GetTextByID(TextID_LongBreak);
      case TimerType_ShortBreak: return GetTextByID(TextID_ShortBreak);
      default: assert(false); return "Todo";
   }
}

static void DrawTimerText(Pomodoro* pomodoro) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();

   int timeLeft = RoundToInt(pomodoro->timer.duration - pomodoro->timer.consumed);
   if (timeLeft < 0) {
      timeLeft = 0;
   }

   const int seconds = timeLeft % 60;
   const int minutes = timeLeft / 60;

   GuiSetFont(pomodoro->fonts.extraBigFont);
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   const char* timeText = EasyPrint("%d:%02d", minutes, seconds);
   GuiLabel(MakeRectangle(0.0f, sh / 2.0f, sw, 0.0f), timeText);

   GuiSetFont(pomodoro->fonts.font);
   GuiLabel(MakeRectangle(0, sh / 2.0f + sh / 10.0f, sw, 0.0f), GetTimerTypeText(pomodoro->timer.type));
}

static void DrawMainViewControlButtons(Pomodoro* pomodoro) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();

   GuiSetFont(pomodoro->fonts.font);
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);

   const float dim = 30.0f;
   Rectangle bounds = {sw - dim, sh - dim, dim, dim};

   if (GuiLabelButton(bounds, GuiIconText(RICON_AUDIO, NULL))) {
      pomodoro->muted = !pomodoro->muted;
   }

   if (pomodoro->muted) {
      GuiLabel(bounds, GuiIconText(RICON_CROSSLINE, NULL));
   }

   bounds.y -= dim;

   if (GuiLabelButton(bounds, GuiIconText(RICON_PLAYER_NEXT, NULL))) {
      PomodoroMoveToNextTimer(pomodoro);
   }

   bounds.y -= dim;

   const int iconId = (pomodoro->timer.paused ? RICON_PLAYER_PLAY : RICON_PLAYER_PAUSE);
   if (GuiLabelButton(bounds, GuiIconText(iconId, NULL))) {
      pomodoro->timer.paused = !pomodoro->timer.paused;
   }

   bounds.y -= dim;

   if (GuiLabelButton(MakeRectangle(5.0f, sh - 25.0f, 50.0f, 20.0f), GetTextByID(TextID_Reset))) {
      pomodoro->timer = PomodoroMakeTimerByType(&pomodoro->config, pomodoro->timer.type);
      PomodoroPlaySound(pomodoro, GetSoundForCurrentTimer(pomodoro));
   }
   
   GuiLabel(MakeRectangle(5.0f, sh - 50.0f, 40.0f, 20.0f), EasyPrint("%d/%d", pomodoro->round, (int)pomodoro->config.rounds));
}

static void DrawChangeViewButton(Pomodoro* pomodoro) {
   // NOTE: The bounds computation does not work with icons. GuiLabelButton makes
   // the bounds wider than they really are.
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);

   const guiIconName icon = (pomodoro->currentView == View_Main ? RICON_GEAR : RICON_ARROW_LEFT);
   if (GuiLabelButton(MakeRectangle(-25, 5, 0, 20), GuiIconText(icon, NULL))) {
      pomodoro->currentView = (pomodoro->currentView == View_Main ? View_Settings : View_Main);
   }
}

static float DrawTimerConfigTimeSlider(const char* label, float* value, float y, float minValue, float maxValue, TimerConfigSliderFlag flag) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   const float ystep = 20.0f;

   Rectangle bounds = {sw / 2.0f - sw / 3.0f, y, sw * 2.0f / 3.0f, ystep};

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
   GuiLabel(bounds, label);

   const int rounded = RoundToInt(*value);
   const char* valueLabel = (flag == TimerConfigSliderFlag_Time ? EasyPrint("%d:00", rounded) : EasyPrint("%d", rounded));
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
   GuiLabel(bounds, valueLabel);
   bounds.y += ystep;

   *value = GuiSlider(bounds, NULL, NULL, *value, minValue, maxValue);
   bounds.y += 2.0f * ystep;

   return bounds.y;
}

static void DrawTimerPage(Pomodoro* pomodoro) {
   TimerConfig previous = pomodoro->config;
   TimerConfig current = pomodoro->config;

   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   const float ystep = sh / 16.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
   GuiSetFont(pomodoro->fonts.bigFont);

   Rectangle bounds = {0.0f, sh / 16.0f, sw, 0.0f};

   GuiLabel(bounds, GetTextByID(TextID_Timer));

   GuiSetFont(pomodoro->fonts.font);
   bounds.y += 2.0f * ystep;

   bounds.y = DrawTimerConfigTimeSlider(GetTextByID(TextID_Focus), &current.focus, bounds.y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   bounds.y = DrawTimerConfigTimeSlider(GetTextByID(TextID_ShortBreak), &current.shortBreak, bounds.y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   bounds.y = DrawTimerConfigTimeSlider(GetTextByID(TextID_LongBreak), &current.longBreak, bounds.y, 1.0f, 90.0f, TimerConfigSliderFlag_Time);
   bounds.y = DrawTimerConfigTimeSlider(GetTextByID(TextID_Rounds), &current.rounds, bounds.y, 1.0f, 12.0f, TimerConfigSliderFlag_Int);

   if (current.focus != previous.focus ||
       current.shortBreak != previous.shortBreak ||
       current.longBreak != previous.longBreak ||
       current.rounds != previous.rounds) {

      PomodoroSetFlag(pomodoro, PomodoroFlag_ConfigChanged);
   }

   pomodoro->config = current;
}

static void DrawOptionsPage(Pomodoro* pomodoro) {
   Options* options = &pomodoro->options;

   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   const float ystep = sh / 16.0f;

   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
   GuiSetFont(pomodoro->fonts.bigFont);

   Rectangle bounds = {0.0f, sh / 16.0f, sw, 0.0f};

   GuiLabel(bounds, GetTextByID(TextID_Options));

   GuiSetFont(pomodoro->fonts.font);
   bounds.y += 2.0f * ystep;

   bounds.x = sw / 2.0f - sw * 2.0f / 5.0f;
   bounds.width = 20.0f;
   bounds.height = 20.0f;

   options->alwaysOnTop = GuiCheckBox(bounds, GetTextByID(TextID_OptionAlwaysOnTop), options->alwaysOnTop);
   bounds.y += ystep;

   options->autoStartWorkTimer = GuiCheckBox(bounds, GetTextByID(TextID_OptionAutoStartWorkTimer), options->autoStartWorkTimer);
   bounds.y += ystep;

   options->autoStartBreakTimer = GuiCheckBox(bounds, GetTextByID(TextID_OptionAutoStartBreakTimer), options->autoStartBreakTimer);
   bounds.y += ystep;

   options->tickSounds = GuiCheckBox(bounds, GetTextByID(TextID_OptionTickSounds), options->tickSounds);
   bounds.y += ystep;
}

static void DrawStylesPage(Pomodoro* pomodoro) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   float ystep = sh / 16.0f;

   GuiSetFont(pomodoro->fonts.bigFont);
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   Rectangle bounds = {0.0f, sh / 16.0f, sw, 0.0f};

   GuiLabel(bounds, GetTextByID(TextID_Styles));
   bounds.y += 2.0f * ystep;

   GuiSetFont(pomodoro->fonts.font);
   bounds.height = (float)pomodoro->fonts.font.baseSize;

   const int styleCount = sb_count(pomodoro->styles);
   for (int styleIndex = 0; styleIndex < styleCount; ++styleIndex) {
      if (GuiLabelButton(bounds, pomodoro->styles[styleIndex].name)) {
         pomodoro->currentStyleIndex = styleIndex;
         PomodoroSetFlag(pomodoro, PomodoroFlag_UpdateStyle);
      }
      bounds.y += ystep;
   }
}

static void DrawSettingsViewPageTabs(Pomodoro* pomodoro) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   const float iconWidth = sw / 4.0f;

   Rectangle bounds = {0.0f, sh - 30.0f, iconWidth, 30.0f};

   if (GuiButton(bounds, GuiIconText(RICON_CLOCK, NULL))) {
      pomodoro->currentSettingsPage = SettingsPage_TimerConfig;
   }
   bounds.x += iconWidth;

   if (GuiButton(bounds, GuiIconText(RICON_TOOLS, NULL))) {
      pomodoro->currentSettingsPage = SettingsPage_Options;
   }
   bounds.x += iconWidth;

   if (GuiButton(bounds, GuiIconText(RICON_COLOR_BUCKET, NULL))) {
      pomodoro->currentSettingsPage = SettingsPage_Styles;
   }
   bounds.x += iconWidth;

   if (GuiButton(bounds, GuiIconText(RICON_INFO, NULL))) {
      pomodoro->currentSettingsPage = SettingsPage_About;
   }
}

static void DrawAboutPage(Pomodoro* pomodoro) {
   const float sw = (float)GetScreenWidth();
   const float sh = (float)GetScreenHeight();
   const float ystep = sh / 8.0f;

   GuiSetFont(pomodoro->fonts.bigFont);
   GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

   Rectangle bounds = {0.0f, sh / 16.0f, sw, 0.0f};

   GuiLabel(bounds, GetTextByID(TextID_About));
   bounds.y += ystep;

   GuiSetFont(pomodoro->fonts.font);
   GuiLabel(bounds, GetTextByID(TextID_Pomodoro));
   bounds.y += ystep;

   GuiLabel(bounds, GetTextByID(TextID_AboutDescription));
}

void PomodoroDraw(Pomodoro* pomodoro) {
   ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

   if (pomodoro->currentView == View_Main) {
      DrawTimerRings(pomodoro);
      DrawTimerText(pomodoro);
      DrawMainViewControlButtons(pomodoro);
   } else if (pomodoro->currentView == View_Settings) {

      switch (pomodoro->currentSettingsPage) {
         case SettingsPage_TimerConfig: DrawTimerPage(pomodoro); break;
         case SettingsPage_Options: DrawOptionsPage(pomodoro); break;
         case SettingsPage_Styles: DrawStylesPage(pomodoro); break;
         case SettingsPage_About: DrawAboutPage(pomodoro); break;
      }

      DrawSettingsViewPageTabs(pomodoro);
   }

   DrawChangeViewButton(pomodoro);
}
