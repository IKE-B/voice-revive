# voice-revive

This project aims to **improve the speech quality of laryngectomy patients** by developing a smartphone application.

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Documentation](#documentation)
  - [Developer Documentation](#developer-documentation)
  - [User Documentation](#user-documentation)
  - [Hardware-Setup](#hardware-setup)
- [Acknowledgements](#acknowledgements)

## Overview

Due to the removal of the larynx during this procedure, the **speech capabilities** of these patients are **severely impaired**.
While current solutions can restore their speech in a way, patients report problems like not being able to speak loudly, having a deep and rough voice, fast speaking fatigue, communication issues with strangers, ...

The goal of this project is to develop a **smartphone application** where:
1. users can speak with less (fatiguing) loudness
2. into a **microphone**, which sends the audio signal to
3. a smartphone that performs **audio transformations** on the incoming audio-stream in **real-time**, and
4. outputs the transformed audio to a **speaker** that drowns out the original voice.

```mermaid
%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'fontSize': '56px',
      'lineColor': '#46AFF0'
    }
  }
}%%

  flowchart LR
  
    A([👤💬])
    A --> B([🎤])
    B --> C([📱⚙️])
    C --> D([🔊])
```

The app provides multiple degrees of freedom for users to adjust their voice transformation to their liking.
This solution offers an improvement of the speech quality **on top of current treatments without further surgery or medical intervention**.
It only needs relatively cheap, commonly available hardware.

The two biggest challenges are implementing audio transformations that can achieve the desired voice quality and ensuring that they are applied in **real-time**,
to avoid jamming the speech of the users themselves.
Additionally, the app must be developed with special care for **usability and accessibility**, as the target group consists of elderly people in most parts.

The app frontend is developed in [Flutter](https://flutter.dev/), so it can be deployed on both **Android and iOS** devices.
Audio processing is done in [JUCE](https://juce.com/), a *C++* framework for audio applications, which makes a real-time audio processing possible.

**Disclaimer: Due to complications in connecting the app frontend to the audio processing backend, the currently actual-in-use UI is a prototype entirely written in JUCE/C++!**

## Project Structure

`.github` contains repository related files like issue templates and workflows.

`.vscode` contains development related settings for Visual Studio Code.

`docs` contains documentation related files of the project. Do not consult the contents of this folder directly, but rather use the [human readable online documentation](https://ike-b.github.io/voice-revive/).

`juce-8.0.0-windows/JUCE` contains a complete clone of the JUCE framework and a setup tutorial to ease setting up the audio processing development environment. Consult [CONTRIBUTING.md](CONTRIBUTING.md#juce-development-environment) for further information. 

`revoiceme` contains the source code of the app frontend written in Flutter/Dart. [CONTRIBUTING.md](CONTRIBUTING.md#project-structure) explains its contents in more detail. 

`revoiceme_juce` contains the source code for the app backend, written in JUCE/C++ - as well as a prototype UI.

`CONTRIBUTING.md` holds developer documentation, like opening issues, **setting up the development environment**, integrating changes into the main codebase, etc.

## Documentation

### Developer Documentation

An up-to-date **code documentation** (of the app frontend) of this project is available online at: [ike-b.github.io/voice-revive](https://ike-b.github.io/voice-revive/).

The **issue board** of this project can be found [here](https://github.com/orgs/IKE-B/projects/6).

Consult the [CONTRIBUTING.md](CONTRIBUTING.md) for all further information about the complete development cycle of this project:

- from how to opening issues
- to **setting up the development environment**
- to eventually integrating changes into the main codebase.

### User Documentation

This section describes how to use the **JUCE/C++ UI prototype**.

The usage itself is quite intuitive.
After opening the app, the "Start/Stop" button can be used to toggle the voice changing.
If the app somehow detected the wrong speaker or microphone, the periphery can be changed in the tab "Audiogeraet" under "Input" and "Output".
Per default, the voice should sound nearly unaltered.
To tune voice manipulation to one's concrete needs, users can experiment with the settings under "Konfiguration".
Because of its pure experimental nature, the app currently does not store configurations after exiting it. 

Stopping the voice changing via the button causes the app to select the default devices again (instead of the last user selected devices).
So changing the periphery must be done while voice changing is active.

**While the usage itself is intuitive, finding a good configuration is not. This is something a professional needs to do.**

Here is a **quick start guide** to avoid infinite feedback loops:

1. Set your device volume to 0.
2. Set the *LowCutFreq* to a value between 2000 to 5000.
3. Set your device volume to your desired level.

### Hardware-Setup

To run the app succesfully on a smartphone, an **externally** connected **directional** mircophone and an **external** speaker are needed.
Additionally, to connect both at the same time, a 2-to-1 (AUX) adapter is needed as well.

We tested our app with the following equipment:

- a Google Pixel 5 with 8 GB RAM and 128 GB storage
- a [Rode AI-Micro](https://rode.com/de/interfaces-and-mixers/ai-series/ai-micro) as adapter for two parallel AUX-Inputs
- a [Giecy speech enhancer](https://www.amazon.de/dp/B07VRPY2DT/?tag=glv-21&ascsubtag=dd0c82bc-5c9a-4a14-86c2-2772675619be&th=1&linkCode=osi) as speaker and microphone

This is **not** an advertisement. This is just the hardware we used during development where we tested the functionality.
Every other smartphone, directional microphone, speaker, and 2-to-1 (AUX) adapter should work as well.

## Acknowledgements

We express our gratitude to the [Vogel Stiftung Dr. Eckernkamp](https://www.vogel-stiftung.de/) as well as the [ImDS](https://www.ukw.de/interdisziplinaere-einrichtungen/institut-fuer-medizinische-datenwissenschaften/startseite/#) for their support. Without their support, this project would not have been possible.
