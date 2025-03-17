#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryDrivers(HKEY hKey)
{
    TCHAR achKey[MAX_KEY_LENGTH];
    DWORD cbName;
    DWORD cSubKeys = 0;
    FILETIME ftLastWriteTime;
    DWORD retCode;
    
    
    retCode = RegQueryInfoKey(hKey, NULL, NULL, NULL, &cSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (retCode != ERROR_SUCCESS) {
        printf("Failed to query registry info.\n");
        return;
    }

    for (DWORD i = 0; i < cSubKeys; i++) {
        cbName = MAX_KEY_LENGTH;
        retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
        if (retCode == ERROR_SUCCESS) {
            HKEY hSubKey;
            TCHAR subKeyPath[MAX_PATH];
            _stprintf_s(subKeyPath, TEXT("SYSTEM\\CurrentControlSet\\Services\\%s"), achKey);
            
            if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                DWORD typeValue, typeSize = sizeof(DWORD);
                DWORD imagePathSize = MAX_PATH;
                TCHAR imagePath[MAX_PATH];
                
                if (RegQueryValueEx(hSubKey, TEXT("Type"), NULL, NULL, (LPBYTE)&typeValue, &typeSize) == ERROR_SUCCESS) {
                    if (typeValue == SERVICE_KERNEL_DRIVER || typeValue == SERVICE_FILE_SYSTEM_DRIVER) {
                        if (RegQueryValueEx(hSubKey, TEXT("ImagePath"), NULL, NULL, (LPBYTE)imagePath, &imagePathSize) == ERROR_SUCCESS) {
                            _tprintf(TEXT("Driver: %s\nImagePath: %s\n\n"), achKey, imagePath);
                        }
                    }
                }
                RegCloseKey(hSubKey);
            }
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    HKEY hTestKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS) {
        QueryDrivers(hTestKey);
        RegCloseKey(hTestKey);
    } else {
        printf("Failed to open registry key.\n");
    }
    return 0;
}
