#pragma once

#include <Windows.h>

#include "owlpipeoptions.h"
#include "owlpipeserver.h"

namespace owlpipe {
	class OwlClientsListener {
		OwlPipeClientsListenerOptions _options;
	public:
		OwlClientsListener(OwlPipeClientsListenerOptions&& opt = {}) {
			_options = opt;
		}

		void SetOpenMode(DWORD mode) {
			_options.open_mode = mode;
		}

		void SetInBuffer(DWORD size) {
			_options.in_buffer = size;
		}

		void SetOutBuffer(DWORD size) {
			_options.out_buffer = size;
		}

		OwlPipeServer CreateServer(const std::string& name) const {
			return OwlPipeServer::CreateServer(name, OwlPipeServerOptions{ _options.open_mode,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
				_options.in_buffer, _options.out_buffer, NMPWAIT_USE_DEFAULT_WAIT, 0}
			);
		}

		std::vector<OwlPipeServer> CreateServers(const std::string& name, size_t count) const {
			std::vector<OwlPipeServer> servers;
			servers.reserve(count);
			for (auto i = 0; i < count; ++i) 
				servers.push_back(CreateServer(name));
			return servers;
		}
	};
}
