#pragma once
#include "aura.h"
#include <random>

namespace Helpers {
	static bool enable_privladge(LPCWSTR lpPrivilegeName) {
		TOKEN_PRIVILEGES Privilege;
		HANDLE hToken;
		DWORD dwErrorCode;

		Privilege.PrivilegeCount = 1;
		Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
			&Privilege.Privileges[0].Luid))
			return GetLastError();

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES, &hToken))
			return GetLastError();

		if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
			NULL, NULL)) {
			dwErrorCode = GetLastError();
			CloseHandle(hToken);
			return dwErrorCode;
		}

		CloseHandle(hToken);
		return TRUE;
	}

	static bool create_regkey(LPWSTR lpSubKeyy, LPWSTR image_path) {
		// RegSetKey Values
		ULONG dwErrorCode;
		LPWSTR lpSubKey = lpSubKeyy;
		HKEY hKey = HKEY_LOCAL_MACHINE;
		DWORD Reserved = 0;
		LPWSTR lpClass = NULL;
		DWORD dwOptions = 0;
		REGSAM samDesired = KEY_ALL_ACCESS;
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
		HKEY phkResult;
		// Set Value settings
		DWORD dwDisposition;
		DWORD dwServiceType = 1;
		DWORD dwServiceErrorControl = 1;
		DWORD dwServiceStart = 3;
		// RegSetValueExW values for image
		LPCWSTR lpValueImagePath = L"ImagePath";
		DWORD dwType = REG_EXPAND_SZ;
		LPWSTR ServiceImagePath = image_path;
		SIZE_T ServiceImagePathSize;
		// RegSetValueExW values for Type
		LPCWSTR lpValueImageType = L"Type";
		DWORD dwTypeDWord = REG_DWORD;
		// RegSetValueExW values for Error
		LPCWSTR lpValueError = L"ErrorControl";
		// RegSetValueExW values for Start
		LPCWSTR lpValueStart = L"Start";

		dwErrorCode = RegCreateKeyExW(hKey,	lpSubKey, Reserved,	lpClass, dwOptions, samDesired,	lpSecurityAttributes, &phkResult, &dwDisposition);
		if (dwErrorCode) {
			RegCloseKey(phkResult);
			return FALSE;
		}
		// TODO : how to get byte size?
		ServiceImagePathSize = (lstrlenW(ServiceImagePath) + 1) * sizeof(WCHAR);
		dwErrorCode = RegSetValueExW(phkResult,	lpValueImagePath, Reserved,	dwType, (const BYTE*)ServiceImagePath, ServiceImagePathSize);
		if (dwErrorCode) {
			RegCloseKey(phkResult);
			return FALSE;
		}

		// Set the Type for key
		dwErrorCode = RegSetValueExW(phkResult, lpValueImageType, Reserved, dwTypeDWord, (const BYTE*)&dwServiceType, sizeof(DWORD));
		if (dwErrorCode) {
			RegCloseKey(phkResult);
			return FALSE;
		}

		dwErrorCode = RegSetValueExW(phkResult,	lpValueError, Reserved,	dwTypeDWord, (const BYTE*)&dwServiceErrorControl, sizeof(DWORD));
		if (dwErrorCode) {
			RegCloseKey(phkResult);
			return FALSE;
		}

		// Set the Start key
		dwErrorCode = RegSetValueExW(phkResult,	lpValueStart, Reserved,	dwTypeDWord, (const BYTE*)&dwServiceStart, sizeof(DWORD));
		if (dwErrorCode) {
			RegCloseKey(phkResult);
			return FALSE;
		}

		RegCloseKey(phkResult);
		return TRUE;
	}

	static std::wstring s2ws(const std::string& s) {
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}


	static std::string random_string(int size) {
		std::string chars(XorStr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));

		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(chars.begin(), chars.end(), generator);
		return chars.substr(0, size);
	}

	static string UnicodeToUTF8(const wstring& ws) {
		string s;
		for (int i = 0; i < ws.size(); ++i)
		{
			wchar_t wc = ws[i];
			if (0 <= wc && wc <= 0x7f)
			{
				s += (char)wc;
			}
			else if (0x80 <= wc && wc <= 0x7ff)
			{
				s += (0xc0 | (wc >> 6));
				s += (0x80 | (wc & 0x3f));
			}
			else if (0x800 <= wc && wc <= 0xffff)
			{
				s += (0xe0 | (wc >> 12));
				s += (0x80 | ((wc >> 6) & 0x3f));
				s += (0x80 | (wc & 0x3f));
			}
			else if (0x10000 <= wc && wc <= 0x1fffff)
			{
				s += (0xf0 | (wc >> 18));
				s += (0x80 | ((wc >> 12) & 0x3f));
				s += (0x80 | ((wc >> 6) & 0x3f));
				s += (0x80 | (wc & 0x3f));
			}
			else if (0x200000 <= wc && wc <= 0x3ffffff)
			{
				s += (0xf8 | (wc >> 24));
				s += (0x80 | ((wc >> 18) & 0x3f));
				s += (0x80 | ((wc >> 12) & 0x3f));
				s += (0x80 | ((wc >> 6) & 0x3f));
				s += (0x80 | (wc & 0x3f));
			}
			else if (0x4000000 <= wc && wc <= 0x7fffffff)
			{
				s += (0xfc | (wc >> 30));
				s += (0x80 | ((wc >> 24) & 0x3f));
				s += (0x80 | ((wc >> 18) & 0x3f));
				s += (0x80 | ((wc >> 12) & 0x3f));
				s += (0x80 | ((wc >> 6) & 0x3f));
				s += (0x80 | (wc & 0x3f));
			}
		}
		return s;
	}

	static D3DXVECTOR3 FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return D3DXVECTOR3(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2)
		{
			return D3DXVECTOR3(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3)
		{
			return D3DXVECTOR3(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4)
		{
			return D3DXVECTOR3(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return D3DXVECTOR3(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else
		{
			return D3DXVECTOR3(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}
}