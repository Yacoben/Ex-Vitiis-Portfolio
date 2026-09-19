# **Ex Vitiis** — Source Code Portfolio

> **Ex Vitiis** is a dark fantasy first-person dungeon crawler with brutal souls-like combat, procedurally generated characters, and monsters pulled straight from the depths of the twisted medieval imagination.

---

## My role in the project

I'm the main gameplay programmer on the team. As is typical in a small team, I handle a bit of everything.

Most of my time has gone into the AI system, which is built on **Goal Oriented Action Planning (GOAP)**. GOAP runs without the built-in debugging layer that Behavior Trees or State Trees provide, so I built my own debugger using the **ImGUI** plugin, which makes working with it far easier.

I also work closely with the game designer, providing tools that support production: a full set of cheats **(Cheat Manager)**, custom debuggers **(ImGui)**, and **CVars** that allow values to be changed in real time without restarting the game.

I take an active part in writing both technical **(TDD)** and game design **(GDD)** documentation.

I collaborate with the animators on both technical and design matters, and with the audio designer, together implementing the audio system using **Wwise**.

I'm also responsible for level design and lighting. I went with **Lumen + MegaLights**, mainly because the game uses very enclosed spaces and a large number of small light sources (torches, candles), which MegaLights handles extremely well. I wanted fully dynamic lighting with no baking, given how heavily the levels rely on object physics. Level Streaming also helps maintain performance and is a perfect fit for linear dungeon crawlers. While learning lighting, I spent a lot of time on Lumen optimixation and documented everything I found in /Docs/LumenOptimization.md in this repository. Worth noting that I make extensive use of AI (Claude/Cursor/Copilot) when writing documentation. My end goal is to keep Lumen under 5 ms of the 16 ms frame budget (60 FPS).


## Summary

- Built in Unreal Engine 5
- Based on the Gameplay Ability System framework
- UI built on Common UI + MVVM
- AI using Goal Oriented Action Planning + a custom movement component (root motion animations)
- Lighting: Lumen + MegaLights


## Note

This is a **source-code-only** portfolio repository. It is not intended to compile standalone.
The code is shared to demonstrate knowledge.

---

## License

This code is shared for **portfolio and review purposes only**.  
All rights reserved. Do not use in commercial or personal projects without permission.

