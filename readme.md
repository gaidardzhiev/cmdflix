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

### How It Works (Low-Level Details)

- **Networking:** Uses POSIX sockets to create a TCP server listening on port 30303. It accepts connections and reads HTTP GET requests.  
- **HTTP Parsing:** The server parses incoming request strings for the pattern `/cmd?cmd=COMMAND`, extracting the requested command.  
- **Command Validation:** Commands are validated against a white-list of allowed commands:  
  - brave_focus  
  - brave_playpause  
  - brave_netflix_next  
  - volume_up  
  - volume_down  
- **URL Decoding:** The GET parameters are URL-decoded to support special characters.  
- **AppleScript Integration:** For valid commands, a corresponding AppleScript string is generated and executed via popen():  
  - Focus or launch Brave Browser  
  - Send space key to toggle play/pause  
  - Use JavaScript to click Netflix's next episode button  
  - Adjust macOS system volume up/down by 5 units  

- **Response Handling:** Returns HTTP 200 OK responses with success or error messages in HTML.  
- **Static Serving:** Serves `index.html` for root requests if present.  

---

### Installation

Compile on macOS with:

`make`

Run with:

`./cmdflix`

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
- AppleScript (`osascript` command)  

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
