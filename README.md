<p align="center"><img src="./doc/assets/logo.png" width="564" alt="Adventure Studio Designer Logo"></p>

# Adventure Studio Designer

**Adventure Studio Designer** was born with the idea of giving life back to a type of game that was embraced by an entire generation, conversational adventures. Surely you remember adventures like the [Hobbit](https://bbcmicro.co.uk/jsbeeb/play.php?autoboot&disc=https://bbcmicro.co.uk/gameimg/discs/1681/Disc094-HobbitSTT.ssd&noseek), [Zork](https://es.wikipedia.org/wiki/Zork) or, in Spain, any of the text adventures created by the company [Aventuras AD](https://es.wikipedia.org/wiki/Aventuras_AD) like [La Aventura Original](https://es.wikipedia.org/wiki/La_Aventura_Original), [Jabato](https://es.wikipedia.org/w/index.php?title=Jabato_(videojuego)&action=edit&redlink=1) o [La Diosa de Cozumel](https://es.wikipedia.org/w/index.php?title=La_diosa_de_Cozumel&action=edit&redlink=1).

Nowadays, a small bunch of people around the world are still creating, playing and distributing this kind of game. In Spain,  the most significant public organisation is [CAAD](https://caad.club/), which provides news, support, forums and a distribution channel for those games.

I was one of those people who spent countless hours playing Text Adventures —also known in Spanish as "Aventuras Conversacionales". I believe that with today's technologies, we can breathe new life into a new genre called **Enhanced Conversational Adventures**. These modern versions could incorporate voice recognition and text-to-speech for accessibility, include video and animated scenes to enhance immersion, and even use Augmented Reality to bring adventures into the real world.

I strongly believe it's possible to empower anyone to create their own Text Adventure without needing any programming knowledge. By providing an intuitive Editor —where users only need to point, create, connect, and compile— we can make game creation accessible to everyone.

Imagine a teacher using such a tool to build a short adventure that re-creates the French Revolution. With no coding skills required, students could explore the events as participants, gaining a deeper understanding of history by experiencing it interactively— even exploring alternate outcomes, like preventing the revolution entirely.

## How to achieve that

I'm an everyday person—a husband, father, grandfather, and lifelong technology enthusiast. I’m fully aware that I don’t have much free time to dedicate to full-time development, but I’m committed to making steady progress and will do my best to deliver new features as often as possible.

The **Adventure Designer Studio** will consist of three main components: the **Editor**, the **Compiler**, and the **Emulator**.

### Editor

This is the cornerstone of the system—a powerful yet user-friendly tool that allows users to build an entire game without writing a single line of code.

The Editor will feature a comprehensive control panel where users can create everything from objects to full scenes, which make up the game map. Within each scene, users will be able to assign images, triggers, objects, and more. They will also be able to define triggers and actions for objects, enabling rich interactivity among the game elements.

Additionally, the Editor will allow users to define the game’s lexicon—verbs, objects, adjectives, synonyms, etc.—enabling players to interact using typed phrases.

Users will also be able to define NPCs (Non-Player Characters), who can interact with the player by offering tips, objects, or puzzles to solve. In the future, NPCs could be enhanced with AI or even replaced with real players in a multiplayer setting.

### Interpreter

This component is responsible for running the game without compiling it—ideal for development and testing. It will offer features like a debugger, custom starting points, and a variable dump window that displays the current objects and their states, triggers, actions, and more. These tools will help developers identify and fix issues during the development phase.

### Compiler

This is the lowest-level component in the system, responsible for generating a deployable product for 8-, 16-, 32-, and 64-bit machines, tailored to the specific characteristics of each platform.

It will also offer a web interface, allowing the game to be played on any device with a browser—providing a truly platform-independent experience.

As you might have guessed, not all features will be available right away, but I’ll work to implement them over time.

The project will be developed in **C++**, chosen for its stability, performance, and flexibility across platforms such as Linux, Windows, and macOS.

C++ also enables us to create specialized processes for compiling games for classic 8-bit platforms like the **ZX Spectrum**, **MSX**, **Commodore 64**, and **Amiga 500**, without losing focus on support for modern platforms.

For the web version, we’ll likely use **React** and **React Native** as the core libraries to run the game on both web and mobile platforms.

The persistence layer will manage data using either a **JSON file** or an **SQLite database**, automatically switching to SQLite if the data grows too large. This approach keeps the project compact and lightweight, minimizing its footprint on any system where it’s run or compiled.

A **Dockerized version** will likely be available for web servers, supporting a “download-and-play” philosophy while keeping the application isolated from the host system and ensuring no modifications are made outside its container.

To achieve some features, I may rely on public libraries or occasionally use other programming languages—but always with the same goal in mind: to deliver a solid, easy-to-use product.

## Key Features:

**Multilingual Support:**

- The **Adventure Designer Studio** will support **i18n**, meaning it will offer a multilingual interface for both the Editor and the game itself. This allows developers to work and players to enjoy the game without language barriers.

- This i18n support will include a **Natural Language Processing Engine (NPLE)** to ensure your stories can be played in multiple languages and reach a wider audience, free from language limitations.

**Import from Popular Engines:**

- Seamlessly import projects from well-known engines like PQWS and DAAD. This capability ensures that you can continue your existing projects within Adventure Studio Designer without losing any progress, providing a smooth transition and enhanced workflow.

**Editor:**

- Seamlessly import projects from well-known engines like **PQWS**, **PAWNS**, **Inform7**, **Aventuron**, **DAAD**, and **readyDAAD**. This capability ensures you can continue your existing projects within **Adventure Designer Studio** without losing any progress, providing a smooth transition and enhanced workflow.

- Each importer will be implemented as a plugin, which can be loaded or unloaded at the user’s discretion.

**Compiler and Interpreter:**

- Adventure Studio Designer will include a robust compiler and interpreter, enabling real-time testing and execution of your adventures. (see interpreter section in this README.md)


**Cross-Platform Compatibility:**

- You can create Adventures that run seamlessly across multiple platforms thanks to the integrated compiler.

- A versatile **Interpreter** lets you run and preview the game exactly as it will appear on the target platform. To achieve this, we will leverage existing emulators to execute the code.

### Rich Media Integration

- Enhance your adventures with rich media elements, including images, sound, and animations. Adventure Designer Studio will support a wide range of current media formats, allowing you to create a more immersive and engaging experience for players.

- The supported formats will be converted by the **Adventure Designer Studio Compiler** into media elements compatible with the target platform.

### New Technologies for Classic Games

- Integrate new technologies such as text-to-speech and speech-to-text, video playback, **Natural Language Processing**, Augmented Reality, AI, and more. These innovations are the core proposals of this project, breathing new life into this often neglected genre of games.

**Community and Support**

- You can contribute your games, ideas, and any other resources to the project. All contributions are welcome, and participation is completely free.

- We encourage an open and inclusive community where everyone’s voice matters. Whether you’re a developer, designer, storyteller, or player, your input helps shape the future of **Adventure Designer Studio**.

- Comprehensive documentation, tutorials, and forums will be available to support users at all levels.

- Regular updates and community events will foster collaboration, learning, and sharing among users worldwide.

- Together, we aim to build not just a tool, but a vibrant ecosystem where creativity and innovation thrive.

**Adventure Studio Designer** is more than just a tool; it's a gateway to endless possibilities in interactive storytelling. Whether you will craft a simple text-based adventure or a complex, branching narrative, **Adventure Studio Designer** provides the tools and support you need to turn your vision into reality. Dive into the world of conversational adventures and start creating today!

## Contributing

Thank you very much for considering contributing to the Adventure Studio Designer! Everyone will be welcome, you just need to write to [cayetano.hernandez.osma@gmail.com](mailto:cayetano.hernandez.osma@gmail.com) and tell us how you can and why you want to collaborate with the project.

We would greatly appreciate it if you could follow the [Convenital Commits guide](https://www.conventionalcommits.org/en/v1.0.0/) when committing your code to the repository. It helps us keep a clean, consistent commit history and enables automatic changelog generation.

We suggest to use the following ones:   
- **feat**: a new feature  
- **fix**: a bug fix
- **docs**: documentation only changes
- **style**: formatting, missing semi colons, etc
- **refactor**: code change that neither fixes a bug nor adds a feature
- **perf**: a code change that improves performance
- **test**: adding missing tests
- **chore**: changes to the build process or auxiliary tools
- **ci**: continuous integration-related changes

## Code of Conduct

In order to ensure that the Adventure Studio Designer community is welcoming to all, please review and abide by the Code of Conduct.

## Security Vulnerabilities

If you discover a security vulnerability or a security tread, please send an e-mail to Adventure Studio Designer via [cayetano.hernandez.osma@gmail.com](mailto:cayetano.hernandez.osma@gmail.com). All security vulnerabilities will be promptly addressed.

## License

The Adventure Designer Studio is open-source software licensed under the [GNU General Public License version 3 (GPL v3)](https://opensource.org/license/gpl-3-0).  
See [LICENSE.md](./LICENSE.md) for the full license text including the additional attribution requirement.
