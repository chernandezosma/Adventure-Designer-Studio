# How LexEngine gets used in practice

## Access point

Every `Core::Project` owns exactly one LexEngine instance (not per-scene/item — one per project):

```c++
#include "Core/Project.h"

ADS::Core::Project project("My Adventure");                                                                                                                                     
ADS::LexEngine::LexEngine& lex = project.getLexEngine();

```

## Feeding text (authoring time)

As the author types dialogue/descriptions in the IDE, feed sentences through an NLP backend (currently FallbackNLPBackend, with INLPBackend as the swappable interface for a    
future UDPipe-backed one):

```c++
#include "LexEngine/FallbackNLPBackend.h"

ADS::LexEngine::FallbackNLPBackend backend;                                                                                                                                     
lex.feed("Coge la llave dorada", ADS::LanguageCode{"es_ES"}, backend);
```

Internally feed() tokenizes the sentence and calls record() per lexical token, which inserts new LexEntry objects (or bumps frequency on existing ones) and runs the            
SynonymPipeline to propose synonym links.

## Compiling (indexing)

Onc e authoring for a language is "done" (or before emitting the binary vocab table), call:

```c++
lex.index();
```

This sorts each language's entries by descending frequency and assigns TokenIndex values (1-byte for the top 254, 3-byte beyond that) — this is the step that produces the      
compact token IDs destined for the 8-bit target's vocab_table.

## Querying

const auto& entries = lex.getEntries(ADS::LanguageCode{"es_ES"});                                                                                                               
LexEntry* e = lex.findById(someId);

## Persistence

LexEngineSerializer::toJson(entry) / fromJson(json) handle the canonical per-entry JSON shape (documented in docs/core/lexengine/vocabulary.md) for save/load — this isn't wired
into Project's save/load yet, since project (de)serialisation is still on your Next work items list.

That's the whole surface right now — feed()/record() to build vocabulary, index() to compile it, getEntries()/findById() to query it. Nothing else in the codebase calls it yet
(Core::Project::getLexEngine() is currently unused by any IDE panel), so it's ready to be wired into whatever text-entry UI feeds it next.  