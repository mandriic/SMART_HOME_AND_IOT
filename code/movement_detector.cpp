/*
  Simple PIR motion detector example for ESP8266 D1 Mini / Wemos D1

  Wiring:
  - PIR VCC -> 3.3V (or 5V if your module requires it; see note below)
  - PIR GND -> GND
  - PIR OUT -> D2 (GPIO4)
  - The sketch uses the built-in LED on D4 (GPIO2)
  
  Note: If your PIR module is powered from 5V and its output is 5V, use a level
  shifter or a simple voltage divider on the OUT line before connecting to the
  ESP8266 input to avoid damaging the board.
  */
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

// Use the Dn names that map nicely to D1 Mini silk labels
const uint8_t PIR_PIN = D2; // connect PIR output here
const uint8_t LED_PIN = D4; // built-in LED on many D1 Mini boards (often active LOW)

// MQTT client setup
WiFiClient espClient;
PubSubClient client(espClient);
//У скетчі тобі треба вказати:
//IP-адресу твоєї Raspberry Pi.
//Порт (за замовчуванням 1883).



unsigned long lastEvent = 0;
const unsigned long debounceMs = 200;
const unsigned long motionHoldMs = 3000; // keep LED on for 3 seconds after detection

// Wi-Fi credentials (edit before uploading)
const char* WIFI_SSID = "*********";
const char* WIFI_PASS = "*********";

ESP8266WebServer server(80);
bool modulePresent = true; // set at boot if PIR seems connected
bool motionDetected = false;
// Buzzer configuration: connect active buzzer to this pin (use transistor for passive buzzer)
const uint8_t BUZZER_PIN = D3;
bool buzzerEnabled = true;

// Many D1 Mini / Wemos boards have the on-board LED wired active LOW.
// Define helper macros for clarity.
#define LED_ON()  digitalWrite(LED_PIN, LOW)
#define LED_OFF() digitalWrite(LED_PIN, HIGH)
void reconnect() {
  // Цикл, поки не підключимося
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Намагаємося підключитися (вказуємо унікальне ім'я клієнта)
    if (client.connect("ESP8266_Motion_Sensor")) {
      Serial.println("connected");
      // Можна опублікувати щось при старті
      client.publish("device/status", "online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  LED_OFF();
  Serial.println("PIR motion detector starting (HC-SR501)");
  // Configure MQTT broker (replace with your broker IP/port)
  client.setServer("192.168.1.132", 1883);

  // Quick sanity check: if the PIR output is HIGH at boot and stays
  // high during a short sampling window, warn the user (likely floating
  // input or wiring/power issue). Hardware fix: add a 10K pulldown from
  // PIR OUT to GND so pin reads LOW when module is disconnected.
  bool stuckHigh = true;
  for (int i = 0; i < 10; ++i) {
    if (digitalRead(PIR_PIN) == LOW) {
      stuckHigh = false;
      break;
    }
    delay(10);
  }
  if (stuckHigh) {
    Serial.println("Warning: PIR pin reads HIGH on boot — check wiring or add 10K pulldown");
    modulePresent = false;
  } else {
    modulePresent = true;
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi '"); Serial.print(WIFI_SSID); Serial.println("'...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long wifiStart = millis();
  const unsigned long wifiTimeout = 15000;
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart) < wifiTimeout) {
    Serial.print('.');
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Wi-Fi connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Wi-Fi not connected (check credentials)");
  }

  // Web server routes
  server.on("/", []() {
    String page = "<html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">";
    page += "<title>PIR Motion Detector</title></head><body style=\"font-family:Arial,sans-serif;line-height:1.4;\">";
    page += "<h2>PIR Motion Detector</h2>";
    page += "<p><b>Wi-Fi:</b> ";
    page += (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "Not connected";
    page += "</p>";
    page += "<p><b>Module present:</b> ";
    page += (modulePresent ? "Yes" : "No");
    page += "</p>";
    page += "<p><b>Motion:</b> <span id=\"motion\">";
    page += (motionDetected ? "DETECTED" : "No");
    page += "</span></p>";
    if (lastEvent != 0) {
      page += "<p>Last: ";
      page += String(lastEvent);
      page += " ms</p>";
    }
    page += "<p><label><input type=\"checkbox\" id=\"soundToggle\" checked> Enable browser sound</label></p>";
    page += "<p><small>Auto-updates every second.</small></p>";
    page += R"rawliteral(<script>
      (function(){
        var prevMotion = false;
        function beep(){
          try{
            var ctx = new (window.AudioContext || window.webkitAudioContext)();
            var o = ctx.createOscillator();
            var g = ctx.createGain();
            o.type = 'sine';
            o.frequency.value = 2000;
            g.gain.value = 0.08;
            o.connect(g); g.connect(ctx.destination);
            o.start(0);
            setTimeout(function(){ o.stop(); ctx.close(); }, 300);
          }catch(e){ console.log('beep error', e); }
        }
        async function poll(){
          try{
            var res = await fetch('/status',{cache:'no-store'});
            if(!res.ok) return;
            var j = await res.json();
            var m = !!j.motion;
            document.getElementById('motion').textContent = m ? 'DETECTED' : 'No';
            var soundEnabled = document.getElementById('soundToggle').checked;
            if(m && !prevMotion && soundEnabled){ beep(); }
            prevMotion = m;
          }catch(e){ console.log('poll error', e); }
        }
        setInterval(poll,1000);
        // initial poll
        poll();
      })();
    </script>)rawliteral";
    page += "</body></html>";
    server.send(200, "text/html", page);
  });

  server.on("/status", []() {
    String s = "{";
    s += "\"motion\":";
    s += (motionDetected ? "true" : "false");
    s += ",\"ip\":\"";
    s += WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "";
    s += "\"}";
    server.send(200, "application/json", s);
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  server.handleClient();

  int v = digitalRead(PIR_PIN);
  static int consecutiveHigh = 0;

  // 1. Фільтрація шуму
  if (v == HIGH) {
    consecutiveHigh++;
  } else {
    consecutiveHigh = 0;
  }

  // 2. Логіка детекції (Trigger)
  // Додаємо перевірку !motionDetected, щоб не спрацьовувало по колу, поки горить LED
  if (consecutiveHigh >= 3 && !motionDetected) { 
    if (millis() - lastEvent > debounceMs) {
      Serial.println("Motion detected");
      LED_ON();
      client.publish("home/livingroom/motion", "ON");
      
      lastEvent = millis();
      motionDetected = true;
      
      if (buzzerEnabled) {
        tone(BUZZER_PIN, 2000, 300);
      }
      // ПРИБЕРИ delay(1000) звідси! Він блокує все.
    }
    consecutiveHigh = 0;
  }

  // 3. Логіка вимкнення (Reset)
  // Вимикаємо через motionHoldMs після останнього спрацювання
  if (motionDetected && (millis() - lastEvent >= motionHoldMs)) {
    LED_OFF();
    noTone(BUZZER_PIN);
    motionDetected = false;
    // НЕ СКИДАЙ lastEvent в 0. Нехай він зберігає час останнього руху для дебаунсу.
    client.publish("home/livingroom/motion", "OFF"); // корисно для статусу
  }

  delay(50); 
}

// put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   int result = myFunction(2, 3);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
// }

// put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }