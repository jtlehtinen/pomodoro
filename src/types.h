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

   TextID_StyleAshes,
   TextID_StyleBluish,
   TextID_StyleCandy,
   TextID_StyleCherry,
   TextID_StyleCyber,
   TextID_StyleDefault,
   TextID_StyleJungle,
   TextID_StyleLavanda,
   TextID_StyleTerminal,

   TextID_OptionAlwaysOnTop,
   TextID_OptionAutoStartWorkTimer,
   TextID_OptionAutoStartBreakTimer,
   TextID_OptionTickSounds,
   TextID_OptionDesktopNotifications,
   TextID_OptionMinimizeToTray,
   TextID_OptionMinimizeToTrayOnClose,
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

typedef enum {
   Style_Ashes,
   Style_Bluish,
   Style_Candy,
   Style_Cherry,
   Style_Cyber,
   Style_Default,
   Style_Jungle,
   Style_Lavanda,
   Style_Terminal,
} Style;

typedef struct {
   bool alwaysOnTop;
   bool autoStartWorkTimer;
   bool autoStartBreakTimer;
   bool tickSounds;
   bool desktopNotifications;
   bool minimizeToTray;
   bool minimizeToTrayOnClose;
} Options;

typedef struct {
   float focus;
   float shortBreak;
   float longBreak;
   float rounds;
   bool changed;
} TimerConfig;

typedef enum {
   TimerConfigSliderFlag_Time,
   TimerConfigSliderFlag_Int,
} TimerConfigSliderFlag;
