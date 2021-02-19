#pragma once

#include <Windows.h>
#include <codecvt>
#include <shlobj.h>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "xor.h"
#pragma comment(lib, "winhttp.lib")



class Session {
public:
	bool authenticated = false;
	int auth_id = 0;

	string status = "";
	string username = "";
	string password = "";

	string encrypt(string str) {
		for (int i = 0; (i < 100 && str[i] != '\0'); i++)
			str[i] = str[i] + 2;

		return str;
	}

	string decrypt(string str) {
		for (int i = 0; (i < 100 && str[i] != '\0'); i++)
			str[i] = str[i] - 2;

		return str;
	}

	void toggle_input() {
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode = 0;
		GetConsoleMode(hStdin, &mode);
		SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
	}

	void handle_login() {
		while (!authenticated) {
			system("cls");
			cout << XorStr("[roseha.cc]") << endl;
			if (remember_credentials()) {
				cout << XorStr("Welcome back, ") << username << XorStr(".") << XorStr("\nLogging in...");
				authenticate(GetTickCount64());
				if (authenticated) {
					system("cls");
					break;
				}

				cout << XorStr("\nLogin failed, please login again.") << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(3500));
				system("cls");
			}

			cout << XorStr("[roseha.cc]\nUsername: ");
			cin >> username;

			toggle_input();

			cout << XorStr("Password: ");
			cin >> password;

			toggle_input();
			authenticate(GetTickCount64());

			cout << endl << status << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(2500));
			system("cls");
		}

		save_credentials();
		cout << XorStr("[+] Checking subscription state...") << endl;

		auto user_access = XorStr("RUST_BETA");

		settings->misc.session_username = username;
		cout << XorStr("[+] User Access: ") << user_access << endl;
		settings->misc.console_log(XorStr("[+] Console Logging Enabled"));
	}

	bool remember_credentials() {
		char my_documents[MAX_PATH];
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
		strcat(my_documents, XorStr("\\rosehacc"));
		std::filesystem::create_directories(my_documents);

		char file_location[128];
		sprintf(file_location, "%s\\rosehacc.login", my_documents);

		ifstream ifs(file_location);
		if (!ifs.is_open())
			return false;

		string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

		string delimiter = "|";
		username = decrypt(content.substr(0, content.find(delimiter)));
		password = decrypt(content.substr(username.length() + 1, content.length() - username.length()));
		return true;
	}

	void save_credentials() {
		char my_documents[MAX_PATH];
		SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
		strcat(my_documents, XorStr("\\rosehacc"));
		std::filesystem::create_directories(my_documents);

		char file_location[128];
		sprintf(file_location, "%s\\rosehacc.login", my_documents);

		ofstream credentials(file_location);

		credentials << encrypt(username) << XorStr("|") << encrypt(password);
		credentials.close();
	}

	void authenticate(int id) {
		char id_str[128];
		sprintf(id_str, XorStr("%d"), id);

		char web_path[256];
		sprintf(web_path, XorStr("/handler.php?username=%s&password=%s&id=%d"), username, password, id);

		authenticated = true;
		status = XorStr("Login successful.");
	}
};
