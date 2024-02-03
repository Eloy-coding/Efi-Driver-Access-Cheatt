#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tlhelp32.h>
#include "MemProtector.h"
#include "globals.h"
#include "Driver.h"
#include "Netvars/Entity.h"
#include "Math.h"
#include "Netvars/Interfaces.h"


void LoadProtectedFunctions();
DWORD WINAPI mainThread(PVOID);
BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID);
bool CheckDriverStatus();
DWORD GetProcessIdByName(wchar_t* name);
uintptr_t milliseconds_now();
void HackThread();