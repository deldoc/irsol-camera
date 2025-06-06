# 04-message-handlers {#message_handlers}
@see examples/04-message-handlers/main.cpp

Welcome to the `irsol::server::handlers` subsystem!  
This guide introduces you to the core concepts and practical usage of **handlers** and the **message-handler** system in the irsol framework.

## What are Handlers?

Handlers are the building blocks that connect protocol messages (such as commands, assignments, and inquiries) to your application logic. Each handler is responsible for processing a specific type of protocol message and producing an appropriate response.

Handlers are typically implemented as C++ classes (deriving from a base handler template @ref irsol::server::handlers::internal::HandlerBase), but can also be defined as lambda functions for rapid prototyping or simple use cases.

## What is a MessageHandler?

The @ref irsol::server::handlers::MessageHandler is a central registry and dispatcher. It:

- Receives parsed protocol messages and forwards them to the correct handler.
- Allows you to register, replace, or remove handlers at runtime.

## Typical Workflow

1. **Implement a Handler**  
   Create a handler class (or lambda) that processes a specific protocol message.

2. **Register the Handler**  
   Register your handler with a `MessageHandler` instance, associating it with a message identifier.

3. **Dispatch Messages**  
   When a protocol message is received, the `MessageHandler` looks up the handler for its identifier and invokes it, passing the message and client context.

## Example
@see examples/04-message-handlers/main.cpp
See the `main` file in this folder for a complete example showing:

- How to define a custom handler.
- How to register it with a `MessageHandler`.
- How to dispatch messages and verify handler invocation.

## When to Use `Lambda` Handlers

For simple or one-off logic, you can use a lambda handler instead of a full class. This is especially useful for:

- Prototyping new features.
- Writing test or mock handlers.
- Reducing boilerplate for trivial message processing.


For more details, see the documentation in @ref irsol/server/handlers/base.hpp and @ref irsol/server/handlers/lambda_handler.hpp, and the API reference.
