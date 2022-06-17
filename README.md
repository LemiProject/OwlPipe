# OwlPipe
# NamedPipe Wrapper
Simple Wrapper for NamedPipe in C++. The goal is to provide a simple way for programs to use local communications.

# Example

Server:
```c++
#include <owlpipe.h>

#define PIPE_NAME "\\\\.\\pipe\\owltestpipe"
#define PIPE_BUFFER_SIZE 1024

using namespace owlpipe;

void thread_for_client(OwlPipeServer server) {
  printf("New message: %s", server.ReadToString(PIPE_BUFFER_SIZE));
  server.DisconnectClient();
  server.Close();
}

int main() {
  const OwlClientsListener listener(OwlPipeClientsListenerOptions {PIPE_ACCESS_DUPLEX, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE}); // Create server builder
  const auto server = listener.CreateServer(PIPE_NAME); // Create server instance
  
  while (server.GetHandle() != INVALID_HANDLE_VALUE) { // While server is valid
    if (server.ConnectClient() || GetLastError() == ERROR_PIPE_CONNECTED) { // If new connection
      std::thread(thread_for_client, server).detach(); // Create new thread for client
      
      server = listener.CreateServer(PIPE_NAME);  // Reset server instance for new client connection
    }
  }
}
```

Client:
```c++
#include <owlpipe.h>

#define PIPE_NAME "\\\\.\\pipe\\owltestpipe"
#define PIPE_BUFFER_SIZE 1024

using namespace owlpipe;

constexpr auto message = "Hello, Server)";

int main() {
  auto client = OwlPipeClient::Connect(PIPE_NAME); // Connect to server
  client.Write(message, strlen(message)); // Write message to server
  client.Close(); // Close client handle
}
```
