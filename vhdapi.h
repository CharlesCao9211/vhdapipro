#pragma once
#include <Windows.h>

#ifndef DLL_EXPORTVHDAPI
#define DLL_VHDAPI extern "C" _declspec(dllimport)
#else
#define DLL_VHDAPI extern "C" _declspec(dllexport)
#endif
//创建VHD
DLL_VHDAPI bool __cdecl CreateVHD_Fixed(PCWSTR pszVhdPath, ULONG sizeInMB);

//打开并附加VHD
DLL_VHDAPI DWORD __cdecl OpenAndAttachVHD(PCWSTR pszVhdPath);

//打开并使其简洁VHD?
DLL_VHDAPI bool __cdecl OpenAndCompactVHD(PCWSTR pszVhdPath);

//打开并分离VHD
DLL_VHDAPI DWORD __cdecl OpenAndDetachVHD(PCWSTR pszVhdPath);

// 打开并扩容VHD，必须先分离
DLL_VHDAPI bool __cdecl OpenAndExpandVHD(PCWSTR pszVhdPath, ULONG newSizeMB);

//打开并获取物理VHD
DLL_VHDAPI bool __cdecl OpenAndGetPhysVHD(PCWSTR pszVhdPath, PWSTR pszPhysicalDiskPath);

//打开并获取VHD信息
DLL_VHDAPI int __cdecl OpenAndGetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid); //返回stringlist

//打开并合并VHD
DLL_VHDAPI bool __cdecl OpenAndMergeVHD(PCWSTR pszVhdPath);

//打开并设置VHD信息
DLL_VHDAPI bool __cdecl OpenAndSetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid, GUID *Guid);

