# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OOSMOS (Object-Oriented State Machine Operating System) is a framework for building hierarchical state machines in embedded C. It bridges visual UML design and production code through automatic code generation from UMLet diagrams.

## Common Development Commands

### Building Components

**Classes (Core Classes):**
```bash
cd Classes
python bld.py
```

**Tests:**
```bash
cd Tests
python bld.py
```

**Examples (Platform-specific):**
```bash
# Windows examples
cd Examples/Basic/Windows
python bld.py

# Linux examples  
cd Examples/Basic/Linux
python bld.py
```

### Code Generation

**Generate state machine code from UXF files:**
```bash
# From Classes directory
..\Gen\gen.exe classes.json

# From specific example directory
..\..\..\Gen\gen.exe basic.json
```

### Cleaning Build Artifacts

**Global clean (removes all generated files):**
```bash
python clean.py
```

**Platform-specific clean:**
```bash
cd Examples/Basic/Windows
python clean.py
```

### Initial Setup

**Populate example directories after cloning:**
```bash
python populate.py
```

## Code Architecture

### Core Framework Structure

The framework consists of three main components:

1. **Core Engine** (`Source/oosmos.h`, `Source/oosmos.c`): The state machine runtime engine (~1700 lines)
2. **Code Generator** (`Gen/gen.exe`): Converts UXF diagrams to C code  
3. **Class Library** (`Classes/`): Hardware abstraction and utility classes

### State Machine Object Structure

All OOSMOS objects follow this pattern:

```c
// Header - public interface only
typedef struct classTag class;
extern class * classNew(params...);

// Implementation - private structure  
struct classTag {
  // State machine with event queue
  oosmos_sStateMachine(ROOT, EventType, QueueSize);
  // OR without event queue
  oosmos_sStateMachineNoQueue(ROOT);
  
  // State declarations
  oosmos_sComposite ParentState;
  oosmos_sLeaf      ChildState;
  
  // Private data
};
```

### Execution Models

OOSMOS supports three execution contexts:

1. **State Machines**: Event-driven hierarchical state machines
2. **Active Objects**: Simple run() functions called each cycle
3. **Object Threads**: Cooperative threads with blocking operations

### Code Generation Workflow

1. **Design**: Create UML state diagrams in UMLet (.uxf files)
2. **Configure**: Create JSON files specifying which UXF files to process
3. **Generate**: Run `gen.exe` to produce C code with special markers:
   ```c
   //>>>EVENTS
   // Generated event enums
   //<<<EVENTS
   
   //>>>CODE  
   // Generated state functions
   //<<<CODE
   ```
4. **Implement**: Add custom logic between markers (never modify generated sections)
5. **Compile**: Use provided Python build scripts

### Key Classes

- **`pin`**: GPIO abstraction with debouncing state machine
- **`btn`**: Button handling built on pin class
- **`sw`**: Switch class with press/release events
- **`toggle`**: Periodic pin toggling using object threads
- **`matrix`**: LED matrix display driver with state machine
- **`keyer`**: Morse code keyer implementation
- **`sock`**: Socket networking abstraction
- **`dns`**: DNS resolution utilities

## Platform Support

OOSMOS supports multiple platforms through conditional compilation:

- **Windows**: Visual Studio compiler (cl.exe)
- **Linux**: GCC with specific flags for embedded constraints
- **Arduino**: Arduino IDE compilation
- **PIC32**: MPLAB X IDE
- **STM32**: IAR Embedded Workbench
- **ESP32/ESP8266**: Arduino framework
- **mbed**: ARM mbed compiler

## Memory Management

- Uses **static allocation only** - no malloc/free
- Objects allocated from compile-time pools using `oosmos_Allocate()`
- Predictable memory usage suitable for safety-critical systems
- Object pools configured at compile time

## Development Patterns

### Creating New Classes

1. Create header with public interface only
2. Implement with private structure containing OOSMOS objects
3. Use code generation markers if implementing state machines
4. Follow naming convention: `classMethodName(class * pInstance, ...)`

### State Machine Implementation

1. Design state machine in UMLet
2. Export as .uxf file
3. Add to JSON configuration file
4. Run code generator
5. Implement action functions and custom logic
6. Never modify generated code sections

### Event Handling

- Events can carry payload data through unions
- Built-in events: `ENTER`, `EXIT`, `POLL`, `TIMEOUT`, `COMPLETE`
- Custom events defined in generated code
- Events processed hierarchically up state tree

### Threading

Use state threads for sequential logic within states:
```c
case oosmos_POLL:
  oosmos_ThreadBegin();
    oosmos_ThreadDelayMS(1000);
    // More sequential code
  oosmos_ThreadEnd();
```

## Build System Details

The build system uses Python scripts (`oosmos.py`) that provide:

- **`oosmos.cWindows.Compile()`**: Windows compilation with Visual Studio
- **`oosmos.cLinux.Compile()`**: Linux compilation with GCC
- Automatic include path setup for Source/ and Classes/ directories
- Debug builds with symbol information enabled
- Warning configuration optimized for embedded development

## File Organization

- **`Source/`**: Core OOSMOS framework (oosmos.h, oosmos.c)
- **`Classes/`**: Reusable class library with hardware abstractions
- **`Examples/`**: Platform-specific example projects
- **`Tests/`**: Unit tests and framework validation
- **`Gen/`**: Code generator executable and support files
- **`LINT/`**: PC-LINT configuration for static analysis
- **`UMLetPalette/`**: UMLet element palette for OOSMOS diagrams

## Important Notes

- Always run `populate.py` after cloning to set up example directories
- Never modify code between `//>>>` and `//<<<` markers - it will be overwritten
- Use provided Python build scripts rather than direct compiler invocation
- State machines must be registered with `oosmos_RunStateMachines()` main loop
- All timing uses microsecond precision - use conversion macros for other units