#pragma once

typedef enum {
   TextID_Focus,
   TextID_LongBreak,
   TextID_ShortBreak,

   TextID_Timer,
   TextID_Options,
   TextID_Styles,
   TextID_About,

   TextID_Pomodoro,
   TextID_AboutDescription,
} TextID;

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

typedef struct {
   int minutes;
   int seconds;
} Duration;

typedef enum {
   View_Main,
   View_Settings,
} View;

typedef enum {
   SettingsPage_TimerConfig,
   SettingsPage_Options,
   SettingsPage_Styles,
   SettingsPage_About,
} SettingsPage;

typedef struct {
   Font font;
   Font bigFont;
   Font extraBigFont;
} Fonts;
