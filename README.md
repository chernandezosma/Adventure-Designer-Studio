<p align="center"><img src="public/assets/logo.png" width="564" alt="Adventure Studio Designer Logo"></p>

# Adventure Studio Designer

**Adventure Studio Designer** is a project born from the idea of reviving a
genre that captivated an entire generation: conversational adventures. You might
remember classic text adventures like
[The Hobbit](https://bbcmicro.co.uk/jsbeeb/play.php?autoboot&disc=https://bbcmicro.co.uk/gameimg/discs/1681/Disc094-HobbitSTT.ssd&noseek),
[Zork](https://es.wikipedia.org/wiki/Zork), or, in Spain, any of the adventures
created by [Aventuras AD](https://es.wikipedia.org/wiki/Aventuras_AD) such as
[La Aventura Original](https://es.wikipedia.org/wiki/La_Aventura_Original),
[Jabato](https://es.wikipedia.org/w/index.php?title=Jabato_(videojuego)&action=edit&redlink=1),
or [La Diosa de Cozumel](https://es.wikipedia.org/w/index.php?title=La_diosa_de_Cozumel&action=edit&redlink=1).

## The Retro Community

The retro-gaming community is growing more than I ever expected. Although we're
a small group of enthusiasts around the world who continue creating, playing,
and distributing these types of games, the passion remains strong. In Spain,
the most significant community organization related to this genre is
[CAAD](https://caad.club/), which provides news, support, forums, and a
distribution channel for text adventures. There are also several Telegram
channels whose main goal is to support this kind of development for retro
platforms.

## Vision

I spent countless hours playing Text Adventures—also known in Spanish as
'Aventuras Conversacionales'. I firmly believe that today's technologies can
breathe new life into this genre by creating **Enhanced Conversational
Adventures**. These modern versions could incorporate features such as:

- Voice recognition and text-to-speech for accessibility
- Video and animated scenes to enhance immersion
- Augmented Reality to bring adventures into the real world

Note that these enhanced features will likely be supported only on modern
platforms and won't be available for older 8/16-bit systems.

## Empowering Creators

I strongly believe it's possible to empower anyone to create their own Text
Adventure without any programming knowledge. By providing an intuitive
editor—where users simply point, create, connect, and compile—we can make game
creation accessible to everyone.

### Educational Potential

Imagine a teacher using such a tool to build a short adventure that recreates
the French Revolution. With no coding skills required, students could explore
historical events as participants, gaining a deeper understanding through
interactive experience—even exploring alternate outcomes, like preventing the
revolution entirely.

## How We Try to Achieve That

I'm an everyday person—a husband, father, grandfather, and lifelong technology
enthusiast. I'm fully aware that I don't have much free time to dedicate to
full-time development, but I'm committed to making steady progress and will do
my best to deliver new features as often as possible.

**Adventure Studio Designer** will consist of several components: the
**Editor**, the **Compiler**, the **Interpreter**, the **Debugger**, and other
tools that will be built as the project grows.

### Editor

This is the cornerstone of the system—a powerful yet user-friendly tool that
allows users to build an entire game without writing a single line of code.

The Editor will feature a comprehensive control panel where users can create
everything from objects to full scenes that make up the game map. Within each
scene, users will be able to assign different object properties using the
integrated inspector and bring every single object to life. Among the
properties are actions and triggers, which are events that can be programmed to
execute when a condition or specific event occurs. All these associations will
be made using only the mouse and the worldwide point-and-click paradigm. These
features will enable rich interactivity among game elements.

Additionally, the Editor will support multiple languages for the texts
displayed during gameplay and for the interpreter (parser), which will analyze
and interpret the player's commands to progress through the game. Creators can
define the game's lexicon—verbs, objects, adjectives, synonyms, etc.—using
i18n, enabling players to interact using typed phrases.

I plan to give users the ability to define NPCs (Non-Player Characters) who can
interact with the player by offering tips, objects, or puzzles to solve. I also
plan to bring these NPCs to life—in future versions, they could be enhanced
with AI or even replaced with real players in a multiplayer setting. Obviously,
these advanced features will be available only on modern platforms, not on
older ones.

### Interpreter

This component is responsible for running the game without compiling it—an
ideal tool for development and testing. It will offer features like a debugger,
custom starting points, and a variable and memory dump window to monitor how
variables and memory positions change during game execution. These tools help
developers identify and fix errors that may occur during gameplay.

### Compiler

This is the lowest-level component in the system, responsible for generating a
deployable product for all platforms. It will include a component that
determines whether the game's objects fit within the target platform's
constraints and helps the developer easily choose which objects to remove to
meet the requirements for the selected platform.

The compilation process will create native assembly-based code, compiled and
ready to load on the selected target platform.

For older 8-bit and 16-bit platforms, the specific characteristics of each
target platform will be taken into account to ensure the code is as efficient
as possible in terms of size and speed.

For modern platforms and the web, we will use a JavaScript runtime, likely
React Native, to enable app publishing on various app stores (iOS and Android)
while maintaining the same efficiency criteria. The compiler will also offer a
web interface, allowing the game to be played on any device with a
browser—providing a truly platform-independent experience.

As you might have guessed, not all of these features will be available right
away, but I'll work to implement them over time.

### Technology Stack

The project will be developed in **C++ version 23**, chosen for its stability,
performance, and flexibility across platforms such as Linux, Windows, and
macOS.

C++ also enables us to create specialized processes for compiling games for
classic 8-bit platforms like the **ZX Spectrum**, **MSX**, **Commodore 64**,
and **Amiga 500**, without losing focus on support for modern platforms.

For the web version, we'll likely use **React** and **React Native** as the
core libraries to run the game on both web and mobile platforms.

The persistence layer will manage data using either a **JSON file** or an
**SQLite database**, automatically switching to SQLite if the data grows too
large. This approach keeps the project compact and lightweight, minimizing its
footprint on any system where it's run or compiled.

A **Dockerized version** will likely be available for web servers, supporting a
"download-and-play" philosophy while keeping the application isolated from the
host system and ensuring no modifications are made outside its container.

To achieve some features, I may rely on public libraries or occasionally use
other programming languages—but always with the same goal in mind: to deliver a
solid, easy-to-use product.

## Key Features

### Multilingual Support

**Adventure Studio Designer** will support **i18n**, offering a multilingual
interface for both the Editor and the game itself. This allows developers to
work and players to enjoy games without language barriers.

The i18n support includes a **Natural Language Processing Engine (NPLE)** to
ensure your stories can be played in multiple languages, reaching a wider
audience free from language limitations.

Note that some advanced multilingual features, particularly the NPLE
capabilities, may not be available on older 8/16-bit platforms due to memory
and processing constraints. These features will be automatically included when
targeting platforms that support them, ensuring your game takes full advantage
of each platform's capabilities.

### Cross-Platform Compatibility

Create adventures that run seamlessly across multiple platforms thanks to the
integrated compiler. The versatile **Interpreter** lets you run and preview
your game exactly as it will appear on the target platform. To achieve this, we
will leverage existing emulators to execute the code, enabling real-time
testing and execution of your adventures.

### Interchangeability and Plugin System

One of the key strengths of **Adventure Studio Designer** is its ability to
work with projects from classic adventure game development systems. The Editor
includes built-in import capabilities for popular engines such as **The
Quill**, **PAWS**, **DAAD**, **Inform 7**, **Aventuron**, and **readyDAAD**,
ensuring you can migrate existing projects without losing progress.

Both import and export functionality are designed as a plugin system, allowing
for modularity and extensibility. Each importer and exporter is implemented as
an independent plugin that can be loaded or unloaded at your discretion. This
architecture keeps the core application lightweight while making it easy for
the community to contribute additional support for other adventure development
systems.

The plugin system is developer-friendly, with clear documentation and examples
to help community members create their own importers and exporters. This
collaborative approach ensures **Adventure Studio Designer** can grow to
support an ever-expanding range of classic and modern adventure game formats.

### Rich Media Integration

Enhance your adventures with rich media elements, including images, sound, and
animations. Adventure Studio Designer supports a wide range of current media
formats, allowing you to create immersive and engaging experiences for players.

The **Adventure Studio Designer Compiler** will convert supported formats into
media elements compatible with your target platform, ensuring your rich content
works seamlessly across all systems.

### New Technologies for Classic Games

Integrate cutting-edge technologies such as text-to-speech and speech-to-text,
video playback, **Natural Language Processing**, Augmented Reality, and AI.
These innovations are at the core of this project, breathing new life into this
often neglected genre of games and creating **Enhanced Conversational
Adventures** for modern audiences.

### Community and Support

We encourage an open and inclusive community where everyone's voice matters.
Whether you're a developer, designer, storyteller, or player, your input helps
shape the future of **Adventure Studio Designer**.

You can contribute your games, ideas, and any other resources to the project.
All contributions are welcome, and participation is completely free.

Comprehensive documentation, tutorials, and forums will be available to support
users at all levels. Regular updates and community events will foster
collaboration, learning, and sharing among users worldwide.

Together, we aim to build not just a tool, but a vibrant ecosystem where
creativity and innovation thrive.

---

**Adventure Studio Designer** is more than just a tool; it's a gateway to
endless possibilities in interactive storytelling. Whether you craft a simple
text-based adventure or a complex, branching narrative, **Adventure Studio
Designer** provides the tools and support you need to turn your vision into
reality. Dive into the world of conversational adventures and start creating
today!

## Contributing

Thank you for considering contributing to Adventure Studio Designer! Everyone
is welcome. Simply write to
[cayetano.hernandez.osma@gmail.com](mailto:cayetano.hernandez.osma@gmail.com)
and tell us how and why you want to collaborate with the project.

We would greatly appreciate it if you could follow the
[Conventional Commits guide](https://www.conventionalcommits.org/en/v1.0.0/)
when committing your code to the repository. It helps us keep a clean,
consistent commit history and enables automatic changelog generation.

We suggest using the following commit types:

- **feat**: a new feature
- **fix**: a bug fix
- **docs**: documentation only changes
- **style**: formatting, missing semicolons, etc.
- **refactor**: code change that neither fixes a bug nor adds a feature
- **perf**: a code change that improves performance
- **test**: adding missing tests
- **chore**: changes to the build process or auxiliary tools
- **ci**: continuous integration-related changes

## Code of Conduct

To ensure that the Adventure Studio Designer community is welcoming to all,
please review and abide by the Code of Conduct.

## Security Vulnerabilities

If you discover a security vulnerability or security threat, please send an
email to Adventure Studio Designer via
[cayetano.hernandez.osma@gmail.com](mailto:cayetano.hernandez.osma@gmail.com).
All security vulnerabilities will be promptly addressed.

## License

Adventure Studio Designer is open-source software licensed under the
[GNU General Public License version 3 (GPL v3)](https://opensource.org/license/gpl-3-0).
See [LICENSE.md](./LICENSE.md) for the full license text including the
additional attribution requirement.