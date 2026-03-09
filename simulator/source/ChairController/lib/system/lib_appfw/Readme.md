# Event-Driven Application Framework for Arduino/ESP32

This framework provides a structured, event-driven architecture to help you build clean and modular applications on Arduino and ESP32 platforms. By enforcing a clear separation of concerns, the framework makes it easy to attach and reuse application components. Components, derived from **AppComponent**, are integrated into the main application and receive events through the framework, allowing you to build event-driven applications with minimal hassle.

## Table of Contents

- [Overview](#overview)
- [Key Components](#key-components)
- [Getting Started](#getting-started)
  - [Installation](#installation)
    - [PlatformIO](#platformio)
    - [Manual Installation](#manual-installation)
- [Usage](#usage)
  - [Application Structure](#application-structure)
  - [Event Handling](#event-handling)
  - [Dispatching Events](#dispatching-events)
  - [Logging](#logging)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [Support](#support)
- [License](#license)

## Overview

The goal of this framework is to encourage clean coding practices and allow developers to build applications in a modular, event-driven manner on Arduino or ESP32 devices. Instead of lumping all the logic into the traditional Arduino `setup` and `loop` functions, this framework delegates the responsibilities as follows:

- The **Application** class serves as the main entry point that abstracts the Arduino launcher.
- Components are created as subclasses of **AppComponent** and attached to the Application.
- The framework manages an event system where components receive asynchronous events (such as connection changes, sensor updates, or custom events) via a well-defined API.

## Key Components

- **Application**  
  The main class that initializes and processes application events. It wraps the platform-specific launcher; for example, the Arduino `setup` function instantiates and initializes the Application, while the `loop` function calls `ProcessEvents`.

- **AppComponent**  
  A base class for all components used in the application. Each component can subscribe to or generate events, enabling modularity and separation of concerns.

- **AppEvent & AppExecEvent**  
  These classes define the event types processed by the framework. While `AppEvent` provides the basic event structure, `AppExecEvent` is used for executing specific tasks, such as triggering periodic actions.

- **AppEventTarget**  
  An abstract class that defines the interface for event handlers. Components and the Application itself implement this interface to process events.

- **AppEventHub**  
  A central dispatcher that receives events and forwards them to attached event targets. It ensures that components only receive the events relevant to them.

- **AppEventPoster**  
  Manages event dispatching using a queue, enabling asynchronous event processing. It also provides a static `Post` method to dispatch execution events (e.g., `AppExecEvent`).

- **AppLogger**  
  A simple logging utility that provides macros for debug logging. When enabled, it helps in tracing event flows and debugging component behavior.

## Getting Started

### Installation

This library is designed being used in PlatformIO driven project.

#### PlatformIO

Include the library in attribute `lib_deps` of your PlatformIO project.

#### Manual Installation

1. **Download the Library**  
   Clone or download the repository as well as local libraries this library depends on (see section _Dependencies_).

2. **Include in Your Project**  
   Add the library files to your project. Make sure your development environment is set up accordingly.

## Usage

### Application Structure

The framework centers around the **Application** class, which abstracts platform-specific details. In an Arduino context, your `setup` function instantiates and initializes your concrete Application, while the `loop` function continually processes events.

Example of a simple application:
```cpp
#include <AppLogger.h>
#include <Application.h>
#include <AppComponent.h>

// Custom component derived from AppComponent.
class MyComponent : public AppFw::AppComponent {
public:
    // Override event processing if necessary.
    virtual void OnEvent(AppFw::AppEventPtr ev) override {
        // Process events specific to this component.
    }
};

// Concrete application class.
class MyApplication : public AppFw::Application<MyApplication> {
public:
    void Init() override {
        // Initialize your application, attach components, etc.
        // Example: Attach a custom component.
        Attach(std::make_shared<MyComponent>());
        AppLogDbg("MyApplication initialized.");
    }
};

void setup() {
    // Instantiate and initialize the concrete application.
    MyApplication::Instance().Init();
}

void loop() {
    // Process queued events
    MyApplication::Instance().ProcessEvents();

    // Optionally, dispatch an execution event
    // AppFw::AppEventPoster::Post(std::make_shared<AppFw::AppExecEvent>());
}
```

### Event Handling

The framework employs an event-driven paradigm to decouple component logic:
- **Creating Events:**  
  You can create custom events by subclassing **AppEvent**.
- **Handling Events:**  
  Override the `OnEvent(AppEventPtr ev)` method in your component or Application. This method processes incoming events.
- **Dispatching Events:**  
  Use the **AppEventPoster** for dispatching events. For example, you can trigger an `AppExecEvent`:
  ```cpp
  AppFw::AppEventPoster::Post(std::make_shared<AppFw::AppExecEvent>());
  ```

This architecture promotes modularity by allowing each component to focus on its own responsibilities without tight coupling to other parts of the system.

### Dispatching Events

The **AppEventHub** and **AppEventPoster** work together to distribute events:
- **AppEventHub:**  
  Forwards events to all attached components that implement the **AppEventTarget** interface.
- **AppEventPoster:**  
  Enqueues events for asynchronous processing. This setup makes it easy to dispatch and process events without blocking the main execution thread.

### Logging

Debug logging is built into the framework via **AppLogger**. Enable it by defining `LOG_APP_DBG_ENABLED` in your build configuration. Use the provided macros to print debug messages:
```cpp
#define LOG_APP_DBG_ENABLED
#include <AppLogger.h>

// Debug log example:
AppLogDbg("Dispatching event: %d", eventId);
```

Logging helps in monitoring application state and troubleshooting event flows.

## Dependencies

This library depends on `lib_utils`.

## Contributing

Contributions to improve features, documentation, and bug fixes are welcome. Please adhere to the repository’s contribution guidelines when submitting pull requests or reporting issues.

## Support

If you encounter any problems or have feature requests, please consult the issue tracker in the repository.

## License

This library is licensed under the [MIT License](LICENSE).  
(c) HTL Leonding. All trademarks used in this document are property of their respective owners.
