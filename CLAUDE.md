# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

OOSMOS (Object-Oriented State Machine Operating System) is a framework for building hierarchical state machines in embedded C. It bridges visual UML design and production code through automatic code generation from UMLet diagrams. The core framework is compact (~2,600 lines across two files) yet powers complex embedded systems through hierarchical state machines, event-driven architecture, and built-in threading capabilities.

## Common Development Commands

### Building Components

**Classes library:**
```bash
cd Classes
python bld.py
```

**All tests (Linux):**
```bash
cd Tests
python bld.py
```

**Individual test (Linux):**
```bash
cd Tests
gcc -I../Source -I../Classes -I../Classes/Tests -I. -std=c99 -Wall -Wno-overflow -Wno-unused-parameter -pedantic -Werror -Wshadow -flto -o TestName -D_POSIX_C_SOURCE=199309 -D__linux__ -Doosmos_DEBUG -Doosmos_ORTHO TestName.c ../Source/oosmos.c
./TestName
```

**Example projects:**
```bash
# Linux
cd Examples/Basic/Linux
python bld.py

# Windows
cd Examples/Basic/Windows
python bld.py
```

### Code Generation

Generate C code from UML state machine diagrams (UXF files):
```bash
# From Classes directory (with classes.json)
..\Gen\gen.exe classes.json

# From test directory (with tests.json)
..\Gen\gen.exe tests.json

# From example directory (with basic.json)
..\..\..\Gen\gen.exe basic.json
```

### Cleaning Build Artifacts

```bash
# Global clean (repository root)
python clean.py

# Platform-specific clean
cd Examples/Basic/Windows
python clean.py
```

### Initial Setup

After cloning, populate example directories:
```bash
python populate.py
```

## Code Architecture

### Core Framework Structure

The framework consists of three main components:

1. **Core Engine** (`Source/oosmos.h` ~765 lines, `Source/oosmos.c` ~1896 lines): The state machine runtime engine. Provides macros and functions for hierarchical state machines, event queuing, timing, and threading primitives.
2. **Code Generator** (`Gen/gen.exe`): Converts UML state diagrams (UXF files) to C code with special markers for custom logic integration
3. **Class Library** (`Classes/`): Hardware abstractions and utilities built on the core engine (pin, btn, sw, matrix, keyer, socket, DNS, etc.)

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

The build system uses Python scripts for cross-platform compilation:

**`oosmos.py`** provides compiler abstractions:
- **`oosmos.cWindows.Compile(oosmos_dir, FileArray, Options)`**: Compiles with Visual Studio (cl.exe), auto-generates code, sets up includes
- **`oosmos.cLinux.Compile(oosmos_dir, Target, FileArray, Options)`**: Compiles with GCC, auto-generates code, sets up includes

**Key compiler flags:**
- **Windows**: `-I` paths for Source/ and Classes/, `-Doosmos_DEBUG`, `-Doosmos_ORTHO` (for orthogonal regions), `-D_CRT_SECURE_NO_WARNINGS`
- **Linux**: `-std=c99 -Wall -Werror -Wshadow -flto`, `-D_POSIX_C_SOURCE=199309 -D__linux__ -Doosmos_DEBUG -Doosmos_ORTHO`

**Build process in each `bld.py`:**
1. Run code generator: `gen.exe <config>.json` (Windows-specific)
2. Call `oosmos.cWindows.Compile()` or `oosmos.cLinux.Compile()`
3. Compiler outputs executable in current directory

## File Organization

- **`Source/`**: Core OOSMOS framework (oosmos.h, oosmos.c)
- **`Classes/`**: Reusable class library with hardware abstractions
- **`Examples/`**: Platform-specific example projects
- **`Tests/`**: Unit tests and framework validation
- **`Gen/`**: Code generator executable and support files
- **`LINT/`**: PC-LINT configuration for static analysis
- **`UMLetPalette/`**: UMLet element palette for OOSMOS diagrams

## Important Notes

**Initialization & Setup:**
- Always run `python populate.py` after cloning to set up embedded platform example directories

**Code Generation Workflow:**
- Never modify code between `//>>>` and `//<<<` markers - code generator will overwrite these sections
- Code generator configuration is specified in JSON files (classes.json, tests.json, basic.json, etc.)
- Generated code includes event enums and state handler functions with these markers

**Building:**
- Use provided `bld.py` Python scripts rather than direct compiler invocation
- Scripts automatically run code generator and set up compiler includes/flags
- Use `clean.py` to remove generated files before re-generating

**State Machine Integration:**
- State machines must be instantiated with `*New()` functions
- Register instances with the main event loop using `oosmos_RunStateMachines()`
- Event queues are per-object, sized via queue_size in JSON config

**Timing:**
- All timing in OOSMOS uses microsecond precision internally
- Use provided conversion macros: `oosmos_MS(1000)`, `oosmos_SEC(1)`, etc.
- Polling-based state machines use `oosmos_POLL` event for continuous operation