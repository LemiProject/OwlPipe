#pragma once

#include <Windows.h>

namespace owlpipe {
	struct OwlPipeClientOptions {
		DWORD open_flags;
		DWORD read_mode;
		DWORD wait_timeout;
	};

	struct OwlPipeClientsListenerOptions {
		DWORD open_mode = PIPE_ACCESS_DUPLEX;
		DWORD in_buffer = 1024;
		DWORD out_buffer = 1024;
	};

	struct OwlPipeServerOptions {
		DWORD open_mode;
		DWORD pipe_mode;
		DWORD max_instances;
		DWORD in_buffer_size;
		DWORD out_buffer_size;
		DWORD wait;
		LPSECURITY_ATTRIBUTES protection;
	};
}
