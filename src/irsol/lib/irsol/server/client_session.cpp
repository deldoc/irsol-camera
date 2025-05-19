#include "irsol/server/client_session.hpp"
#include "irsol/logging.hpp"
#include "irsol/server/command_processor.hpp"
#include "irsol/utils.hpp"
#include <algorithm>
#include <vector>

namespace irsol {

ClientSession::ClientSession(sockpp::tcp_socket &&sock, ServerApp &app)
    : m_sock(std::move(sock)), m_app(app) {}

void ClientSession::run() {
  constexpr size_t INITIAL_BUFFER_SIZE = 1024;
  std::vector<char> buffer(INITIAL_BUFFER_SIZE);
  std::string messageBuffer;

  IRSOL_LOG_DEBUG("Client session started running");

  while (true) {
    // Read data from socket
    sockpp::result<size_t> readResult = m_sock.read(buffer.data(), buffer.size());

    // Handle read errors or connection closure
    if (!readResult) {
      IRSOL_LOG_ERROR("Socket read error: {}", readResult.error().message());
      break;
    }

    size_t bytesRead = readResult.value();
    if (bytesRead == 0) {
      IRSOL_LOG_INFO("Connection closed by client");
      break;
    }

    // Append received data to message buffer
    messageBuffer.append(buffer.data(), bytesRead);
    IRSOL_LOG_TRACE("Read {} bytes: '{}'", bytesRead,
                    messageBuffer.substr(messageBuffer.size() - bytesRead));

    // Process all complete messages in the buffer
    processMessageBuffer(messageBuffer);

    // Dynamically resize buffer if it was filled completely
    if (bytesRead == buffer.size()) {
      IRSOL_LOG_DEBUG("Increasing buffer size to {}", buffer.size());
      buffer.resize(buffer.size() * 2);
    }
  }

  m_sock.close();
  IRSOL_LOG_INFO("Client session terminated");
}

void ClientSession::processMessageBuffer(std::string &messageBuffer) {
  size_t newlinePos;

  // Process all complete messages (ending with newline)
  while ((newlinePos = messageBuffer.find('\n')) != std::string::npos) {
    // Extract the complete message
    std::string rawMessage = messageBuffer.substr(0, newlinePos);

    // Remove the processed message from the buffer
    messageBuffer.erase(0, newlinePos + 1);

    // Process the extracted message
    processRawMessage(rawMessage);
  }
}

void ClientSession::processRawMessage(const std::string &rawMessage) {
  IRSOL_LOG_DEBUG("Processing raw message: '{}'", rawMessage);

  // Strip whitespace and check if message is empty
  std::string strippedMessage = utils::strip(rawMessage);
  if (strippedMessage.empty()) {
    IRSOL_LOG_WARN("Empty message received, ignoring");
    return;
  }

  // Strip the prefix 'bypass ' from the message
  strippedMessage = utils::stripString(strippedMessage, "bypass ");

  // Process the message and get response
  std::vector<CommandResponse> responses;
  if (strippedMessage.back() == '?') {
    strippedMessage = utils::strip(strippedMessage, "?");
    responses = CommandProcessor::handleQuery(strippedMessage, *this);
    IRSOL_LOG_DEBUG("Query processed: '{}'", strippedMessage);
  } else {
    responses = CommandProcessor::handleCommand(strippedMessage, *this);
    IRSOL_LOG_DEBUG("Command processed: '{}'", strippedMessage);
  }

  if (responses.empty()) {
    IRSOL_LOG_DEBUG("No responses for: '{}'", strippedMessage);
    return;
  }
  for (const auto &response : responses) {
    // Send response if available
    if (!response.message.empty()) {
      IRSOL_LOG_DEBUG("Sending response: '{}'", response.message);
      send(response.message);
    } else {
      IRSOL_LOG_DEBUG("No response for: '{}'", strippedMessage);
    }

    // Send broadcast message if available
    if (!response.broadcastMessage.empty()) {
      IRSOL_LOG_DEBUG("Broadcasting message: '{}'", response.broadcastMessage);
      m_app.broadcast(response.broadcastMessage);
    }
  }
}

void ClientSession::send(const std::string &msg) {
  std::string preparedMessage = msg;
  if (preparedMessage.back() != '\n') {
    preparedMessage += '\n';
  }

  std::lock_guard<std::mutex> lock(m_sendMutex);
  IRSOL_LOG_TRACE("Sending message of size {}", preparedMessage.size());

  auto result = m_sock.write(preparedMessage);
  if (!result) {
    IRSOL_LOG_ERROR("Failed to send message: {}", result.error().message());
  } else if (result.value() != preparedMessage.size()) {
    IRSOL_LOG_WARN("Incomplete message sent: {} of {} bytes", result.value(),
                   preparedMessage.size());
  }
}

} // namespace irsol