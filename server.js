const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");

const app = express();
const port = 5000;

// Store the latest command (volatile)
let latestCommand = "";
let alarmTime = null;
app.use(cors());
// new line
app.use(express.static("public"))
app.use(bodyParser.json());


let deviceStates = {
  fan: "OFF",
  light: "OFF",
  security: "DISARMED"
};

let sensorData = {
  temp: "--",
  humidity: "--",
  soil: "--"
};

// Utility to extract time from text
function extractAlarmTime(text) {
  const match = text.match(/(\d{1,2})[: ]?(\d{2})/); // Supports 5:08, 508, 5 08
  if (!match) return null;

  let hour = parseInt(match[1]);
  let minute = parseInt(match[2]);

  // Optional: detect AM/PM if present
  if (/pm/.test(text.toLowerCase()) && hour < 12) hour += 12;
  if (/am/.test(text.toLowerCase()) && hour == 12) hour = 0;

  return { hour, minute };
}

app.post("/command", (req, res) => {
  const { cmd } = req.body;
  if (!cmd) return res.status(400).send("❌ Invalid command");

  const command = cmd.toLowerCase();

  // 🔔 Alarm detection
  if (command.includes("set alarm for")) {
    const time = extractAlarmTime(command);
    if (time && time.hour >= 0 && time.hour < 24 && time.minute >= 0 && time.minute < 60) {
      const formatted = `${time.hour.toString().padStart(2, '0')}:${time.minute.toString().padStart(2, '0')}`;
      latestCommand = `set alarm for ${formatted}`;
      console.log("⏰ Alarm set for", formatted);
      return res.send("⏰ Alarm set for " + formatted);
    } else {
      return res.send("⚠️ Could not understand the alarm time.");
    }
  }

  // Normal case
  latestCommand = command;
  console.log("📥 Received command:", latestCommand);
  res.send("✅ Command received: " + latestCommand);
});

// Endpoint ESP32 can call to get latest command
app.get("/command", (req, res) => {
  res.json({ cmd: latestCommand });
  // Optionally clear after reading:
  // latestCommand = "";
});

let latestESPResponse = "";
app.post("/response", (req, res) => {
  res.send("Received");
  const { data } = req.body;
  console.log("📥 ESP32 Response:", data);
  latestESPResponse = data;
});

// frontend polls this to get updated response
app.get("/latest-response", (req, res) => {
  res.json({ response: latestESPResponse });
});

app.get("/devices", (req, res) => {
  res.json(deviceStates);
});

app.get("/sensor", (req, res) => {
 res.json(sensorData);
});

// Endpoint for ESP32 to send device state updates
app.post("/devices", (req, res) => {
 const { fan, light, security } = req.body;

 if (fan !== undefined) deviceStates.fan = fan;
 if (light !== undefined) deviceStates.light = light;
 if (security !== undefined) deviceStates.security = security;

 console.log("🔧 Device states updated:", deviceStates);
 res.send("✅ Device states received");
});

app.post("/sensor", (req, res) => {
 const { temp, humidity, soil } = req.body;

 if (temp !== undefined) sensorData.temp = temp;
 if (humidity !== undefined) sensorData.humidity = humidity;
 if (soil !== undefined) sensorData.soil = soil;

 console.log("📡 Sensor data updated:", sensorData);
 res.send("✅ Sensor data received");
});


app.get("/alarm", (req, res) => {
 if (latestCommand.startsWith("set alarm for")) {
  res.json({ alarm: latestCommand.replace("set alarm for", "").trim() });
 } else {
  res.json({ alarm: null });
 }
});


app.listen(port, () => {
  console.log(`🚀 Voice Command Server running at http://localhost:${port}`);
});
