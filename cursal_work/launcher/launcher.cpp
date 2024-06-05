#include "launcher.h"

#ifdef _WIN32

process::process_handle_t process::launchProcess(const std::string& app, const std::string& arg)
{
    // Prepare handles.
    STARTUPINFOW si;
    PROCESS_INFORMATION pi; // The function returns this
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Prepare CreateProcess args
    std::wstring app_w;
    widen(app, app_w);

    std::wstring arg_w;
    widen(arg, arg_w);

    std::wstring input = app_w + L" " + arg_w;
    wchar_t* arg_concat = const_cast<wchar_t*>(input.c_str());
    const wchar_t* app_const = app_w.c_str();

    // Start the child process.
    if (!CreateProcessW(
        app_const,      // app path
        arg_concat,     // Command line (needs to include app path as first argument. args separated by whitespace)
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        throw std::runtime_error("Could not create child process");
    }
    else
    {
        std::cout << "[          ] Successfully launched child process" << std::endl;
    }

    // Return process handle
    return pi;
}

bool process::checkIfProcessIsActive(process_handle_t handle)
{
    // Check if handle is closed
    if (handle.hProcess == NULL)
    {
        printf("Process handle is closed or invalid (%d).\n", GetLastError());
        return false;
    }

    // If handle open, check if process is active
    DWORD lpExitCode = 0;
    if (GetExitCodeProcess(handle.hProcess, &lpExitCode) == 0)
    {
        printf("Cannot return exit code (%d).\n", GetLastError());
        throw std::runtime_error("Cannot return exit code");
    }
    else
    {
        return lpExitCode == STILL_ACTIVE;
    }
}

bool process::stopProcess(process_handle_t& handle)
{
    // Check if handle is invalid or has already been closed
    if (handle.hProcess == NULL)
    {
        printf("Process handle invalid. Possibly already been closed (%d).\n", GetLastError());
        return false;
    }

    // Terminate Process
    if (!TerminateProcess(handle.hProcess, 1))
    {
        printf("ExitProcess failed (%d).\n", GetLastError());
        return false;
    }

    // Wait until child process exits.
    if (WaitForSingleObject(handle.hProcess, INFINITE) == WAIT_FAILED)
    {
        printf("Wait for exit process failed(%d).\n", GetLastError());
        return false;
    }

    // Close process and thread handles.
    if (!CloseHandle(handle.hProcess))
    {
        printf("Cannot close process handle(%d).\n", GetLastError());
        return false;
    }
    else
    {
        handle.hProcess = NULL;
    }

    if (!CloseHandle(handle.hThread))
    {
        printf("Cannot close thread handle (%d).\n", GetLastError());
        return false;
    }
    else
    {
        handle.hThread = NULL;
    }
    return true;
}

const wchar_t* process::widen(const std::string& narrow, std::wstring& wide)
{
    size_t length = narrow.size();

    if (length > 0)
    {
        wide.resize(length);
        length = MultiByteToWideChar(CP_UTF8, 0, narrow.c_str(), (int)length, (wchar_t*)wide.c_str(), (int)length);
        wide.resize(length);
    }
    else
    {
        wide.clear();
    }

    return wide.c_str();
}

#else

process::process_handle_t process::launchProcess(const std::string& command, const std::string& arg)
{
	pid_t pid = fork();

	if (pid == 0) // Child process
	{
		char* const args[] = { const_cast<char*>(command.c_str()), const_cast<char*>(arg.c_str()), NULL };
		execvp(command.c_str(), args); // launch

		std::cerr << "Failed to execute command: " << command << std::endl;
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)  // parent process
	{
		return pid;
	}
	else
	{
		std::cerr << "Failed to fork process" << std::endl;
		return -1;
	}
}

bool process::checkIfProcessIsActive(process_handle_t handle)
{
	int status;
	if (waitpid(handle, &status, WNOHANG) == 0)
	{
		return true; // process is still running
	}
	else
	{
		if (WIFEXITED(status)) // has exited
		{
			std::cout << "Process " << handle << " exited with code " << WEXITSTATUS(status) << std::endl;
		}
		else if (WIFSIGNALED(status)) // killed
		{
			std::cout << "Process " << handle << " was killed by signal " << WTERMSIG(status) << std::endl;
		}
		return false;
	}
}

bool process::stopProcess(process_handle_t& handle)
{
	if (kill(handle, SIGTERM) == 0)
	{
		return true;
	}
	else
	{
		std::cerr << "Failed to send SIGTERM signal to process " << handle << std::endl;
		return false;
	}
}

#endif
