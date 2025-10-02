# CmdFlix

CmdFlix is a lightweight remote control server written in C for controlling Netflix playback on macOS using the Brave browser. It listens on a TCP port and accepts specially crafted HTTP GET requests to execute AppleScript commands that control browser actions and system volume.

---

### Features

- Activate or launch Brave Browser
- Play/Pause Netflix playback via space keypress
- Trigger Netflix "Next Episode" button click via JavaScript injection
- Adjust system volume up or down in increments of 5 units
- Simple HTTP server interface on port 30303

---

### How It Works

- **Networking:** Uses POSIX sockets to create a TCP server listening on port 30303. It accepts connections and reads HTTP GET requests.
- **HTTP Parsing:** The server parses incoming request strings for the pattern `/cmd?cmd=COMMAND`, extracting the requested command.
- **Command Validation:** Commands are validated against a white-list of allowed commands:
  - `brave_focus`
  - `brave_playpause`
  - `brave_netflix_next`
  - `volume_up`
  - `volume_down`
- **URL Decoding:** The GET parameters are URL decoded to support special characters.
- **AppleScript Integration:** For valid commands, a corresponding AppleScript string is generated and executed via popen():
  - Focus or launch Brave Browser
  - Send space key to toggle play/pause
  - Use JavaScript to click Netflix's next episode button
  - Adjust macOS system volume up/down by 5 units

- **Response Handling:** Returns HTTP 200 OK responses with success or error messages in HTML.
- **Static Serving:** Serves `index.html` for requests.

---

### Installation

Compile on macOS with:

`make`

Run with:

`./cmdflix`

Go to:

`localhost:30303`

---

### Usage

Send HTTP GET requests to control Netflix remotely:

- Focus Brave Browser: `http://localhost:30303/cmd?cmd=brave_focus`
- Play/Pause: `http://localhost:30303/cmd?cmd=brave_playpause`
- Next Episode: `http://localhost:30303/cmd?cmd=brave_netflix_next`
- Volume Up: `http://localhost:30303/cmd?cmd=volume_up`
- Volume Down: `http://localhost:30303/cmd?cmd=volume_down`

---

### Security

Only predefined commands are accepted, preventing arbitrary command execution. Avoid exposing this server to untrusted networks.

---

### Dependencies

- macOS with Brave Browser installed
- Standard C libraries and network/socket support
- AppleScript (`osascript`)

---

### Supported Commands & Effects

| Command             | Effect                                              |
|---------------------|-----------------------------------------------------|
| `brave_focus`       | Launches or focuses Brave Browser                   |
| `brave_playpause`   | Simulates space key to play/pause Netflix           |
| `brave_netflix_next`| Executes JS to press Netflix's next episode button  |
| `volume_up`         | Increases system volume by 5 (max 100)              |
| `volume_down`       | Decreases system volume by 5 (min 0)                |

---

##  index.html

This is the simple web interface designed to control the Netflix playback remotely through `cmdflix` server. It communicates by sending HTTP GET requests with commands, providing a user friendly visual control panel.

---

### Layout and Styling

- The page uses a minimalist, style reminiscent of old terminal screens: black background with glowing green text and borders.
- The entire content is centered vertically and horizontally using flexbox for easy viewing on any screen size.
- Buttons are arranged in a neat 2x3 grid with spacing to separate control functions visually.
- Each button has a transparent background with green borders and text that glow and invert colors on hover for clear interactivity cues.
- The font uses monospace `'Courier New'`, which complements the terminal aesthetic.
- A status text area below the buttons displays feedback messages about the command sent and the server response.

---

### Controls and Interaction

- Five main buttons are provided:
  - Play / Pause
  - Next Track
  - Volume Up
  - Volume Down
  - Focus (to bring Brave browser to front)
- Each button has an `id` and a click event listener attached via JavaScript.

---

### JavaScript Functionality

- The `sendCommand(cmd)` function sends an asynchronous HTTP GET request to `/cmd?cmd=COMMAND` on the server for the given command.
- On clicking a button, the corresponding command string is passed to `sendCommand`.
- While the request is in progress, a status message shows "Sending [command]...".
- When the server responds:
  - If successful (HTTP 200), the returned response text is shown in the status area. 
  - If there's an error (non-200 status or request failure), an error message is displayed.

---

### How It Works Together

- The HTML front end presents an easy to use controller interface compatible with `cmdflix` command set.
- Users interact by clicking buttons; JavaScript sends the mapped commands over HTTP to the `cmdflix` server.
- `cmdflix` executes those commands on `macOS` to control Brave browser and volume as requested.
- Feedback from `cmdflix` is shown live to the user, giving clear and instant confirmation or error messages.

---

## Trademark Disclaimer

This software project, CmdFlix, makes reference to certain trademarks which are the property of their respective owners. Specifically, "Netflix," "macOS," and "Brave" are registered trademarks or service marks belonging to their respective companies and entities.

This project is an independent and unofficial tool developed for educational and personal use only. It is **not affiliated with, endorsed by, sponsored by, or in any way officially connected to Netflix, Apple Inc. (macOS), or Brave Software**. Any use of these trademarks is solely for the purpose of describing compatibility and integration with their respective products and platforms.

The trademarks, and brand names referenced here are used purely for descriptive purposes to identify the platforms and services the software interacts with, and do not imply any partnership, collaboration, or approval by the trademark owners.

Users and distributors of this software are expressly reminded that the legal rights over these trademarks belong solely to the trademark holders. This project does not claim ownership, permission, or any rights to use the trademarks beyond what is permitted under applicable trademark law, including nominative fair use.

If you distribute, modify, or publicly display this software, please ensure that all trademark references clearly maintain these disclaimers and are not used in ways that might mislead others into believing there is a commercial relationship or endorsement.

By using this software, you agree to respect all trademark rights of the respective owners and acknowledge that this project’s author bears no responsibility for any trademark infringement claims or disputes that arise from improper use of these trademarks.

---

## License

This project is provided under the GPL3 License.

---

## User Interface

![UI Screenshot](https://github.com/user-attachments/assets/d4de2bd2-5764-45bf-86df-bd7b5c887587)

---
