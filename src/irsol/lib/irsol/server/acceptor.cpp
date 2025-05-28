#include "irsol/server/acceptor.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/server/client/session.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace server {
namespace internal {
ClientSessionAcceptor::ClientSessionAcceptor(
  irsol::types::port_t  port,
  OnNewClientCallback_t onNewClientCallback)
  : m_port(port), m_onNewClientCallback(onNewClientCallback)
{
  m_isOpen = m_acceptor.open(irsol::types::inet_address_t(m_port));
}

std::string
ClientSessionAcceptor::error() const
{
  IRSOL_ASSERT_ERROR(!isOpen(), "Acceptor is not in error mode");
  return m_isOpen.error().message();
}

bool
ClientSessionAcceptor::isOpen() const
{
  return bool(m_isOpen);
}

void
ClientSessionAcceptor::stop()
{
  if(!m_running) {
    IRSOL_LOG_DEBUG("Accept loop already stopped");
    return;
  }
  m_running.store(false);
  m_acceptor.close();
}

void
ClientSessionAcceptor::run()
{
  if(!isOpen()) {
    IRSOL_LOG_ERROR("Failed to open acceptor on port {}: {}", m_port, error());
    throw std::runtime_error("Failed to open acceptor");
  }
  if(m_running) {
    IRSOL_LOG_ERROR("Accept loop already running");
    return;
  }
  m_running.store(true);

  IRSOL_LOG_INFO("Accept loop started");
  while(m_running) {

    // Set non-blocking mode to avoid blocking indefinitely on accept
    m_acceptor.set_non_blocking(true);

    auto sockResult = m_acceptor.accept();

    // Check if we should exit the loop
    if(!m_running) {
      IRSOL_LOG_DEBUG("Accept loop stopped");
      break;
    }

    if(!sockResult) {
      if(m_running) {
        sockpp::error_code err{sockResult.error()};
        // These errors are expected in non-blocking mode when no connection is available
        bool isExpectedError =
          (err == std::errc::resource_unavailable_try_again ||
           err == std::errc::operation_would_block || err == std::errc::timed_out);

        if(isExpectedError) {
          // Sleep for a short time to avoid busy waiting when no connections are available
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
          // Log unexpected errors
          IRSOL_LOG_WARN("Failed to accept connection: {}", sockResult.error_message());
        }
      }
      continue;
    }

    // Generate a unique ID for this client session
    irsol::types::client_id_t clientId = irsol::utils::uuid();
    IRSOL_LOG_DEBUG("Generated client ID: {}", clientId);
    m_onNewClientCallback(clientId, sockResult.release());
  }
  IRSOL_LOG_INFO("Accept loop ended");
}
}
}
}