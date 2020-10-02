#pragma once

typedef enum {
   TextID_Focus,
   TextID_LongBreak,
   TextID_ShortBreak,

   TextID_Timer,
   TextID_Options,
   TextID_Styles,
   TextID_About,

   TextID_Rounds,
   TextID_ResetTimerConfig,

   TextID_Pomodoro,
   TextID_AboutDescription,

   TextID_OptionAlwaysOnTop,
   TextID_OptionAutoStartWorkTimer,
   TextID_OptionAutoStartBreakTimer,
   TextID_OptionTickSounds,

   TextID_Reset,
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

typedef struct {
   Sound tick;
   Sound longBreak;
   Sound shortBreak;
   Sound work;
} Sounds;

typedef struct {
   bool alwaysOnTop;
   bool autoStartWorkTimer;
   bool autoStartBreakTimer;
   bool tickSounds;
} Options;

typedef struct {
   float focus;
   float shortBreak;
   float longBreak;
   float rounds;
} TimerConfig;

typedef enum {
   TimerConfigSliderFlag_Time,
   TimerConfigSliderFlag_Int,
} TimerConfigSliderFlag;

typedef struct {
   const char* name;
   const char* fileName;
} Style;

enum {
   PomodoroFlag_UpdateStyle = (1 << 0),
   PomodoroFlag_ConfigChanged = (1 << 1),
};

#define POMODORO_MAX_PATH  256

typedef struct {
   TimerConfig config;
   Timer timer;
   int round;

   View currentView;
   SettingsPage currentSettingsPage;

   Options options;

   Style* styles;
   int currentStyleIndex;

   Fonts fonts;
   Sounds sounds;

   char resFilePrefix[POMODORO_MAX_PATH];

   bool muted;

   int flags;
} Pomodoro;
