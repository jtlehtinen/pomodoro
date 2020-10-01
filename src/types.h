#pragma once

typedef enum {
   TextID_Focus,
   TextID_LongBreak,
   TextID_ShortBreak,
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
