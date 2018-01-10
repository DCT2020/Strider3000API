/*
 * Copyright (C) 2012-2017 Motion Systems
 * 
 * This file is part of ForceSeat motion system.
 *
 * www.motionsystems.eu
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <windows.h>
#include <stdlib.h>

static HMODULE ForceSeatMI_LoadLibrarySecure();
static void    ForceSeatMI_FreeLibrarySecure(HMODULE handle);

#include "ForceSeatMI_Functions.h"

typedef FSMI_Handle (*ForceSeatMI_Create_Func)                    ();
typedef void        (*ForceSeatMI_Delete_Func)                    (FSMI_Handle api);
typedef FSMI_Bool   (*ForceSeatMI_BeginMotionControl_Func)        (FSMI_Handle api);
typedef FSMI_Bool   (*ForceSeatMI_EndMotionControl_Func)          (FSMI_Handle api);
typedef FSMI_Bool   (*ForceSeatMI_GetPlatformInfoEx_Func)         (FSMI_Handle api, FSMI_PlatformInfo* platformInfo, unsigned int platformInfoStructSize, unsigned int timeout);
typedef FSMI_Bool   (*ForceSeatMI_SendTelemetry_Func)             (FSMI_Handle api, const FSMI_Telemetry* telemetry);
typedef FSMI_Bool   (*ForceSeatMI_SendTopTablePosLog_Func)        (FSMI_Handle api, const FSMI_TopTablePositionLogical* position);
typedef FSMI_Bool   (*ForceSeatMI_SendTopTablePosPhy_Func)        (FSMI_Handle api, const FSMI_TopTablePositionPhysical* position);
typedef FSMI_Bool   (*ForceSeatMI_SendTopTableMatrixPhy_Func)     (FSMI_Handle api, const FSMI_TopTableMatrixPhysical* matrix);
typedef FSMI_Bool   (*ForceSeatMI_SendTactileFeedbackEffects_Func)(FSMI_Handle api, const FSMI_TactileFeedbackEffects* effects);

typedef struct ForceSeatMI_Functions
{
	ForceSeatMI_Create_Func                        Create;
	ForceSeatMI_Delete_Func                        Delete;
	ForceSeatMI_BeginMotionControl_Func            BeginMotionControl;
	ForceSeatMI_EndMotionControl_Func              EndMotionControl;
	ForceSeatMI_GetPlatformInfoEx_Func             GetPlatformInfoEx;
	ForceSeatMI_SendTelemetry_Func                 SendTelemetry;
	ForceSeatMI_SendTopTablePosLog_Func            SendTopTablePosLog;
	ForceSeatMI_SendTopTablePosPhy_Func            SendTopTablePosPhy;
	ForceSeatMI_SendTopTableMatrixPhy_Func         SendTopTableMatrixPhy;
	ForceSeatMI_SendTactileFeedbackEffects_Func    SendTactileFeedbackEffects;
	HMODULE                                        LibraryHandle;
	FSMI_Handle                                    API;
} ForceSeatMI_Functions;

static void ForceSeatMI_LoadLibrary(ForceSeatMI_Functions* functions)
{
	if (! functions)
	{
		return;
	}
	memset(functions, 0, sizeof(ForceSeatMI_Functions));
	functions->LibraryHandle = ForceSeatMI_LoadLibrarySecure();
	if (functions->LibraryHandle == NULL)
	{
		return;
	}

	functions->Create                     = (ForceSeatMI_Create_Func)                    GetProcAddress(functions->LibraryHandle, "ForceSeatMI_Create");
	functions->Delete                     = (ForceSeatMI_Delete_Func)                    GetProcAddress(functions->LibraryHandle, "ForceSeatMI_Delete");
	functions->BeginMotionControl         = (ForceSeatMI_BeginMotionControl_Func)        GetProcAddress(functions->LibraryHandle, "ForceSeatMI_BeginMotionControl");
	functions->EndMotionControl           = (ForceSeatMI_EndMotionControl_Func)          GetProcAddress(functions->LibraryHandle, "ForceSeatMI_EndMotionControl");
	functions->GetPlatformInfoEx          = (ForceSeatMI_GetPlatformInfoEx_Func)         GetProcAddress(functions->LibraryHandle, "ForceSeatMI_GetPlatformInfoEx");
	functions->SendTelemetry              = (ForceSeatMI_SendTelemetry_Func)             GetProcAddress(functions->LibraryHandle, "ForceSeatMI_SendTelemetry");
	functions->SendTopTablePosLog         = (ForceSeatMI_SendTopTablePosLog_Func)        GetProcAddress(functions->LibraryHandle, "ForceSeatMI_SendTopTablePosLog");
	functions->SendTopTablePosPhy         = (ForceSeatMI_SendTopTablePosPhy_Func)        GetProcAddress(functions->LibraryHandle, "ForceSeatMI_SendTopTablePosPhy");
	functions->SendTopTableMatrixPhy      = (ForceSeatMI_SendTopTableMatrixPhy_Func)     GetProcAddress(functions->LibraryHandle, "ForceSeatMI_SendTopTableMatrixPhy");
	functions->SendTactileFeedbackEffects = (ForceSeatMI_SendTactileFeedbackEffects_Func)GetProcAddress(functions->LibraryHandle, "ForceSeatMI_SendTactileFeedbackEffects");

	if (functions->Create                     == NULL || 
		functions->Delete                     == NULL ||
		functions->BeginMotionControl         == NULL ||
		functions->EndMotionControl           == NULL ||
		functions->GetPlatformInfoEx          == NULL ||
		functions->SendTelemetry              == NULL ||
		functions->SendTopTablePosLog         == NULL ||
		functions->SendTopTablePosPhy         == NULL ||
		functions->SendTopTableMatrixPhy      == NULL ||
		functions->SendTactileFeedbackEffects == NULL)
	{
		ForceSeatMI_FreeLibrarySecure(functions->LibraryHandle);
		memset(functions, 0, sizeof(ForceSeatMI_Functions));
	}
}

static void ForceSeatMI_FreeLibrary(ForceSeatMI_Functions* functions)
{
	if (functions)
	{
		if (functions->LibraryHandle)
		{
			FreeLibrary(functions->LibraryHandle);
		}
		memset(functions, 0, sizeof(ForceSeatMI_Functions));
	}
}

FSMI_Handle __cdecl ForceSeatMI_Create()
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)malloc(sizeof(ForceSeatMI_Functions));
	ForceSeatMI_LoadLibrary(functions);
	if (! functions || ! functions->Create)
	{
		ForceSeatMI_FreeLibrary(functions);
		free(functions); // Free memory if it was reserved
		return NULL;
	}
	functions->API = functions->Create();
	return (FSMI_Handle)functions;
}

void __cdecl ForceSeatMI_Delete(FSMI_Handle api)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->Delete)
	{
		functions->Delete(functions->API);
	}
	ForceSeatMI_FreeLibrary(functions);
	free(functions);
}

FSMI_Bool __cdecl ForceSeatMI_BeginMotionControl(FSMI_Handle api)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->BeginMotionControl)
	{
		return functions->BeginMotionControl(functions->API);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_EndMotionControl(FSMI_Handle api)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->EndMotionControl)
	{
		return functions->EndMotionControl(functions->API);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_GetPlatformInfoEx(FSMI_Handle api,
                                                FSMI_PlatformInfo* platformInfo,
                                                unsigned int platformInfoStructSize, 
                                                unsigned int timeout)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->GetPlatformInfoEx)
	{
		return functions->GetPlatformInfoEx(functions->API, platformInfo, platformInfoStructSize, timeout);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_SendTelemetry(FSMI_Handle api,
                                            const FSMI_Telemetry* telemetry)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->SendTelemetry)
	{
		return functions->SendTelemetry(functions->API, telemetry);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_SendTopTablePosLog(FSMI_Handle api,
                                                 const FSMI_TopTablePositionLogical* position)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->SendTopTablePosLog)
	{
		return functions->SendTopTablePosLog(functions->API, position);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_SendTopTablePosPhy(FSMI_Handle api,
                                                 const FSMI_TopTablePositionPhysical* position)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->SendTopTablePosPhy)
	{
		return functions->SendTopTablePosPhy(functions->API, position);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_SendTopTableMatrixPhy(FSMI_Handle api,
                                                    const FSMI_TopTableMatrixPhysical* matrix)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->SendTopTableMatrixPhy)
	{
		return functions->SendTopTableMatrixPhy(functions->API, matrix);
	}
	return FSMI_False;
}

FSMI_Bool __cdecl ForceSeatMI_SendTactileFeedbackEffects(FSMI_Handle api,
                                                         const FSMI_TactileFeedbackEffects* effects)
{
	ForceSeatMI_Functions* functions = (ForceSeatMI_Functions*)api;
	if (functions && functions->SendTactileFeedbackEffects)
	{
		return functions->SendTactileFeedbackEffects(functions->API, effects);
	}
	return FSMI_False;
}

HMODULE ForceSeatMI_LoadLibrarySecure()
{
#ifdef _M_X64
	static const wchar_t* const LIBRARY_DLL_NAME = L"ForceSeatMI64.dll";
	static const wchar_t* const REG_PATH         = L"SOFTWARE\\Wow6432Node\\MotionSystems\\ForceSeatPM";
#else
	static const wchar_t* const LIBRARY_DLL_NAME = L"ForceSeatMI32.dll";
	static const wchar_t* const REG_PATH         = L"SOFTWARE\\MotionSystems\\ForceSeatPM";
#endif

	HKEY hKey  = NULL;
	HMODULE hLibrary = NULL;

	// Let's check if there is ForceSeatPM installed, if yes there is ForceSeatMIxx.dll that can be used
	if (ERROR_SUCCESS == RegOpenKeyW(HKEY_LOCAL_MACHINE, REG_PATH, &hKey))
	{
		wchar_t buffer[MAX_PATH * 2] = {0};
		DWORD dwBufSize = sizeof(buffer);
		DWORD dwType = REG_SZ;

		if (ERROR_SUCCESS == RegQueryValueExW(hKey, L"InstallationPath", 0, &dwType, (BYTE*)buffer, &dwBufSize))
		{
			wcscat_s(buffer, MAX_PATH * 2, L"\\");
			wcscat_s(buffer, MAX_PATH * 2, LIBRARY_DLL_NAME);
			hLibrary = LoadLibraryW(buffer);
		}

		RegCloseKey(hKey);
	}

	// If there is still not ForceSeatMIxx.dll found, then let's try in standard search path
	if (hLibrary == NULL)
	{
		hLibrary = LoadLibraryW(LIBRARY_DLL_NAME);
	}

	return hLibrary;
}

void ForceSeatMI_FreeLibrarySecure(HMODULE handle)
{
	FreeLibrary(handle);
}