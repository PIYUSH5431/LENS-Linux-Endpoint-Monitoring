# LENS — Linux Endpoint Monitoring & Security Agent

> **A Linux-based endpoint monitoring and security prototype built with C/C++ and a custom Linux character device driver.**

**LENS** (Linux Endpoint Monitoring & Security Agent) is an individual Linux systems project designed to demonstrate practical concepts in **Linux system programming, process/resource monitoring, security rule evaluation, kernel modules, character devices, and kernel–user-space communication**.

The project combines a modular C++ monitoring agent with a custom C Linux kernel module exposed through `/dev/lens_monitor`. It collects endpoint information from Linux interfaces such as `/proc`, `/sys`, and filesystem statistics, evaluates configurable resource conditions through rule-based logic, and presents the results through a command-line security console.

---

## Table of Contents

- [Project Overview](#project-overview)
- [Problem Statement](#problem-statement)
- [Objectives](#objectives)
- [Key Features](#key-features)
- [Architecture](#architecture)
- [How LENS Works](#how-lens-works)
- [Module Breakdown](#module-breakdown)
- [Project Structure](#project-structure)
- [Technology Stack](#technology-stack)
- [Requirements](#requirements)
- [Build and Installation](#build-and-installation)
- [Running the Kernel Driver](#running-the-kernel-driver)
- [Running the LENS Agent](#running-the-lens-agent)
- [Full Demonstration Flow](#full-demonstration-flow)
- [Security and Monitoring Logic](#security-and-monitoring-logic)
- [Testing and Validation](#testing-and-validation)
- [Design Decisions](#design-decisions)
- [Limitations](#limitations)
- [Future Enhancements](#future-enhancements)
- [Learning Outcomes](#learning-outcomes)
- [Project Status](#project-status)
- [Author](#author)

---

## Project Overview

Modern Linux endpoints generate a large amount of operational information: CPU activity, memory consumption, disk utilization, running processes, network traffic, uptime, and kernel-level device information.

LENS provides a lightweight command-line monitoring and security prototype that brings several of these capabilities together into one modular application.

The project has two major layers:

1. **User-space monitoring and security agent**
   - Implemented primarily in C++
   - Reads Linux system information
   - Performs resource analysis
   - Runs security rules
   - Produces monitoring and alert output

2. **Kernel-space driver interface**
   - Implemented in C
   - Built as a Linux kernel module
   - Registered as a character device
   - Exposes `/dev/lens_monitor`
   - Provides a kernel-to-user-space monitoring interface

### Core architecture

```text
                 ┌──────────────────────────────┐
                 │        LENS Agent (C++)      │
                 │                              │
                 │  System / Process / Network │
                 │  Security / Health / Alerts  │
                 └──────────────┬───────────────┘
                                │
                 ┌──────────────┴───────────────┐
                 │        Linux Interfaces      │
                 │                              │
                 │ /proc   /sys   statvfs()     │
                 │ /proc/net/dev   system data  │
                 └──────────────┬───────────────┘
                                │
                                │
                       /dev/lens_monitor
                                │
                 ┌──────────────▼───────────────┐
                 │ Linux Character Driver (C)   │
                 │                              │
                 │ lens_driver.ko               │
                 │ open / read / release        │
                 └──────────────┬───────────────┘
                                │
                         Linux Kernel
```

> **Important implementation note:** The custom driver demonstrates the kernel/user-space interface and character-device mechanism. The current prototype obtains most endpoint metrics directly in user space through Linux interfaces such as `/proc` and filesystem statistics; the driver currently exposes a monitoring interface/status message rather than claiming to collect every metric itself.

---

## Problem Statement

A Linux endpoint can expose useful operational information through multiple system interfaces, but these sources are normally inspected independently.

The objective of LENS is to create a single modular security-monitoring prototype capable of:

- Monitoring endpoint resource utilization
- Inspecting running processes
- Observing network interface counters
- Detecting selected resource threshold conditions
- Recording monitoring events
- Calculating an overall health score
- Performing a basic process-resource security scan
- Demonstrating a custom Linux character driver
- Providing a clear kernel-to-user-space communication path

---

## Objectives

### Primary Objectives

- Build a Linux-focused endpoint monitoring application using C/C++.
- Implement a custom Linux kernel module and character device.
- Demonstrate communication between user space and kernel space.
- Monitor CPU, memory, disk, process, and network information.
- Implement rule-based resource alerting.
- Integrate multiple monitoring modules through one launcher.
- Maintain a modular source-code structure suitable for further development.

### Engineering Objectives

- Use Linux `/proc` interfaces for system/process information.
- Use filesystem statistics for disk monitoring.
- Use `/proc/net/dev` for network interface counters.
- Use Linux kernel module APIs for the driver layer.
- Keep kernel-space and user-space responsibilities separated.
- Make the prototype easy to compile, test, demonstrate, and extend.

---

## Key Features

| Feature | Description |
|---|---|
| System Monitor | CPU, RAM, disk utilization and uptime |
| Process Monitor | Running-process inspection and resource usage |
| Network Monitor | Interface-level RX/TX byte counters |
| Alert Logger | Records monitoring events in a log file |
| Security Alert Engine | RAM/disk threshold-based security alerts |
| Kernel Driver Interface | Reads the custom `/dev/lens_monitor` character device |
| System Health Score | Produces a rule-based endpoint health score |
| Process Security Scan | Flags processes exceeding a configured RAM threshold |
| Integrated Launcher | Single menu-driven interface for all modules |
| Full Monitoring Cycle | Sequential execution of major monitoring/security checks |

---

## Architecture

### High-Level Architecture

```text
                    LENS SECURITY AGENT
                           │
             ┌─────────────┴─────────────┐
             │                           │
       USER-SPACE LAYER             KERNEL LAYER
             │                           │
       C++ Monitoring Agent          C Driver
             │                           │
    ┌────────┼────────┐          ┌───────┴────────┐
    │        │        │          │ Character      │
  /proc    /sys   Filesystem     │ Device         │
    │        │        │          │ /dev/lens_     │
    │        │        │          │ monitor        │
    └────────┼────────┘          └───────┬────────┘
             │                           │
             └──────────┬────────────────┘
                        │
                  LENS CLI Output
                        │
              Monitoring + Security
```

### Responsibility Separation

**User Space**
- Resource monitoring
- Process inspection
- Network statistics
- Rule evaluation
- Health scoring
- Alert logging
- User interaction

**Kernel Space**
- Character device registration
- Device node interface
- Driver open/read/release operations
- Kernel-to-user monitoring status interface

This separation keeps the kernel component focused while allowing the monitoring logic to remain easier to test and evolve.

---

## How LENS Works

A typical monitoring cycle follows this sequence:

```text
1. Start LENS
      ↓
2. Select monitoring/security operation
      ↓
3. Read Linux system interfaces
      ↓
4. Process collected values
      ↓
5. Apply security/threshold rules
      ↓
6. Generate status / alert information
      ↓
7. Optionally communicate with kernel driver
      ↓
8. Display result to operator
```

For the driver path:

```text
LENS C++ Interface
        │
        │ open()
        ▼
/dev/lens_monitor
        │
        ▼
Linux Character Driver
        │
        ▼
Linux Kernel
        │
        ▼
read() response
        │
        ▼
LENS displays driver status
```

---

## Module Breakdown

### 1. System Monitor

**Source:** `agent/system_monitor.cpp`

Collects:

- CPU utilization
- RAM utilization
- Disk utilization
- System uptime

Status levels:

```text
< 75%       → NORMAL
75–89%      → WARNING
≥ 90%       → CRITICAL
```

The monitor uses Linux system information such as `/proc/stat`, `/proc/meminfo`, `/proc/uptime`, and filesystem statistics.

---

### 2. Process Monitor

**Source:** `agent/process_monitor.cpp`

Scans Linux processes through `/proc` and reports process-level information.

The module is designed to provide visibility into:

- Process IDs
- Process names
- Process states
- Memory usage
- CPU/resource activity

A top-process view is used to make the information easier to inspect during demonstration.

---

### 3. Network Monitor

**Source:** `agent/network_monitor.cpp`

Reads `/proc/net/dev` and reports network interface counters including:

- Interface name
- RX bytes
- TX bytes

Example interfaces may include:

```text
lo
wlan / Wi-Fi interface
Ethernet interface
```

The module focuses on endpoint-level interface statistics rather than packet inspection.

---

### 4. Alert Logger

**Source:** `agent/alert_logger.cpp`

Provides a simple event logging mechanism.

The current implementation records important agent events such as startup into:

```text
lens_alerts.log
```

The logging layer is intentionally kept separate so additional security rules can write structured events to the same logging mechanism later.

---

### 5. Security Alert Engine

**Source:** `agent/security_alert.cpp`

Implements threshold-based resource security rules.

Current checks:

- RAM utilization
- Root filesystem/disk utilization

Rules:

```text
< 75%       → NORMAL
75–89%      → WARNING
≥ 90%       → CRITICAL
```

When a warning/critical condition is detected, the event is printed and written to:

```text
lens_alerts.log
```

> The current implementation does not perform CPU-based alerting in this module.

---

### 6. System Health Score

**Source:** `agent/health_score.cpp`

Calculates a simplified endpoint health score starting from:

```text
100 points
```

The score is reduced according to the current RAM and disk conditions.

The module also reports:

- CPU monitoring status
- RAM status
- Disk status
- Network activity status
- Kernel driver connection status

> CPU is currently displayed as monitored status but is **not included in the numerical health-score calculation**.

---

### 7. Process Security Scanner

**Source:** `agent/process_security.cpp`

Performs a basic process-resource security scan.

Current rule:

```text
Process RAM usage ≥ 20% of total system RAM
                ↓
          Suspicious process
```

The scanner traverses `/proc`, counts processes, calculates process memory usage, and reports processes crossing the configured threshold.

This is a lightweight anomaly-detection prototype and is **not intended to replace an antivirus/EDR engine**.

---

### 8. Kernel Driver Interface

**User-space source:** `agent/driver_interface.cpp`

**Kernel source:** `driver/lens_driver.c`

The driver is implemented as a Linux character device.

The module:

- Registers a character-device interface
- Creates `/dev/lens_monitor`
- Supports open/read/release operations
- Provides a kernel-level monitoring interface

The C++ application opens the device and reads the driver's status.

Example concept:

```text
C++ Agent
   │
   ├── open("/dev/lens_monitor")
   │
   ├── read()
   │
   ▼
Kernel Driver
   │
   ▼
"LENS Kernel Driver: Monitoring interface active"
```

This demonstrates a fundamental Linux device-driver workflow without requiring dedicated physical hardware.

---

## Project Structure

```text
LENS/
│
├── agent/
│   ├── lens_main.cpp
│   ├── system_monitor.cpp
│   ├── process_monitor.cpp
│   ├── network_monitor.cpp
│   ├── alert_logger.cpp
│   ├── security_alert.cpp
│   ├── health_score.cpp
│   ├── process_security.cpp
│   └── driver_interface.cpp
│
├── driver/
│   ├── lens_driver.c
│   └── Makefile
│
└── README.md
```

### Design Philosophy

The project is intentionally modular:

```text
lens_main
   │
   ├── System Monitoring
   ├── Process Monitoring
   ├── Network Monitoring
   ├── Alert Logging
   ├── Security Alerts
   ├── Driver Interface
   ├── Health Score
   └── Process Security
```

This makes individual components independently testable while keeping the overall system easy to demonstrate.

---

## Technology Stack

### Programming Languages

- **C**
- **C++17**

### Operating System

- **Linux**

### Linux Concepts

- Linux kernel modules
- Character devices
- `/proc`
- `/sys`
- Device nodes
- File descriptors
- Kernel/user-space communication
- Process inspection
- Filesystem statistics

### Development Tools

- GCC / G++
- GNU Make
- Linux kernel build system
- Git
- GitHub
- Linux terminal

---

## Requirements

A Linux environment with:

- GCC
- G++
- GNU Make
- Linux kernel headers
- Root/sudo access for kernel-module operations
- Git

For Debian/Ubuntu-based distributions, typical prerequisites are:

```bash
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r) git
```

---

## Build and Installation

Clone the repository:

```bash
git clone https://github.com/PIYUSH5431/LENS-Linux-Endpoint-Monitoring.git
cd LENS-Linux-Endpoint-Monitoring
```

### Build the Driver

```bash
cd driver
make
```

A successful build produces:

```text
lens_driver.ko
```

### Build the User-Space Modules

```bash
cd ../agent
```

Compile the individual programs as required. For example:

```bash
g++ -std=c++17 system_monitor.cpp -o system_monitor
g++ -std=c++17 process_monitor.cpp -o process_monitor
g++ -std=c++17 network_monitor.cpp -o network_monitor
g++ -std=c++17 alert_logger.cpp -o alert_logger
g++ -std=c++17 security_alert.cpp -o security_alert
g++ -std=c++17 health_score.cpp -o health_score
g++ -std=c++17 process_security.cpp -o process_security
g++ -std=c++17 driver_interface.cpp -o driver_interface
g++ -std=c++17 lens_main.cpp -o lens
```

---

## Running the Kernel Driver

From the `driver` directory:

```bash
sudo insmod lens_driver.ko
```

Verify that the module is loaded:

```bash
lsmod | grep lens
```

Verify the device node:

```bash
ls -l /dev/lens_monitor
```

Test the character device:

```bash
sudo cat /dev/lens_monitor
```

The driver should return its monitoring-interface status message.

### Removing the Driver

```bash
sudo rmmod lens_driver
```

---

## Running the LENS Agent

From:

```bash
cd agent
```

Start the main interface:

```bash
./lens
```

The launcher provides a menu similar to:

```text
========================================
        LENS SECURITY AGENT
   Linux Endpoint Monitoring System
========================================

1. System Monitor
2. Process Monitor
3. Network Monitor
4. Alert Logger
5. Security Alert Engine
6. Kernel Driver Interface
7. System Health Score
8. Process Security Scan
9. Run Full Monitoring
0. Exit
```

---

## Full Demonstration Flow

For a compact project demonstration, use:

```text
./lens
```

Then select:

```text
9. Run Full Monitoring
```

The integrated flow runs the major components sequentially:

```text
[1] System Monitor
        ↓
[2] Process Monitor
        ↓
[3] Network Monitor
        ↓
[4] Kernel Driver Interface
        ↓
[5] Security Alert Engine
        ↓
[6] Process Security Scan
        ↓
       DONE
```

This provides a single end-to-end demonstration of the monitoring and security pipeline.

---

## Security and Monitoring Logic

LENS currently uses deterministic rule-based analysis.

### Resource Threshold Model

```text
                 Resource Usage
                       │
          ┌────────────┼────────────┐
          │            │            │
        < 75%       75–89%        ≥ 90%
          │            │            │
        NORMAL       WARNING      CRITICAL
```

### Process Security Model

```text
Process
   │
   ▼
Read /proc/<PID>
   │
   ▼
Calculate memory usage
   │
   ▼
Compare against threshold
   │
   ├── Below 20% → OK
   │
   └── ≥ 20% → ALERT
```

These rules are intentionally simple and explainable, making the prototype suitable for demonstrating system-programming and security-monitoring fundamentals.

---

## Testing and Validation

The project was validated module-by-module before integration.

### System Monitor

Validated:

- CPU calculation
- RAM calculation
- Disk calculation
- Uptime display
- Threshold status

### Process Monitor

Validated:

- `/proc` traversal
- Process discovery
- Process resource information
- Top-process reporting

### Network Monitor

Validated:

- Interface discovery
- RX byte counters
- TX byte counters

### Alert Logger

Validated:

- Application startup logging
- Log-file creation
- Timestamped entries

### Security Alert Engine

Validated:

- RAM threshold evaluation
- Disk threshold evaluation
- Normal/warning/critical states
- Alert logging

### Health Score

Validated:

- RAM/disk-based score calculation
- Driver status reporting
- Network status reporting
- Overall status presentation

### Process Security Scanner

Validated:

- Process scanning
- Memory threshold comparison
- Suspicious-process reporting

### Kernel Driver

Validated:

- Kernel-module compilation
- Module insertion
- Character-device creation
- `/dev/lens_monitor` access
- User-space read operation

### Integration

Validated:

- Menu-based launcher
- Individual module execution
- Full monitoring cycle
- Kernel driver integration

---

## Design Decisions

### Why Linux?

Linux provides direct access to:

- `/proc`
- `/sys`
- Device nodes
- Kernel modules
- Character devices
- Low-level system interfaces

This makes Linux particularly suitable for demonstrating system programming and device-driver concepts.

### Why C and C++?

The project deliberately uses both languages for their appropriate layers:

**C**
- Linux kernel module
- Character device
- Kernel APIs

**C++**
- User-space monitoring agent
- Modular application logic
- CLI integration
- Resource/security analysis

### Why a Character Driver?

A character device provides a straightforward interface between user space and kernel space.

The project uses:

```text
Application → /dev/lens_monitor → Kernel Driver
```

This demonstrates a practical Linux driver architecture without requiring custom physical hardware.

### Why a Modular Architecture?

Monitoring functions are separated into independent modules so that:

- Individual components can be tested independently.
- Failures are easier to isolate.
- Features can be extended without rewriting the whole application.
- The final demonstration remains organized.

---

## Limitations

LENS is a **systems-programming and security-monitoring prototype**, not a production EDR/antivirus product.

Current limitations include:

1. The custom driver currently exposes a monitoring/status interface rather than collecting all endpoint metrics directly from kernel space.
2. The Security Alert Engine currently evaluates RAM and disk thresholds.
3. CPU is not part of the numerical Health Score calculation.
4. The Process Security Scanner currently uses a RAM-based threshold.
5. No central server or cloud dashboard is implemented.
6. No persistent database is used.
7. No packet-level network inspection is implemented.
8. No malware signature database or behavioral malware engine is included.
9. Driver communication is currently based on the character-device read interface; advanced `ioctl`/event-buffer mechanisms are future work.
10. The prototype focuses on demonstrating Linux concepts, modular design, monitoring, and security-rule evaluation.

These limitations are intentional boundaries for the current project scope.

---

## Future Enhancements

The architecture can be extended significantly.

### Kernel-Level Enhancements

- `ioctl()` based driver commands
- Kernel event buffers
- Synchronization using mutexes/spinlocks where appropriate
- Polling/event notification mechanisms
- Interrupt handling for suitable hardware-backed use cases
- Deferred work/workqueues
- More structured kernel-to-user telemetry

### Security Enhancements

- Configurable rule engine
- CPU-based process anomaly detection
- File-integrity monitoring
- Suspicious command detection
- Authentication/login monitoring
- Privilege escalation detection
- Security event correlation
- Configurable alert severity

### Infrastructure Enhancements

- Central monitoring server
- REST/secure network reporting
- Web dashboard
- SQLite/PostgreSQL event storage
- Historical trend analysis
- Multi-endpoint monitoring
- Role-based access control

### Deployment Enhancements

- systemd service
- Configuration file
- Structured JSON logs
- Automated driver loading
- Installer/package support
- Containerized development/testing

### Driver Development Enhancements

Where a suitable hardware/event source is introduced, the driver architecture can be extended toward:

```text
Application
     ↓
Linux Kernel
     ↓
Device Driver
     ↓
Device Model / Bus
     ↓
Hardware
```

QEMU can also be used as a controlled environment for future kernel/driver experimentation without requiring physical hardware.

---

## Learning Outcomes

Through LENS, the following concepts are demonstrated:

- Linux system programming
- C/C++ development
- Process management and `/proc`
- File descriptors and device files
- Linux character devices
- Kernel modules
- Kernel/user-space boundaries
- Resource monitoring
- Rule-based security analysis
- Logging
- Modular software architecture
- GNU Make
- Git and GitHub workflow
- Testing and incremental integration
- Linux debugging and command-line tooling

---

## Project Status

**Status: Working Prototype / Capstone MVP**

The current implementation provides:

- Modular Linux monitoring
- Security rule evaluation
- Process-resource analysis
- Network statistics
- Alert logging
- System health scoring
- Custom Linux character driver
- User-space driver interface
- Integrated command-line launcher

The project is structured to support future development toward a more complete endpoint-security platform.

---

## Author

**Piyush Kumar Gupta**

GitHub: **PIYUSH5431**

Project: **LENS — Linux Endpoint Monitoring & Security Agent**

---

## License

This project is intended as an academic/capstone project and learning reference. Add an explicit open-source license if the repository is intended for redistribution or external contribution.
