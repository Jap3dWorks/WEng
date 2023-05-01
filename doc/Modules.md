# Modules structure
## Introduction
Modules structures will be explained next.

## Tree
* modules (camelCase)
    * ModulesNames _(PascalCase)_
        * CMakeLists.txt (each mmodule will be managed by its own CMakeLists)
        * src
            * ModuleFile.cpp _(PascaoCase)_
        * include
            * ModuleHeader.h _(PascalCase)_
    * ...
* source (camelCase)
    * WSourceModules _(PascalCase)_
        * CMakeLists.txt (each mmodule will be managed by its own CMakeLists)
        * src
            * ModuleFile.cpp _(PascaoCase)_
        * include
            * ModuleHeader.h _(PascalCase)_
    * ...

## Source modules _In progress_
### Objectives
Modules should be linkable bettween them and dinamically linkable with the core application.
Our source modules should avoid unneeded dependencies.
Model controller paradigm.
Data structure design paradigm (memory addresess sorted by component class).
Use standard library for data types. 
Use libraries to solve complex problems.

### WEngine
_Principal Module_  
WEngine is the starting point of the aplication, the main function will be here.
Maybe basic classes and data structures will be here too, like.
* int types
* vector types
* map types
* ...

WEngine will also load each module dinamically, this way the application can be extended using a plugin system, 
modules for ours.

### WCommand
_Command Module_
This module will contain the system for command registation, base classes and command queues. 
each module will be responsible of its commands creation and registation in the system.
