#include "vhdapi.h"
#include <windows.h>
#include <stdio.h>
#define DEFIND_GUID
#include <initguid.h>
#include <virtdisk.h>
#include <cguid.h>
#include <WTypesbase.h>
#include <combaseapi.h>
#pragma comment(lib, "VirtDisk.lib")

#define PHYS_PATH_LEN 1024+1
GUID zGuid = GUID_NULL;

bool CreateVHD_Fixed(PCWSTR pszVhdPath, ULONG sizeInMB)
{
	bool bRet = false;
	HANDLE hvhd;
	CREATE_VIRTUAL_DISK_PARAMETERS  params;
	VIRTUAL_DISK_ACCESS_MASK        mask;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};
	params.Version1.UniqueId = GUID_NULL;
	params.Version1.BlockSizeInBytes = 0;
	params.Version1.MaximumSize = sizeInMB * 1024 * 1024;
	params.Version1.ParentPath = NULL;
	params.Version1.SourcePath = NULL;
	params.Version1.SectorSizeInBytes = 512;
	params.Version = CREATE_VIRTUAL_DISK_VERSION_1;
	mask = VIRTUAL_DISK_ACCESS_CREATE;

	DWORD ret = CreateVirtualDisk(&vst,
		pszVhdPath,
		mask,
		NULL,
		// To create a dynamic disk, use CREATE_VIRTUAL_DISK_FLAG_NONE instead.
		CREATE_VIRTUAL_DISK_FLAG_FULL_PHYSICAL_ALLOCATION,
		0,
		&params,
		NULL,
		&hvhd);

	if (ret == ERROR_SUCCESS)
	{
		bRet = true;
	}
	else
	{
		bRet = false;
		printf("failed to create vdisk...err 0x%x\n", ret);
	}

	if (INVALID_HANDLE_VALUE != hvhd)
	{
		CloseHandle(hvhd);
	}

	return bRet;
}

DWORD OpenAndAttachVHD(PCWSTR pszVhdPath)
{
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	DWORD ret;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	ATTACH_VIRTUAL_DISK_PARAMETERS iparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	iparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_GET_INFO | VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("成功打开%ls...\n", pszVhdPath);

		ret = AttachVirtualDisk(hVhd, NULL,
			ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME, 0, &iparams, NULL);

		if (ERROR_SUCCESS == ret)
		{
			printf("成功附加%ls ...\n", pszVhdPath);
		}
		else
		{
			if( 0x16 == ret)
				printf("附加%ls失败：已成功附加,不能再附加！错误代码:0x%x\n", pszVhdPath, ret);
			else
				printf("附加%ls失败：未知原因！错误代码:0x%x\n", pszVhdPath, ret);
		}
	}
	else
	{
		if (0x02 == ret)
			printf("打开%ls失败：文件不存在！错误代码:0x%x\n", pszVhdPath,ret);
		else
			printf("打开%ls失败：未知原因！错误代码:0x%x\n", pszVhdPath, ret);
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return ret;

}

DWORD OpenAndDetachVHD(PCWSTR pszVhdPath)
{
	DWORD ret;
	DETACH_VIRTUAL_DISK_FLAG Flags;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, NULL /*&oparams*/, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("成功打开%ls...\n", pszVhdPath);

		Flags = DETACH_VIRTUAL_DISK_FLAG_NONE;
		ret = DetachVirtualDisk(hVhd, Flags, 0);
		if (ERROR_SUCCESS == ret)
		{
			printf("成功分离%ls ...\n", pszVhdPath);
		}
		else
		{
			if (0x490 == ret)
				printf("分离%ls失败：已成功分离,不能再分离！错误代码:0x%x\n", pszVhdPath, ret);
			else
				printf("分离%ls失败：未知原因！错误代码:0x%x\n", pszVhdPath, ret);

		}
	}
	else
	{
		if (0x02 == ret)
			printf("打开%ls失败：文件不存在！错误代码:0x%x\n", pszVhdPath, ret);
		else
			printf("打开%ls失败：未知原因！错误代码:0x%x\n", pszVhdPath, ret);
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return ret;
}

