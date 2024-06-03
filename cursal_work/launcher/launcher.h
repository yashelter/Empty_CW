//
// Created by Des Caldnd on 6/3/2024.
//

#ifndef MP_OS_LAUNCHER_H
#define MP_OS_LAUNCHER_H


#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <cstdlib>
#include <string>
#include <algorithm>

class process
{
public:

    static PROCESS_INFORMATION launchProcess(std::string app, std::string arg)
    {

        // Prepare handles.
        STARTUPINFOW si;
        PROCESS_INFORMATION pi; // The function returns this
        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );
        //Prepare CreateProcess args
        std::wstring app_w;
        widen(app, app_w);

        std::wstring arg_w;
        widen(arg, arg_w);

        std::wstring input = app_w + L" " + arg_w;
        wchar_t* arg_concat = const_cast<wchar_t*>( input.c_str() );
        const wchar_t* app_const = app_w.c_str();

        // Start the child process.
        if( !CreateProcessW(
                app_const,      // app path
                arg_concat,     // Command line (needs to include app path as first argument. args seperated by whitepace)
                NULL,           // Process handle not inheritable
                NULL,           // Thread handle not inheritable
                FALSE,          // Set handle inheritance to FALSE
                0,              // No creation flags
                NULL,           // Use parent's environment block
                NULL,           // Use parent's starting directory
                &si,            // Pointer to STARTUPINFO structure
                &pi )           // Pointer to PROCESS_INFORMATION structure
                )
        {
            printf( "CreateProcess failed (%d).\n", GetLastError() );
            throw std::runtime_error("Could not create child process");
        }
        else
        {
            std::cout << "[          ] Successfully launched child process" << std::endl;
        }

        // Return process handle
        return pi;
    }

    static bool checkIfProcessIsActive(PROCESS_INFORMATION pi)
    {
        // Check if handle is closed
        if ( pi.hProcess == NULL )
        {
            printf( "Process handle is closed or invalid (%d).\n", GetLastError());
            return FALSE;
        }

        // If handle open, check if process is active
        DWORD lpExitCode = 0;
        if( GetExitCodeProcess(pi.hProcess, &lpExitCode) == 0)
        {
            printf( "Cannot return exit code (%d).\n", GetLastError() );
            throw std::runtime_error("Cannot return exit code");
        }
        else
        {
            if (lpExitCode == STILL_ACTIVE)
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    static bool stopProcess( PROCESS_INFORMATION &pi)
    {
        // Check if handle is invalid or has allready been closed
        if ( pi.hProcess == NULL )
        {
            printf( "Process handle invalid. Possibly allready been closed (%d).\n", GetLastError());
            return 0;
        }

        // Terminate Process
        if( !TerminateProcess(pi.hProcess,1))
        {
            printf( "ExitProcess failed (%d).\n", GetLastError() );
            return 0;
        }

        // Wait until child process exits.
        if( WaitForSingleObject( pi.hProcess, INFINITE ) == WAIT_FAILED)
        {
            printf( "Wait for exit process failed(%d).\n", GetLastError() );
            return 0;
        }

        // Close process and thread handles.
        if( !CloseHandle( pi.hProcess ))
        {
            printf( "Cannot close process handle(%d).\n", GetLastError() );
            return 0;
        }
        else
        {
            pi.hProcess = NULL;
        }

        if( !CloseHandle( pi.hThread ))
        {
            printf( "Cannot close thread handle (%d).\n", GetLastError() );
            return 0;
        }
        else
        {
            pi.hProcess = NULL;
        }
        return 1;
    }

    // helper to widen a narrow UTF8 string in Win32
    static const wchar_t* widen(const std::string& narrow, std::wstring& wide)
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


};//class process
#endif //win32

#endif //MP_OS_LAUNCHER_H
