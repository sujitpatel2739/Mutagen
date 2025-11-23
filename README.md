# **Mutagen**

### *A Self-Evolving Compiler for the Genex Nano-Language*

**Mutagen** is an experimental compiler framework designed to explore autonomous code transformation. Instead of treating source code as static text, Mutagen views it as *evolvable material*. The system mutates, refines, and restructures Genex structures until they reach a valid, working state.

Genex, the nano-language built for this project, is intentionally minimal: small syntax surface, simple semantics, and a structure optimized for rapid transformation. Mutagen acts as its compiler, interpreter, and evolutionary engine.

This project demonstrates how a programming language and its compiler can operate as a dynamic, self-adjusting ecosystem rather than a fixed rulebook.

---

## **Features**

* Custom lexer and parser for the **Genex** nano-language
* Minimal syntax designed for mutation, recombination and structural edits
* Lightweight AST representation
* Modular compilation pipeline
* Self-adapting code evolution engine
* Automatic repair of broken Genex programs
* Clear separation between language, compiler, and evolution layers
* Fully implemented in C++

---

## **Why Genex?**

Genex is intentionally tiny.
Its structure is optimized for:

* high-speed parsing
* predictable transformations
* small program footprints
* clean AST manipulation
* mutation-friendly syntax

This makes it ideal for experimentation in autonomous code repair.

---

## **Why Mutagen?**

Traditional compilers tell you what’s wrong.
Mutagen fixes it, **even when the source is broken**.

The system:

* takes faulty Genex code
* mutates and iteratively transforms it
* evaluates candidate solutions
* converges toward a valid, compiling program

The result is a compiler that behaves more like a living system than a static toolchain.

---

## **Project Structure: Experimental**

```
mutagen/
│
├── core/
│   ├── lexer/
│   ├── parser/
│   ├── ast/
│   ├── tokens/
│   └── utils/
│
├── genex/
│   ├── language_spec/
│   ├── std/
│   └── examples/
│
├── evolution/
│   ├── engine/
│   ├── operators/
│   └── fitness/
│
└── docs/
```

---

## **Status**

Actively under development.
Core components are being built from the ground up with a focus on portability, clarity, and experimental flexibility.

---

## **License**

**All Rights Reserved.**

---

## **Disclaimer**

Mutagen and Genex are experimental.
They are built for research, engineering exploration, and—let’s be honest—a bit of chaos.
Any kind of modification/un-authorized usage/distribution is sctrictly prohibited.
