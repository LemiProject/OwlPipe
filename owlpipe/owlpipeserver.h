#pragma once

#include <Windows.h>

#include "owlpipebaseio.h"
#include "owlpipeerrors.h"
#include "owlpipeoptions.h"

namespace owlpipe {
	class OwlPipeServer : public OwlBaseIO {
	public:
		OwlPipeServer(HANDLE handle) : OwlBaseIO(handle) {

		}

		void DisconnectClient() const {
			if (!DisconnectNamedPipe(GetHandle()))
				throw OWLPIPEERROR(std::string("Cannot disconnect pipe with error: ").append(std::to_string(GetLastError())));
		}

		void Close() const {
			if (!CloseHandle(GetHandle()))
				throw OWLPIPEERROR(std::string("Cannot close pipe handle with error: ").append(std::to_string(GetLastError())));
		}

		bool ConnectClient() const {
			return ConnectNamedPipe(GetHandle(), 0);
		}

		static OwlPipeServer CreateServer(const std::string& name, OwlPipeServerOptions&& options) {
			HANDLE handle = CreateNamedPipe(name.c_str(), options.open_mode, options.pipe_mode,
				options.max_instances, options.out_buffer_size, options.in_buffer_size, options.wait,
				options.protection);
			if (handle == INVALID_HANDLE_VALUE)
				throw OWLPIPEERROR(std::string("Cannot create pipe server with error: ").append(std::to_string(GetLastError())));
			return { handle };
		}
	};
}
