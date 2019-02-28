#pragma once
#include <Windows.h>

#ifndef DLL_EXPORTVHDAPI
#define DLL_VHDAPI extern "C" _declspec(dllimport)
#else
#define DLL_VHDAPI extern "C" _declspec(dllexport)
#endif
//����VHD
DLL_VHDAPI bool __cdecl CreateVHD_Fixed(PCWSTR pszVhdPath, ULONG sizeInMB);

//�򿪲�����VHD
DLL_VHDAPI DWORD __cdecl OpenAndAttachVHD(PCWSTR pszVhdPath);

//�򿪲�ʹ����VHD?
DLL_VHDAPI bool __cdecl OpenAndCompactVHD(PCWSTR pszVhdPath);

//�򿪲�����VHD
DLL_VHDAPI DWORD __cdecl OpenAndDetachVHD(PCWSTR pszVhdPath);

// �򿪲�����VHD�������ȷ���
DLL_VHDAPI bool __cdecl OpenAndExpandVHD(PCWSTR pszVhdPath, ULONG newSizeMB);

//�򿪲���ȡ����VHD
DLL_VHDAPI bool __cdecl OpenAndGetPhysVHD(PCWSTR pszVhdPath, PWSTR pszPhysicalDiskPath);

//�򿪲���ȡVHD��Ϣ
DLL_VHDAPI int __cdecl OpenAndGetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid); //����stringlist

//�򿪲��ϲ�VHD
DLL_VHDAPI bool __cdecl OpenAndMergeVHD(PCWSTR pszVhdPath);

//�򿪲�����VHD��Ϣ
DLL_VHDAPI bool __cdecl OpenAndSetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid, GUID *Guid);

