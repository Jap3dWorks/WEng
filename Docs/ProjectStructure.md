# Project Structure

Each submodule inside Source is a core Module of the project.

Each Game using WEng will also contain a Source directory with all submodules inside.
One of those submodules will be the main module.

A Game using WEng should be located in an external structure.

* DevsDir
  * MyGame
    * Source
    * Install
      * <arch>
        * lib
        * bin
  * WEng
    * Source
    * Install
      * <arch>
        * lib
        * bin

So WEng can be located in any directory in your system.

Each Module contains a CMakeLists.txt file that will manage the compile and link process of the module.

