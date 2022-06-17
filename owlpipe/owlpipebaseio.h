#pragma once

#include <Windows.h>

#include <vector>

#include "owlpipeerrors.h"

namespace owlpipe {
	using owlio_result = struct {
		size_t size;
		bool result;
	};

	class OwlBaseIO {
		HANDLE _handle;
	public:
		OwlBaseIO(HANDLE handle) {
			if (handle == INVALID_HANDLE_VALUE)
				throw OWLPIPEERROR("Handle is invalid");
			_handle = handle;
		}

		HANDLE GetHandle() const {
			return _handle;
		}

		owlio_result Write(const char* const data, const size_t size) const {
			DWORD written;
			const auto result = WriteFile(GetHandle(), data, size, &written, 0);
			return {written, static_cast<bool>(result)};
		}

		owlio_result Read(char* buf, const size_t size) const {
			DWORD read;
			const auto result = ReadFile(GetHandle(), buf, size, &read, 0);
			return {read, static_cast<bool>(result)};
		}

		owlio_result Write(const std::vector<char>& data) const {
			return this->Write(data.data(), data.size());
		}

		owlio_result Read(std::vector<char>& buf, size_t bytes) const {
			return this->Read(buf.data(), bytes);
		}

		std::string ReadToString(size_t size) const {
			std::vector<char> buf(size, 0x0);
			this->Read(buf.data(), size);
			return { buf.begin(), buf.end() };
		}

		owlio_result WriteString(const std::string& str) const {
			return this->Write(str.data(), str.size());
		}

		owlio_result operator<<(const std::vector<char>& data) const {
			return this->Write(data);
		}

		owlio_result operator<<(const std::string& str) const {
			return this->WriteString(str);
		}
	};
}
