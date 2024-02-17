# Introduction 
WEng is a for fun project based in c++, cmake and OpenGL.
The intention is to create a simply modular Game Engine for learning porpuses.

In this initial state our data files will be USD both for models as for levels.

There are intention to try to migrate to Vulkan.

## Dependencies
Vulkan
SDL
GLM
USD

## Project Structure

## wcli
Open an env terminal using Environment script.
```
source Environment.sh
```  

Now you can use wcli, using ```wcli Project``` can help you to create projects and modules.
You can create a new project using the following code.
```
wcli Project -p /path/to/project_folder
```
Also you can create any number of modules using the follogind code.
```
wcli Project -p /path/to/project_folder -m Module1 Module2
```

## Create a Visual Studio Code dev Environment

### Linux
Open a terminal in the engine directory.
Run the following code to load the dev environment. This bash script simply add some paths to PATH env var.

```
source Environment.sh
```

Now you can use the WCli tool to automate some processes.

```
wcli VSCEnv -p .
```

