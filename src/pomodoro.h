#pragma once

#pragma warning(push)
#pragma warning(disable : 4244) // conversion from 'int' to 'float', possible loss of data
#pragma warning(disable : 4267) // 'initializing': conversion from 'size_t' to 'int', possible loss of data

#include "raylib.h"
#include "raygui.h"

#pragma warning(pop)

#include "types.h"

void PomodoroInit(Pomodoro* pomodoro);
void PomodoroCleanup(Pomodoro* pomodoro);

void PomodoroAddStyle(Pomodoro* pomodoro, const char* name, const char* fileName);
void PomodoroUpdate(Pomodoro* pomodoro);
void PomodoroDraw(Pomodoro* pomodoro);
