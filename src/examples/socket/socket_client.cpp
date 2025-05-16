// tcpechotest.cpp
//
// TCP echo timing client.
//
// --------------------------------------------------------------------------
// This file is part of the "sockpp" C++ socket library.
//
// Copyright (c) 2020 Frank Pagliughi
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// --------------------------------------------------------------------------

#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <iostream>
#include <thread>

#include "irsol/irsol.hpp"
#include "sockpp/tcp_connector.h"

// --------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  irsol::initLogging("log/socket-client.log");
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Sample TCP echo client for 'sockpp'");

  std::string server_host = "localhost";
  in_port_t port = sockpp::TEST_PORT;

  std::string client_name = (argc > 1) ? std::string(argv[1]) : "client";
  IRSOL_LOG_INFO("Connecting to server {}:{} as {}", server_host, port, client_name);

  sockpp::initialize();

  std::error_code ec;
  sockpp::tcp_connector conn({server_host, port}, ec);
  if (ec) {
    IRSOL_LOG_ERROR("Error connecting to server at {}:{}: {}", server_host, port, ec.message());
    return 1;
  }

  IRSOL_LOG_INFO("Connected to server");

  // Set a timeout for the responses
  if (auto res = conn.read_timeout(std::chrono::seconds(2)); !res) {
    IRSOL_LOG_ERROR("Error setting timeout on TCP stream: {}", res.error_message());
  }
  IRSOL_LOG_DEBUG("Read timeout set to 2 seconds");

  for (size_t i = 0; i < 1000; ++i) {
    std::string message = "Hello, world from client " + client_name + "! " + std::to_string(i);
    size_t sz = message.length();
    if (conn.write(message) != sz) {
      IRSOL_LOG_ERROR("Error writing to the TCP stream");
      break;
    }

    std::string retrieved_message;
    retrieved_message.resize(message.length());
    if (auto res = conn.read_n(&retrieved_message[0], message.length()); res != sz) {
      IRSOL_LOG_ERROR("Error reading from TCP stream");
      break;
    }

    if (retrieved_message != message) {
      IRSOL_LOG_ERROR("Received message '{}' does not match sent message '{}')", retrieved_message,
                      message);
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}