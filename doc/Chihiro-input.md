# HOD3 input

The input profile is selected from the media board's game executable, so
Segaboot and the game's test executable use the same controls as the game.
If the boot ID has no executable, the launched XBE path is used instead.
Profiles are generated in the Cxbx data directory on first launch:

- `chihiro_input_hod3xb.ini`: HOD3 when the boot ID specifies `hod3xb.xbe`.

Only HOD3 uses these profiles. Other games retain their existing controls.

An existing profile is preserved. Edits are reloaded once per second.
Profile section and key names use the spelling shown in the generated file.

| Action | Keyboard / mouse | XInput controller |
| --- | --- | --- |
| P1 / P2 start | 1 / 2 | Start on controller 1 / 2 |
| P1 / P2 coin | 5 / 6 | Back on controller 1 / 2 |
| Test / service | F1 / F2 | Configurable |
| P1 buttons 1–4 | A / S / D / F | A or RT / B / X / Y |
| P2 buttons 1–4 | J / K / L / P | A or RT / B / X / Y |
| HOD3 P1 aim | Mouse | Select stick sources in the profile |
| HOD3 P1 buttons 1 / 2 | Left / right mouse | A or RT / B |
| HOD3 P2 aim | — | Controller 2 left stick |

`[Analog1]` through `[Analog8]` correspond to JVS channels 1 through 8.
HOD3 defaults use channels 1–4 for P1 X/Y and P2 X/Y.
Axes are centered around 32768.
`Invert=1`, `Deadzone`, and `KeyDeflection` allow adjustments.
Supported sources are `MouseX`, `MouseY`, `LStickX`, `LStickY`,
`RStickX` and `RStickY`. An empty source disables device input.
`Pad=1` or `Pad=2` selects the controller for an analog channel.
`KeyMin` and `KeyMax` accept the same bindings as buttons.

Bindings can combine sources with commas, for example `Key.A,Pad.A,Mouse.Left`.
Supported named keys are arrows, Space, Enter, F1 and F2, plus single letters
and digits. Pad buttons include A, B, X, Y, Start, Back, the D-pad, LT and RT.

`[General] RequireFocus=1` disables input while the game window is in the
background. Set it to 0 to allow background keyboard/controller input.
Mouse aiming and clicks always require the render window to be foreground.
`MouseAspectRatio=0` follows the renderer's actual destination rectangle,
including letterboxing, fullscreen and changes in resolution. A positive
value overrides this with a centered image of that aspect ratio, for example
`1.333333` for 4:3. Mouse clicks outside the image are ignored.

These are host input defaults. Confirm channel assignments, button functions
and gun calibration in each game's test menu when testing a new revision.

## Region

The EEPROM editor's Xbox region now selects the Chihiro baseboard region:
Japan maps to 1, North America to 2, and Rest of World to 3 (Export).
Stop emulation, save the EEPROM region, then restart the game. Changing only
the EEPROM language or DVD region does not select a Chihiro territory.
If the game's boot ID explicitly excludes the requested territory, the
emulator retains a compatible region to avoid Error 05. When boot metadata
is unavailable, the EEPROM selection is used.
