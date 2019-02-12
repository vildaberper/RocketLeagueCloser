#pragma once

#include <atomic>

#include "WHelper.cpp"

class RocketLeagueCloser {

private:

	const std::string TITLE = "Rocket League";
	const std::string EXECUTABLE = "RocketLeague.exe";

	std::thread thread;
	std::atomic_bool b;

	bool matches(const HWND& handle) const{
		std::string title = wh::getTitle(handle);
		if (title.length() < TITLE.length() || title.substr(0, TITLE.length()) != TITLE) return false;

		std::string executable = wh::getExecutable(handle);
		if (executable.length() < EXECUTABLE.length() || executable.substr(executable.length() - EXECUTABLE.length()) != EXECUTABLE) return false;

		return true;
	}

	void log(std::string message) const{
		printf((message + "\n").c_str());
	}

public:

	RocketLeagueCloser() {
		
	}

	~RocketLeagueCloser() {
		b.store(false);
		thread.join();
		Sleep(1000);
	}

	void run() {
		b.store(true);
		thread = std::thread([this] {

			char state = 0;
			char terminationtries = 0;

			HWND rlhandlewindow = 0;
			DWORD rlpid = 0;
			HANDLE rlhandleprocess = 0;

			while (b.load()) {
				switch (state) {

				case 0: { // Looking for RL handle

					HWND handle = wh::getForegroundWindow();
					if (!handle) break;

					if (matches(handle)) {
						rlhandlewindow = handle;

						GetWindowThreadProcessId(handle, &rlpid);
						rlhandleprocess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, rlpid);

						state = 1;
						log("Process found!");
					}
					
					Sleep(5000);

					break;
				}

				case 1: { // Waiting for foreground handle to change

					if (rlhandlewindow != wh::getForegroundWindow()) {
						state = 2;
						Sleep(5000);
					}else Sleep(2000);

					break;
				}

				case 2: { // Checking if RL is still running, close it if no window

					DWORD exitCode = 0;
					GetExitCodeProcess(rlhandleprocess, &exitCode);

					if (exitCode != STILL_ACTIVE) {
						state = 0;
						log("Process closed normally");
					}
					else if (!wh::isVisible(rlhandlewindow)) {
						log("Terminating process...");
						if (TerminateProcess(rlhandleprocess, 0)) {
							rlhandlewindow = 0;
							rlpid = 0;
							rlhandleprocess = 0;
							state = 0;
							terminationtries = 0;
							log("Process terminated");
							break;
						}
						if (++terminationtries >= 10) {
							state = 0;
							log("Failed to terminate process");
						}
						Sleep(100);
					}
					else {
						state = 1;
					}

					break;
				}

				}

			}

		});
	}

	void open() {
		log("open()");
	}

	void test() {
		log("test()");
	}

};