#include "irsol/server/client/session.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol/parsing/parser.hpp"
#include "irsol/protocol/serialization.hpp"
#include "irsol/server/app.hpp"
#include "irsol/server/message_handler.hpp"
#include "irsol/utils.hpp"

#include <algorithm>
#include <vector>

namespace irsol {
namespace server {

ClientSession::ClientSession(const std::string& id, irsol::types::socket_t&& sock, App& app)
  : m_id(id), m_socket(std::move(sock)), m_app(app)
{}

void
ClientSession::run()
{
  constexpr size_t  INITIAL_BUFFER_SIZE = 1024;
  std::vector<char> buffer(INITIAL_BUFFER_SIZE);
  std::string       messageBuffer;

  IRSOL_NAMED_LOG_DEBUG(m_id, "Client session started running");

  while(true) {
    // Read data from socket
    auto readResult = m_socket.read(buffer.data(), buffer.size());

    // Handle read errors or connection closure
    if(!readResult) {
      IRSOL_NAMED_LOG_ERROR(m_id, "Socket read error: {}", readResult.error().message());
      break;
    }

    size_t bytesRead = readResult.value();
    if(bytesRead == 0) {
      IRSOL_NAMED_LOG_INFO(m_id, "Connection closed by client");
      break;
    }

    // Append received data to message buffer
    messageBuffer.append(buffer.data(), bytesRead);
    IRSOL_NAMED_LOG_TRACE(
      m_id,
      "Read {} bytes: '{}'",
      bytesRead,
      messageBuffer.substr(messageBuffer.size() - bytesRead));

    // Process all complete messages in the buffer
    processInMessageBuffer(messageBuffer);

    // Dynamically resize buffer if it was filled completely
    if(bytesRead == buffer.size()) {
      IRSOL_NAMED_LOG_DEBUG(m_id, "Increasing buffer size to {}", buffer.size());
      buffer.resize(buffer.size() * 2);
    }
  }

  m_socket.close();
  IRSOL_NAMED_LOG_INFO(m_id, "Client session terminated");
}

void
ClientSession::handleOutMessages(std::vector<protocol::OutMessage>&& messages)
{
  IRSOL_NAMED_LOG_DEBUG(m_id, "Serializing {} messages", messages.size());
  for(auto& message : messages) {
    handleOutMessage(std::move(message));
  }
}

void
ClientSession::handleOutMessage(protocol::OutMessage&& message)
{
  IRSOL_NAMED_LOG_DEBUG(m_id, "Serializing message: '{}'", irsol::protocol::toString(message));
  auto serializedMessage = irsol::protocol::Serializer::serialize(std::move(message));
  IRSOL_NAMED_LOG_DEBUG(m_id, "Serialized message: '{}'", serializedMessage.toString());

  handleSerializedMessage(serializedMessage);
}

void
ClientSession::handleSerializedMessage(
  const protocol::internal::SerializedMessage& serializedMessage)
{
  // Send the serialized message to the client
  if(serializedMessage.hasHeader()) {
    send(serializedMessage.header);
  }
  if(serializedMessage.hasPayload()) {
    send(serializedMessage.payload.data(), serializedMessage.payloadSize());
  }
}

void
ClientSession::processInMessageBuffer(std::string& messageBuffer)
{
  size_t newlinePos;

  // Process all complete messages (ending with newline)
  while((newlinePos = messageBuffer.find('\n')) != std::string::npos) {
    // Extract the complete message
    std::string rawMessage = messageBuffer.substr(0, newlinePos);

    // Remove the processed message from the buffer
    messageBuffer.erase(0, newlinePos + 1);

    // Process the extracted message
    processInRawMessage(rawMessage);
  }
}

void
ClientSession::processInRawMessage(const std::string& rawMessage)
{
  IRSOL_NAMED_LOG_DEBUG(m_id, "Processing raw message: '{}'", rawMessage);

  // Strip the prefix 'bypass ' from the message
  std::string processedMessage = utils::stripString(rawMessage, "bypass ");

  // Extract the parsed message
  std::optional<irsol::protocol::InMessage> optionalParsedMessage =
    irsol::protocol::Parser::parse(processedMessage);
  if(!optionalParsedMessage) {
    IRSOL_NAMED_LOG_ERROR(m_id, "Failed to parse message: '{}'", rawMessage);
    // TODO: ability to create error message without identifier for failed parsing
    // handleOutMessage(irsol::protocol::Error());
    return;
  }

  // Extract the InMessage from the optional wrapper and submit it the the message handler
  auto result = app().messageHandler().handle(m_id, std::move(*optionalParsedMessage));
  IRSOL_NAMED_LOG_DEBUG(m_id, "Received {} response(s)", result.size());

  // lock the session's mutex to prevent race conditions
  std::scoped_lock<std::mutex> lock(m_socketMutex);
  for(auto& message : result) {
    handleOutMessage(std::move(message));
  }
  IRSOL_NAMED_LOG_DEBUG(m_id, "Sent {} response(s) to client", result.size());
}

void
ClientSession::send(const std::string& msg)
{
  std::string preparedMessage = msg;

  IRSOL_NAMED_LOG_TRACE(m_id, "Sending message of size {}", preparedMessage.size());
  if(!m_socket) {
    IRSOL_NAMED_LOG_ERROR(
      m_id, "Socket seems to be closed, this is unexpected. Ignoring send request.");
    return;
  }

  auto result = m_socket.write(preparedMessage);
  if(!result) {
    IRSOL_NAMED_LOG_ERROR(m_id, "Failed to send message: {}", result.error().message());
  } else if(result.value() != preparedMessage.size()) {
    IRSOL_NAMED_LOG_WARN(
      m_id, "Incomplete message sent: {} of {} bytes", result.value(), preparedMessage.size());
  }
}

void
ClientSession::send(const irsol::types::byte_t* const data, size_t size)
{
  IRSOL_NAMED_LOG_TRACE(m_id, "Sending binary data of size {}", size);
  if(!m_socket) {
    IRSOL_NAMED_LOG_ERROR(
      m_id, "Socket seems to be closed, this is unexpected. Ignoring send request.");
    return;
  }

  auto result = m_socket.write_n(data, size);
  if(!result) {
    IRSOL_NAMED_LOG_ERROR(m_id, "Failed to send binary data: {}", result.error().message());
  } else if(result.value() != size) {
    IRSOL_NAMED_LOG_WARN(m_id, "Incomplete binary data sent: {} of {} bytes", result.value(), size);
  }
}
}  // namespace server
}  // namespace irsol