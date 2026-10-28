//
// Created by ehart on 10/26/2026.
//

// Global cpp includes, could move to a precompiled header
#include <iostream>
#include <string>
#include <arv.h>
#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <libwdi.h>
#pragma comment(lib, "setupapi.lib")

bool IsLibUsbKInstalled();

bool InstallLibUsbKForDevice();

