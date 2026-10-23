# HarbourOS

**HarbourOS** is a small, educational, 32-bit x86 operating-system kernel built from scratch. The project explores how a computer starts a kernel, how a kernel writes directly to text-mode video memory, and how basic keyboard input can be turned into a command-line shell.

> **Project status:** Early prototype. The bootable kernel, VGA text output, polled keyboard input, basic shell commands, Backspace handling, and screen scrolling have been implemented and tested in QEMU. Features listed under the roadmap are planned work, not completed functionality.

## Table of Contents

- [Overview](#overview)
- [Current Features](#current-features)
- [Technology Stack](#technology-stack)
- [System Design](#system-design)
- [Repository Structure](#repository-structure)
- [Build and Run](#build-and-run)
- [Using the Shell](#using-the-shell)
- [Current Limitations](#current-limitations)
- [Roadmap](#roadmap)
- [Testing](#testing)
- [Contributing](#contributing)
- [Disclaimer](#disclaimer)

## Overview

Most application programs rely on an operating system to provide services such as input/output, memory management, and process execution. HarbourOS is a learning project that starts below the application layer: it builds a small kernel that can be booted by GRUB and run in an emulator.

The current prototype uses the Multiboot boot protocol, a linker script to place kernel sections in memory, VGA text memory for console output, and polling of the legacy PS/2 keyboard controller for input. A small command loop provides a basic interactive shell.

## Current Features

- **Bootable kernel:** GRUB loads the kernel using the Multiboot protocol.
- **Freestanding C++ kernel:** Kernel code is compiled without a hosted standard-library/runtime environment.
- **VGA text console:** Writes characters directly to memory at `0xB8000` in 80 × 25 text mode.
- **Screen clearing:** Clears the console and resets the cursor.
- **Keyboard input:** Polls the legacy keyboard controller's status and data ports.
- **Basic shell:** Accepts typed commands and displays command responses.
- **Built-in commands:**
  - `help` — displays the available commands.
  - `about` — displays a short description of HarbourOS.
  - `clear` — clears the display and restores the shell heading.
- **Line editing:** Backspace removes the most recently typed character when the input buffer is not empty.
- **Scrolling:** Moves console contents up and clears the bottom row when output reaches the bottom of the display.
- **Bounded command buffer:** Stores up to 63 input characters plus a null terminator.

## Technology Stack

| Component | Purpose |
|---|---|
| C++ | Kernel logic and shell |
| x86 assembly | Entry point and initial stack setup |
| Clang / Clang++ | Assemble and compile the freestanding kernel |
| `ld.lld` | Link the kernel using `linker.ld` |
| GRUB / Multiboot | Load the kernel |
| `grub-mkrescue` and xorriso | Create a bootable ISO image |
| QEMU | Run and test the OS in an emulator |
| Ubuntu on WSL | Development environment |

## System Design

At a high level, the current boot and input/output flow is:

```text
Computer / QEMU
      |
      v
GRUB bootloader
      |
      | Multiboot loads kernel
      v
boot/boot.s
      |
      | Sets up stack and calls kernel_main()
      v
kernel/kernel.cpp
      |
      +--> VGA text output at 0xB8000
      |
      +--> Polls keyboard controller ports
      |
      +--> Reads and edits command buffer
      |
      +--> Executes built-in shell commands
```

### Important implementation details

- The kernel entry point is `_start` in `boot/boot.s`.
- The entry point sets up a 16 KiB stack and calls `kernel_main()`.
- `linker.ld` places the kernel at the 1 MiB physical-memory address and retains the Multiboot header.
- The console uses 80 columns × 25 rows, with each VGA text cell represented by a character and an attribute byte.
- Keyboard input currently uses polling rather than hardware interrupts.
- The shell uses a fixed-size character array for command input.

## Repository Structure

```text
harbour-os/
├── boot/
│   └── boot.s                 # Kernel entry point and initial stack
├── include/                   # Reserved for shared headers
├── kernel/
│   └── kernel.cpp             # VGA console, keyboard input, shell
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg       # GRUB boot menu
├── build/                     # Generated build artifacts (not source)
├── linker.ld                  # Kernel memory layout and entry point
├── Makefile                   # Build, ISO, run, and clean targets
└── README.md
```

Generated files under `build/` and the copied kernel binary under `iso/boot/` are build outputs. They generally should not be committed unless the project intentionally tracks generated artifacts.

## Build and Run

### Prerequisites

The current Makefile expects these tools to be available in the development environment:

- `clang`
- `clang++`
- `ld.lld`
- `grub-file`
- `grub-mkrescue`
- `qemu-system-i386`
- Supporting ISO tools such as xorriso

The project has been developed in Ubuntu under WSL. Install missing packages using your distribution's package manager, following the package names available for that release.

### Build the bootable ISO

From the project root:

```bash
make
```

The build process compiles the assembly entry point and C++ kernel, links the kernel, checks that the kernel is Multiboot-compatible, and creates the ISO image:

```text
build/harbouros.iso
```

### Run in QEMU

```bash
make run
```

A QEMU window should open and display the HarbourOS console and shell prompt. Use the emulator window to type commands.

### Clean generated files

```bash
make clean
```

This removes the generated object files, kernel binary, and ISO listed by the Makefile. It does not remove source files.

## Using the Shell

After boot, the shell displays a prompt:

```text
HarbourOS shell
Type help to see available commands.
>
```

Type one of the supported commands and press Enter.

| Command | Expected behaviour |
|---|---|
| `help` | Displays `help`, `about`, and `clear` as available commands |
| `about` | Prints a short HarbourOS description |
| `clear` | Clears the display and restores the shell heading and prompt |
| Any unsupported command | Displays `Unknown command.` |

### Keyboard support

Current input support includes:

- Lowercase letters `a`–`z`
- Space
- Enter
- Backspace

Uppercase letters, digits, punctuation, modifier keys, and a full keyboard layout are not implemented yet. Keyboard handling is based on legacy PS/2 set-1 scancodes and is polled by the kernel.

## Current Limitations

HarbourOS is an educational prototype, not a general-purpose operating system.

- It supports only a small subset of keyboard input.
- Keyboard handling uses busy polling; interrupt-driven input is not implemented.
- The shell supports only three built-in commands.
- Command parsing is basic and case-sensitive.
- The console is fixed to VGA 80 × 25 text cells and a single text attribute.
- There is no general-purpose memory allocator or virtual-memory manager.
- There is no process model, multitasking, or scheduler.
- There is no filesystem, persistent storage layer, or file commands.
- There are no user/kernel privilege transitions, system calls, or application-loading facilities.
- QEMU testing does not establish compatibility with physical hardware.

## Roadmap

The roadmap is aspirational; items below should be marked complete only after implementation and testing.

### Near-term improvements
- [ ] Verify maximum-length command input and Backspace behaviour at the limit.
- [ ] Improve cursor and line-editing behaviour around row boundaries.
- [ ] Expand keyboard support and improve command parsing.
- [ ] Separate console, keyboard, and shell code into modules.

### Kernel fundamentals
- [ ] Add a Global Descriptor Table (GDT) where required by the kernel design.
- [ ] Add an Interrupt Descriptor Table (IDT) and exception handlers.
- [ ] Implement interrupt-driven keyboard input.
- [ ] Add a physical memory map and a basic memory allocator.
- [ ] Explore paging and virtual memory.
- [ ] Add a basic timer and process/task model.
- [ ] Implement cooperative or pre-emptive scheduling.
- [ ] Add a simple filesystem or read-only initial filesystem.

### Developer experience
- [ ] Add repeatable automated build and smoke-test instructions.
- [ ] Document boot, memory layout, and hardware interfaces in more detail.
- [ ] Add a development log with verified milestones.

## Testing

The primary test environment is QEMU. A basic manual smoke test should verify:

1. `make` completes without errors and creates `build/harbouros.iso`.
2. `make run` boots to the HarbourOS shell.
3. The welcome text and shell prompt appear.
4. `help` lists the built-in commands.
5. `about` prints the project description.
6. An unsupported command prints `Unknown command.`
7. Backspace removes a typed character.
8. `clear` clears the display and restores the shell heading.
9. Long output scrolls instead of writing past the visible VGA text buffer.

Record any failed or untested item honestly; do not report a test as passed without observing its result.

## Contributing

This is a learning-oriented operating-system project. Contributions should favour small, testable changes and clear explanations of low-level behaviour. When changing kernel code, rebuild the ISO and verify the behaviour in QEMU before moving on to the next milestone.

## Disclaimer

HarbourOS is for education and experimentation. It is not intended for production use, safety-critical systems, or use as a replacement for a mature operating system.