bool OpenAndCompactVHD(PCWSTR pszVhdPath)
{
	bool bRet = false;
	DWORD ret;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	COMPACT_VIRTUAL_DISK_PARAMETERS parameters;
	COMPACT_VIRTUAL_DISK_FLAG flags = COMPACT_VIRTUAL_DISK_FLAG_NONE;

	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		parameters.Version = COMPACT_VIRTUAL_DISK_VERSION_1;
		parameters.Version1.Reserved = 0;

		ret = CompactVirtualDisk(hVhd,
			COMPACT_VIRTUAL_DISK_FLAG_NONE,
			&parameters,
			0);

		if (ERROR_SUCCESS == ret)
		{
			printf("success expanding vdisk...\n");
			bRet = true;
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			bRet = false;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		bRet = false;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;	
}


bool OpenAndExpandVHD(PCWSTR pszVhdPath, ULONG newSizeMB)
{
	bool bRet = false;
	DWORD ret;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	EXPAND_VIRTUAL_DISK_PARAMETERS xparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS,
		OPEN_VIRTUAL_DISK_FLAG_NONE, NULL, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		xparams.Version = EXPAND_VIRTUAL_DISK_VERSION_1;
		xparams.Version1.NewSize = newSizeMB * 1024 * 1024;

		ret = ExpandVirtualDisk(hVhd, EXPAND_VIRTUAL_DISK_FLAG_NONE, &xparams, 0);

		if (ERROR_SUCCESS == ret)
		{
			printf("success expanding vdisk...\n");
			bRet = true;
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			bRet = false;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		bRet = false;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

bool OpenAndGetPhysVHD(PCWSTR pszVhdPath, PWSTR pszPhysicalDiskPath)
{
	bool bRet = false;
	HANDLE hVhd = INVALID_HANDLE_VALUE;
	DWORD ret;
	OPEN_VIRTUAL_DISK_PARAMETERS oparams;
	ATTACH_VIRTUAL_DISK_PARAMETERS iparams;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	oparams.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparams.Version1.RWDepth = OPEN_VIRTUAL_DISK_RW_DEPTH_DEFAULT;

	iparams.Version = ATTACH_VIRTUAL_DISK_VERSION_1;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ATTACH_RW | VIRTUAL_DISK_ACCESS_GET_INFO | VIRTUAL_DISK_ACCESS_DETACH,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparams, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		ULONG sizePhysicalDisk;
		printf("success opening vdisk...\n");
		memset(pszPhysicalDiskPath, 0, sizeof(wchar_t) * PHYS_PATH_LEN);
		sizePhysicalDisk = (PHYS_PATH_LEN * sizeof(wchar_t)) * 256;
		ret = GetVirtualDiskPhysicalPath(hVhd, &sizePhysicalDisk, pszPhysicalDiskPath);
		if (ERROR_SUCCESS == ret)
		{
			wprintf(L"success getting physical path %s vhdname\n", pszPhysicalDiskPath);
			bRet = true;
		}
		else
		{
			printf("failed to get vhd physical info %d\n", ret);
			bRet = false;
		}
	}
	else
	{
		printf("failed to open vdisk...err 0x%x\n", ret);
		bRet = false;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}


int OpenAndGetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid)
{
	BOOL bRet = FALSE;
	DWORD ret;
	HANDLE hVhd;
	GET_VIRTUAL_DISK_INFO Info;
	ULONG InfoSize;
	ULONG SizeUsed;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ALL, OPEN_VIRTUAL_DISK_FLAG_NONE,
		NULL, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");
		InfoSize = (ULONG)sizeof(GET_VIRTUAL_DISK_INFO);
		Info.Version = GET_VIRTUAL_DISK_INFO_SIZE;
		ret = GetVirtualDiskInformation(hVhd,
			&InfoSize,
			&Info,
			&SizeUsed);

		if (ret == ERROR_SUCCESS)
		{
			printf("success getting virtual disk size info\n");
			printf("Info.Size.VirtualSize  (bytes) = %I64d (dec)\n", Info.Size.VirtualSize);
			printf("Info.Size.PhysicalSize (bytes) = %I64d (dec)\n", Info.Size.PhysicalSize);
			printf("Info.Size.BlockSize    (bytes) = %d (dec)\n", Info.Size.BlockSize);
			printf("Info.Size.SectorSize   (bytes) = %d (dec)\n", Info.Size.SectorSize);
			bRet = TRUE;
		}
		else
		{
			printf("failed to get virtual disk size info %d\n", ret);
			//PrintErrorMessage(GetLastError());
		}

		InfoSize = (ULONG)sizeof(GET_VIRTUAL_DISK_INFO);
		Info.Version = GET_VIRTUAL_DISK_INFO_IDENTIFIER;
		ret = GetVirtualDiskInformation(hVhd,
			&InfoSize,
			&Info,
			&SizeUsed);

		if (ret == ERROR_SUCCESS)
		{
			StringFromCLSID(Info.Identifier, (LPOLESTR *)&pszGuid);
			printf("success getting virtual disk ID info\n");
			wprintf(L"Info.Identifier  (GUID) = %s\n", pszGuid);
			bRet = TRUE;
		}
		else
		{
			printf("failed to get virtual disk ID info %d\n", ret);
			//PrintErrorMessage(GetLastError());
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

bool OpenAndMergeVHD(PCWSTR pszVhdPath)
{
	bool bRet = false;
	DWORD ret;
	HANDLE hVhd;
	MERGE_VIRTUAL_DISK_PARAMETERS mparms;
	OPEN_VIRTUAL_DISK_PARAMETERS oparms;
	MERGE_VIRTUAL_DISK_FLAG flags = MERGE_VIRTUAL_DISK_FLAG_NONE;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	oparms.Version = OPEN_VIRTUAL_DISK_VERSION_1;
	oparms.Version1.RWDepth = 2;

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_METAOPS | VIRTUAL_DISK_ACCESS_GET_INFO,
		OPEN_VIRTUAL_DISK_FLAG_NONE, &oparms, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		mparms.Version = MERGE_VIRTUAL_DISK_VERSION_1;
		mparms.Version1.MergeDepth = oparms.Version1.RWDepth - 1; //MERGE_VIRTUAL_DISK_DEFAULT_MERGE_DEPTH;

		ret = MergeVirtualDisk(hVhd, flags, &mparms, NULL);

		if (ERROR_SUCCESS == ret)
		{
			printf("success merging vdisk...\n");
			bRet = true;
		}
		else
		{
			printf("failed to expand vdisk... %d\n", ret);
			bRet = false;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		bRet = false;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}

bool OpenAndSetVHDInfo(PCWSTR pszVhdPath, PCWSTR pszGuid, GUID * Guid)
{
	bool bRet = false;
	DWORD ret;
	HANDLE hVhd;
	ULONG InfoSize;
	SET_VIRTUAL_DISK_INFO SetInfo;
	VIRTUAL_STORAGE_TYPE            vst =
	{
		VIRTUAL_STORAGE_TYPE_DEVICE_VHD,
		VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT
	};

	ret = OpenVirtualDisk(&vst, pszVhdPath,
		VIRTUAL_DISK_ACCESS_ALL, OPEN_VIRTUAL_DISK_FLAG_NONE,
		NULL, &hVhd);

	if (ERROR_SUCCESS == ret)
	{
		printf("success opening vdisk...\n");

		SetInfo.Version = SET_VIRTUAL_DISK_INFO_IDENTIFIER;
		InfoSize = sizeof(SetInfo);
		SetInfo.UniqueIdentifier = zGuid; //*Guid;

		ret = SetVirtualDiskInformation(hVhd, &SetInfo);
		if (ret == ERROR_SUCCESS)
		{
			printf("success setting vhd info\n");
			bRet = true;
		}
		else
		{
			printf("failed to set vhd info %d\n", ret);
			bRet = false;
		}
	}
	else
	{
		printf("failed to open vdisk...err %d\n", ret);
		bRet = false;
	}

	if (INVALID_HANDLE_VALUE != hVhd)
	{
		CloseHandle(hVhd);
	}

	return bRet;
}
