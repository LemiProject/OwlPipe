#pragma once

#include <Windows.h>

#include <string>
#include <format>

#include "owlpipeoptions.h"
#include "owlpipeerrors.h"
#include "owlpipebaseio.h"

namespace owlpipe {
	class OwlPipeClient : public OwlBaseIO {
	public:
		OwlPipeClient(HANDLE pipe_handle) : OwlBaseIO(pipe_handle) {

		}

		void Close() const {
			CloseHandle(GetHandle());
		}

		bool IsConnected() const {
			ULONG pid;
			return GetNamedPipeServerProcessId(GetHandle(), &pid) && pid > 0;
		}

		bool SetPipeState(DWORD& mode, LPDWORD maxcollcount, LPDWORD colldatatimeout) const {
			return SetNamedPipeHandleState(GetHandle(), &mode, maxcollcount, colldatatimeout);
		}

		static OwlPipeClient Connect(const std::string& name, OwlPipeClientOptions&& options = OwlPipeClientOptions{GENERIC_READ | GENERIC_WRITE, PIPE_READMODE_MESSAGE, 20000}) {
			HANDLE handle = CreateFile(name.c_str(), options.open_flags, 0, 0, OPEN_EXISTING, 0, 0);
			if ((GetLastError() != ERROR_PIPE_BUSY && GetLastError() != 0))
				throw OWLPIPEERROR(std::string("Cannot open pipe with error ").append(std::to_string(GetLastError())));

			if (handle == INVALID_HANDLE_VALUE) {
				if (!WaitNamedPipe(name.c_str(), options.wait_timeout))
					throw OWLPIPEERROR(std::string("Cannot open pipe with timeout ").append(std::to_string(options.wait_timeout)));
				handle = CreateFile(name.c_str(), options.open_flags, 0, 0, OPEN_EXISTING, 0, 0);
			}

			const OwlPipeClient cli(handle);

			DWORD mode = options.read_mode;
			if (!cli.SetPipeState(mode, 0, 0))
				throw OWLPIPEERROR(std::string("Cannot set pipe state with error ").append(std::to_string(GetLastError())));

			return cli;
		}
	};

}
