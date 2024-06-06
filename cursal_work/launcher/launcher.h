#ifndef MP_OS_LAUNCHER_H
#define MP_OS_LAUNCHER_H

#include <string>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#endif

class process {
public:
#ifdef _WIN32
    using process_handle_t = PROCESS_INFORMATION;
#else
    using process_handle_t = pid_t;
#endif

    static process_handle_t launchProcess(const std::string& app, const std::string& arg);

    static bool checkIfProcessIsActive(process_handle_t handle);

    static bool stopProcess(process_handle_t& handle);

#ifdef _WIN32
    static const wchar_t* widen(const std::string& narrow, std::wstring& wide);
#endif
};//class process


#endif //MP_OS_LAUNCHER_H
