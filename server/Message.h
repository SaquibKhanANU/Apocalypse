#include <cstring>

enum class MessageType : uint8_t {
    CLIENT_INFO,
};

struct Message {
    MessageType messageType;
    uint32_t dataSize;
    char data[1024]; 
};