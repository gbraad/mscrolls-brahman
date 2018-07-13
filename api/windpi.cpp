/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


#include <stdio.h>
#include <string>

#include <windows.h>
#include <tchar.h>

#include <SetupApi.h>
#include <cfgmgr32.h>   // for MAX_DEVICE_ID_LEN

#include "windpi.h"
 
#define NAME_SIZE 128

#ifdef UNICODE
#error compile without unicode
#endif
 
static const GUID GUID_CLASS_MONITOR = { 0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 };

typedef std::string string;

// Assumes hEDIDRegKey is valid
static bool GetMonitorSizeFromEDID(const HKEY hEDIDRegKey,
                                   int& WidthMm, int& HeightMm)
{
    DWORD dwType, AcutalValueNameLength = NAME_SIZE;
    TCHAR valueName[NAME_SIZE];
 
    BYTE EDIDdata[1024];
    DWORD edidsize = sizeof(EDIDdata);
 
    for (LONG i = 0, retValue = ERROR_SUCCESS; retValue != ERROR_NO_MORE_ITEMS; ++i)
    {
        retValue = RegEnumValue(hEDIDRegKey, i, &valueName[0],
            &AcutalValueNameLength, NULL, &dwType,
            EDIDdata, // buffer
            &edidsize); // buffer size
 
        if (retValue != ERROR_SUCCESS || 0 != _tcscmp(valueName, _T("EDID")))
            continue;
 
        WidthMm = ((EDIDdata[68] & 0xF0) << 4) + EDIDdata[66];
        HeightMm = ((EDIDdata[68] & 0x0F) << 8) + EDIDdata[67];
 
        return true; // valid EDID found
    }
    return false; // EDID not found
}
 
static bool GetSizeForDevID(const string& TargetDevID, int& WidthMm, int& HeightMm)
{
    HDEVINFO devInfo = SetupDiGetClassDevsEx(
        &GUID_CLASS_MONITOR, //class GUID
        NULL, //enumerator
        NULL, //HWND
        DIGCF_PRESENT | DIGCF_PROFILE, // Flags //DIGCF_ALLCLASSES|
        NULL, // device info, create a new one.
        NULL, // machine name, local machine
        NULL);// reserved
 
    if (NULL == devInfo)
        return false;
 
    bool bRes = false;
 
    for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i)
    {
        SP_DEVINFO_DATA devInfoData;
        memset(&devInfoData, 0, sizeof(devInfoData));
        devInfoData.cbSize = sizeof(devInfoData);
 
        if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
        {
            TCHAR Instance[MAX_DEVICE_ID_LEN];
            SetupDiGetDeviceInstanceId(devInfo, &devInfoData, Instance, MAX_PATH, NULL);
 
            string sInstance(Instance);

            if (sInstance.find(TargetDevID) == std::string::npos)
                continue;
 
            HKEY hEDIDRegKey = SetupDiOpenDevRegKey(devInfo, &devInfoData,
                DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
 
            if (!hEDIDRegKey || (hEDIDRegKey == INVALID_HANDLE_VALUE))
                continue;
 
            bRes = GetMonitorSizeFromEDID(hEDIDRegKey, WidthMm, HeightMm);
 
            RegCloseKey(hEDIDRegKey);
        }
    }
    SetupDiDestroyDeviceInfoList(devInfo);
    return bRes;
}
 
HMONITOR  g_hMonitor;
int       monIndex;
int       monResX, monResY;
 
BOOL CALLBACK MyMonitorEnumProc(
    _In_  HMONITOR hMonitor,
    _In_  HDC hdcMonitor,
    _In_  LPRECT lprcMonitor,
    _In_  LPARAM dwData
    )
{
    // Use this function to identify the monitor of interest: MONITORINFO contains the Monitor RECT.
    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);
 
    GetMonitorInfo(hMonitor, &mi);
    
    if (!g_hMonitor || !monIndex--)
    {
        g_hMonitor = hMonitor;
        monResX = mi.rcMonitor.right;
        monResY = mi.rcMonitor.bottom;
    }
    return TRUE;
}
 
static BOOL DisplayDeviceFromHMonitor(HMONITOR hMonitor, DISPLAY_DEVICE* ddMonOut)
{
    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &mi);
 
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(dd);
    DWORD devIdx = 0; // device index
 
    while (EnumDisplayDevices(0, devIdx, &dd, 0))
    {
        devIdx++;
        if (0 != _tcscmp(dd.DeviceName, mi.szDevice))
            continue;
 
        DISPLAY_DEVICE ddMon;
        ZeroMemory(&ddMon, sizeof(ddMon));
        ddMon.cb = sizeof(ddMon);
        DWORD MonIdx = 0;
 
        while (EnumDisplayDevices(dd.DeviceName, MonIdx, &ddMon, 0))
        {
            MonIdx++;
 
            memcpy(ddMonOut, &ddMon, ddMon.cb);
            return TRUE;
 
            ZeroMemory(&ddMon, sizeof(ddMon));
            ddMon.cb = sizeof(ddMon);
        }
 
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
    }
 
    return FALSE;
}

bool getMonitorSize(int mon, int& w, int& h)
{
    w = 0;
    h = 0;
    
    monIndex = mon;
    monResX = 0;
    monResY = 0;

    // find the monitor, set `g_hMonitor`
    EnumDisplayMonitors( 0, 0, MyMonitorEnumProc, 0);

    if (!g_hMonitor || !monResX || !monResY) return false; // didnt find it.

    DISPLAY_DEVICE ddMon;
    if (FALSE == DisplayDeviceFromHMonitor(g_hMonitor, &ddMon))
    {
        printf("did not find a monitor\n");
        return false;
    }

    string devName;
    const char* p = ddMon.DeviceID;
    while (*p && *p != '\\') ++p;
    if (*p)
    {
        ++p;// skip backslash
        const char* q = p;
        while (*q && *q != '\\') ++q; // find end
        devName = string(p, q - p);
    }
    return GetSizeForDevID(devName, w, h);
}

bool getMonitorDPI(int mon, float& dpi)
{
    dpi = 0;
    int w, h;
    bool res = getMonitorSize(mon, w, h);
    if (res)
    {
        float dpix = monResX/(w/25.4f);
        float dpiy = monResY/(h/25.4f);
        dpi = (dpix + dpiy)/2;
    }
    return res;
}
    
