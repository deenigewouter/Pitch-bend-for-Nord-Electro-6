# Pitchbend for the Nord Electro 6
A MIDI pitchbender for the Nord Electro 6

The Nord Electro 6D supports MIDI pitch bend on all sound engines, but only if the engine is mapped to an external keyboard (External KBD to LO). This device takes the MIDI out from the Electro, applies pitch bend and routes it back on a different channel. I've incorporated the following features:

- Vibrato with adjustable speed and sensitivity by pitching the stick up
- Noise pitch modulation by pitching the stick down

Click [here for a video demonstration](https://www.youtube.com/watch?v=N-VUnCClMB0&ab_channel=Wouter) . Noise modulation was not implemented at time of filming.

### TODO:
1. Find a better joystick. This one is pretty stiff and doesn't perfectly return center. This is solved with a deadzone but I'm not sure how long the joystick will last with extensive playing.
2. Either allow it to be powered via MIDI (The Electro outputs 3.3V), a 9V battery or a chargable battery. This will require a new housing as there is no room left.
3. Clean up the code

### Images
![Front view](https://github.com/deenigewouter/Pitch-bend-for-Nord-Electro-6/blob/main/images/pitchbender-1.jpg?raw=true)
Front view


![Back view](https://github.com/deenigewouter/Pitch-bend-for-Nord-Electro-6/blob/main/images/pitchbender-2.jpg?raw=true)
Back view


![Inside view](https://github.com/deenigewouter/Pitch-bend-for-Nord-Electro-6/blob/main/images/pitchbender-3.jpg?raw=true)
Inside view


![Circuit schematic](https://github.com/deenigewouter/Pitch-bend-for-Nord-Electro-6/blob/main/images/Circuit.png?raw=true)
Circuit schematic
