#pragma once

#include "irsol/server/handlers/assignment_frame_rate.hpp"
#include "irsol/server/handlers/assignment_image_size.hpp"
#include "irsol/server/handlers/assignment_input_sequence_length.hpp"
#include "irsol/server/handlers/assignment_integration_time.hpp"
#include "irsol/server/handlers/command_abort.hpp"
#include "irsol/server/handlers/command_gi.hpp"
#include "irsol/server/handlers/command_gis.hpp"
#include "irsol/server/handlers/inquiry_frame_rate.hpp"
#include "irsol/server/handlers/inquiry_image_size.hpp"
#include "irsol/server/handlers/inquiry_input_sequence_length.hpp"
#include "irsol/server/handlers/inquiry_integration_time.hpp"
#include "irsol/server/handlers/lambda_handler.hpp"

/**
 * @defgroup Handlers Handlers
 * @brief Provides the mapping between incoming protocol messages and application logic.
 *
 * The `Handlers` group contains all components responsible for interpreting, validating,
 * and executing actions in response to protocol messages received from clients.
 *
 * Handlers are the core mechanism by which the server translates protocol-level messages
 * (such as assignments, inquiries, and commands) into concrete operations on the application,
 * including camera control, frame acquisition, and session management.
 *
 * This group includes:
 * - Handler base classes and templates for assignment, inquiry, and command messages.
 * - Concrete handler implementations for all supported protocol message types.
 * - LambdaHandler for rapid prototyping or dynamic handler registration.
 * - Factory utilities for handler construction.
 *
 * Handlers are typically registered with the @ref irsol::server::handlers::MessageHandler,
 * which dispatches parsed protocol messages to the appropriate handler based on message type
 * and identifier. This design enables modular, extensible, and testable message processing.
 *
 * @see irsol::protocol
 * @see irsol::server::handlers::MessageHandler
 */

namespace irsol {
namespace server {

/**
 * @namespace irsol::server::handlers
 * @brief Contains all logic for dispatching and implementing protocol message handlers.
 * @ingroup Handlers
 *
 * The `handlers` namespace encapsulates the entire message handling subsystem of the server.
 * It provides:
 * - Handler base classes and templates for different protocol message categories.
 * - Concrete handler classes for each supported protocol message (assignment, inquiry, command).
 * - The LambdaHandler class for inline or dynamic handler logic.
 * - Factories and utilities for handler instantiation and registration.
 *
 * Handlers are responsible for:
 * - Validating incoming protocol messages.
 * - Executing the corresponding application logic (e.g., camera parameter changes, frame requests).
 * - Generating and returning appropriate protocol responses.
 *
 * The namespace acts as the glue between the protocol parsing layer and the per-client
 * application logic, ensuring a clean separation of concerns and facilitating extensibility.
 *
 * @see irsol::server::handlers::MessageHandler
 * @see irsol::protocol
 */
namespace handlers {}
}
}
