# Overview
WEngine is a kind of game  engine conceived to act with a client server logic.
We want ti use Wasm to create a web application.
The movitaion of the project is to create an easy deployable system in an intranet.
The server will contain assets, data and users and the client will be the editor (web or desktop).
Each new functionality could be a new client. 
e.g a game compiler client.
Configs, acces rights and versions should be managed by the Server.

## Milestones
- [ ] Draw a simple cube using wasm
- [ ] Draw a simple plane and load glsl shaders
- [ ] Load a simple module in main app
- [ ] Load an Usd Model
- [ ] Rethink project structure

## Things to think about
### Component system and component registration.
### Command system, command queue and command registation (Threaded).
### Editor Mode and Play Mode.
### File format, I bet for Usd file format.
### Edit and view modes, useful when more than one user wants to edit the same asset or scene.
Because it have already solutions for scene compositing and dependencies.

## To Investigate
Gotod + Usd
js binding

## References
https://github.com/mortennobel/SimpleRenderEngine

### DLL_EXPORT
https://stackoverflow.com/a/2164853

