#include "pch.h"

#include <thread>
#include <chrono>
#include <string>

#define PIPE_NAME "\\\\.\\pipe\\owltestpipe"
#define PIPE_BUFFER_SIZE 1024

#include "../owlpipe/owlpipe.h"

HANDLE CreateJustWaitingPipeServer() {
	HANDLE handle = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1,
		PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE,
		NMPWAIT_USE_DEFAULT_WAIT,
		0);

	std::thread([&]() {
		if (ConnectNamedPipe(handle, 0)) {
			
		}
	}).detach();

	return handle;
}

HANDLE CreateReceivingPipeServer(const std::string& data) {
	HANDLE handle = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE,
		NMPWAIT_USE_DEFAULT_WAIT,
		0);

	std::thread([](HANDLE handle, const std::string& data) {
		while (handle != INVALID_HANDLE_VALUE) {
			if (ConnectNamedPipe(handle, 0) || GetLastError() == ERROR_PIPE_CONNECTED) {
				DWORD written;
				WriteFile(handle, data.c_str(), data.size() + 1, &written, 0);
				break;
			}
		}
	}, handle, data).detach();

	return handle;
}

TEST(OwlClientPipeTests, ClientConnectTest) {
	auto h = CreateJustWaitingPipeServer();

	auto cli = owlpipe::OwlPipeClient::Connect(PIPE_NAME);
	ASSERT_TRUE(cli.IsConnected());

	DisconnectNamedPipe(h);
	CloseHandle(h);
	cli.Close();
}

TEST(OwlClientPipeTests, ClientConnectAndReadTest) {
	auto h = CreateReceivingPipeServer("OK");

	auto cli = owlpipe::OwlPipeClient::Connect(PIPE_NAME);

	char data[3] = {};
	auto bytes = cli.Read(data, 3);
	ASSERT_EQ(bytes.size, 3);
	ASSERT_STREQ(data, "OK");

	DisconnectNamedPipe(h);
	CloseHandle(h);
	cli.Close();
}

char GetRandomChar() {
	static char c = 'A' + rand() % 24;
	return c;
}

TEST(OwlClientPipeTests, ClientConnectAndReadBigStringTest) {
	std::string s;
	for (auto i = 0; i <= PIPE_BUFFER_SIZE - 1; ++i)
		s.push_back(GetRandomChar());

	const auto h = CreateReceivingPipeServer(s);

	auto cli = owlpipe::OwlPipeClient::Connect(PIPE_NAME);
	const auto string_from_file = cli.ReadToString(PIPE_BUFFER_SIZE);

	ASSERT_EQ(string_from_file, s);

	DisconnectNamedPipe(h);
	CloseHandle(h);
	cli.Close();
}

TEST(OwlServerPipeTests, CreatePipeServerTest) {
	const owlpipe::OwlClientsListener listener(owlpipe::OwlPipeClientsListenerOptions { PIPE_ACCESS_DUPLEX, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE });

	const auto server = listener.CreateServer(PIPE_NAME);

	ASSERT_TRUE(server.GetHandle() != INVALID_HANDLE_VALUE);

	server.Close();
}

TEST(OwlServerPipeTests, CreateAndConnectClientTest) {
	const owlpipe::OwlClientsListener listener(owlpipe::OwlPipeClientsListenerOptions{ PIPE_ACCESS_DUPLEX, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE });
	const auto server = listener.CreateServer(PIPE_NAME);
	ASSERT_TRUE(server.GetHandle() != INVALID_HANDLE_VALUE);

	bool is_connected = false;

	std::thread([&]() {
		while (server.GetHandle() != INVALID_HANDLE_VALUE) {
			if (server.ConnectClient() || GetLastError() == ERROR_PIPE_CONNECTED) {
				is_connected = true;
				server.DisconnectClient();
				break;
			}
		}
	}).detach();

	auto cli = owlpipe::OwlPipeClient::Connect(PIPE_NAME);
	ASSERT_TRUE(cli.IsConnected());

	std::this_thread::sleep_for(std::chrono::seconds(3));

	cli.Close();
	server.Close();

	ASSERT_TRUE(is_connected);
}

TEST(OwlPipeActionsTests, WriteTest) {
	const auto file = CreateFile("TESTFILEWRITE", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	auto srv = owlpipe::OwlPipeServer(file);

	const char data[3] = { 'O', 'K', '\n' };
	ASSERT_TRUE(srv.Write(data, 3).result != false);
	ASSERT_TRUE(srv.WriteString("OK\n").result != false);

	const std::vector vec_data = { 'O', 'K', '\n' };
	ASSERT_TRUE(srv.Write(vec_data).result != false);

	ASSERT_TRUE((srv << "OK\n").result != false);
	ASSERT_TRUE((srv << vec_data).result != false);

	srv.Close();
}


TEST(OwlPipeActionsTests, ReadTest) {
	constexpr auto prepare_file = []() {
		auto f = new FILE();
		fopen_s(&f, "TESTFILEREAD", "w");
		fwrite("TESTTEST", sizeof(char), 8, f);
		fclose(f);
	};

	prepare_file();
	const auto file = CreateFile("TESTFILEREAD", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);

	auto srv = owlpipe::OwlPipeServer(file);

	constexpr auto str_size = 4;
	char buf[str_size + 1] = {};

	ASSERT_TRUE(srv.Read(buf, str_size).result);
	ASSERT_STREQ(buf, "TEST");

	ASSERT_EQ(srv.ReadToString(str_size), "TEST");

	srv.Close();
}