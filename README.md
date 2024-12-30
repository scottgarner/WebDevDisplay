# WebDev Display

This document provides an overview of the custom display and its inner workings.

## Embedded Code

This project uses Platform.IO to build and deploy to an ESP32 on a custom breakout board. The embedded code is responsible for managing networking and display layers for pixel data.

The high-level flow of data looks like this:

1. A pixel buffer is generated client-side, consisting of a custom header followed by raw pixel data.
2. Header and pixel data is received on the ESP32 in `networking.cpp` via HTTP POST or Websockets.
3. This data is processed (and optionally saved) as the `graphicsBuffer`.
4. The display code in `display.cpp` loops continuously, sampling data from the `graphicsBuffer` and placing it in the `displayBuffer` based on a look-up table.

### Networking Layer

In addition to buffer reception/processing described above the networking layer has a few additional modules/functions.

- WiFiManager - Handles first-time setup when connecting to a new access point.
- ArduinoOTA - Allows for remote upload of embedded code and filesystem data.
- ESPAsyncWebServer - The web server library that handle incoming data and serve status/filesystem data.
- ESPmDNS - Exposes the host on the local networking using mDNS.

### Display Layer

As described above, the display layer directly manages updates to the LED strips based on data in the `graphicsBuffer`. It does this primarily by leveraging the FastLED library across multiple IO pins.

Additionally, it handles the following tasks:

1. Saving/loading buffer data from the local filesystem.
2. Basic animation tasks with scrolling on the X and Y axes.

## Graphics Buffer Format

The graphics buffer is simply a large byte array of RGB pixel values prefixed by a custom configuration header:

```
struct PixelDataHeader
{
    uint16_t mode;          // Reserved for support of different "modes".
    uint16_t width;         // Pixel width of the buffer.
    uint16_t height;        // Pixel height of the bufer.
    uint16_t scrollSpeedX;  // Scroll speed in pixels/second on X.
    uint16_t scrollSpeedY;  // Scroll speed in pixels/second on Y.
    uint16_t saveBuffer;    // Optionally save buffer, which is slow.
};
```

## Web Interface

This display was designed specifically with web-based control in mind. This repository includes simple example interface in `Web` that is also hosted on the device itself. Additionally, the class `scoreboard.ts` can be used in other projects to simplify generating the image buffer required by the sign.

The resulting buffer can then be posted to `/buffer` or sent via a WebSocket connection.

## Notes/Caveats

### Refresh Rate and Animation

Users can create animations by sending sequential image buffers at a fixed interval, though note the following limitations.

- In the current design, the image buffers are fully uncompressed and take a non-trivial amount of time to send over the ESP32's relatively slow WiFi connection. This speed is better over WebSockets than HTTP POST because of less overhead.
- Updating the LEDs themselves takes time given the length of the individual strips. As such, there's likely a hard cap to the framerate of 30-45 pixels without physically reworking the electronics.

If animation becomes a major focus, a major speed boost would come from reworking the software to use 8bit color buffers instead of 24bit color buffers.

### Heat/Safety Concerns

- The maximum brightness of the display is capped at below 100%. While all hardware is technically specced to run at full brightness, excess heat from the LEDs and other electrical components could reduce the running life of the electronics and potentially distort/deform plastic components.
- Additionally, it is not recommended to run the display with fully white (or near-white) image data for extended periods.
- It is not recommended to leave the display running for extended periods unattended in the off chance that a malfunction leaves it in a state (e.g. fully white) that could lead to heat related issues. For best results, power off the display when not in use.
- Sending a fully black buffer is another option, though the display pulls some power even when "black".
