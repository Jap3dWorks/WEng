# WEng Engine

> A modern C++23 Vulkan rendering engine developed as a long-term exploration of graphics programming, engine architecture, and data-oriented design.

> **Status:** Experimental • Active Development

---

## Overview

WEng Engine is a personal, long-term project focused on learning modern graphics programming while exploring the architecture of a modular rendering engine.

Rather than serving as a one-off learning exercise, the project is intended to evolve continuously as new rendering techniques, engine systems, and C++23 features are explored.

Current development focuses on building a clean and extensible foundation using modern C++, Vulkan, and an Entity Component System (ECS).

---

## Current Features

### Rendering

* Vulkan renderer
* Deferred rendering pipeline
* Physically Based Rendering (PBR)
* Camera system
* Entity Component System (ECS)

### Work in Progress

* Shadow Mapping
* Screen Space Ambient Occlusion (SSAO)
* Collider system

### Asset Pipeline

* glTF import
* OBJ import
* Texture import
* Internal asset representation

Currently, imported assets are converted into the engine's internal asset system. Asset serialization and packaging are planned for future development.

---

## Design Philosophy

WEng Engine is built around a few core principles:

* Modern C++23
* Zero-overhead abstractions
* Data-oriented design
* Entity Component System architecture
* Modular engine design
* RAII where appropriate
* Maintainable and readable code

An important aspect of the project is the use of **Emacs Org Mode** as part of the development workflow. Documentation, templates, and portions of the generated source code are driven from Org files, allowing implementation and documentation to evolve together.

---

## Project Structure

```text
Source/
├── WCore
├── WEngine
├── WEngineInterfaces
├── WEngineObjects
├── WImporter
├── WRender
├── WSandBox
└── WWindow
```

The project is currently undergoing a gradual namespace and module reorganization to improve consistency and long-term maintainability.

---

## Technologies

Core technologies currently used by the project include:

* C++23
* Vulkan
* Slang
* GLFW
* fastgltf
* stb
* glm

Planned integrations include:

* KTX-Software

---

## Platform Support

| Platform | Status            |
| -------- | ----------------- |
| Linux    | ✅ Fully supported |
| Windows  | ⚠️ Not yet tested |

The repository contains a `Container/` directory with scripts for creating a Fedora Toolbox development environment and installing the required dependencies.

---

## Building

### Development Environment

The repository aims to provide a reproducible development environment.

- Automatic dependency management through CMake
- Fedora Toolbox scripts for containerized development
- Clang, CMake, and Ninja based build workflow
- Minimal manual setup required for supported platforms

### Requirements

* Clang
* CMake
* Emacs
* Make
* Ninja
* Vulkan SDK

#### Why is Emacs required?

WEng uses **Emacs Org Mode** as part of its build pipeline.

Each engine module contains an accompanying Org Mode document with the same name. These documents act as the source of truth for the module, combining documentation, templates, scripts and metadata in a single place.

During the build process, Emacs processes these Org files to generate portions of the C++ source code automatically. This approach keeps documentation and implementation synchronized while reducing boilerplate and repetitive code.

### Build System

The project uses:

* **Make** as the build orchestration layer
* **Emacs Org mode** as the templating autogeneration system
* **CMake** for project configuration
* **Ninja** as the build generator
* **Clang** as the primary compiler

---

## Roadmap

The project is under continuous development.

Planned work includes:

* Collider system
* Physics system
* Skeletal animation
* Asset serialization
* Additional rendering techniques
* Continued engine architecture improvements
* Ongoing performance optimizations

---

## Screenshots

Screenshots will be added as the rendering pipeline reaches a more complete state.

---

## Why This Project?

WEng Engine exists primarily as a long-term educational project.

Its goal is to deepen the understanding of:

* Modern C++23
* Vulkan
* Rendering techniques
* Engine architecture
* ECS and data-oriented design
* Resource management
* Software engineering practices

The project is intended to grow over time rather than reach a fixed endpoint, serving as a continuous exploration of graphics programming and engine development.

---

## License

This project is currently under development.


