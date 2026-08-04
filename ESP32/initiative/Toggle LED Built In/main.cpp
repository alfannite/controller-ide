#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// ==== GANTI SESUAI WIFI KAMU ====
const char* WIFI_SSID     = "OKEGAS";
const char* WIFI_PASSWORD = "12345678";

#define LED_PIN LED_BUILTIN 

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool ledState = false;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Aurum Switch</title>
  <style>
    :root{
      --bg-0:#0b0a08;
      --bg-1:#141210;
      --panel:#1c1916;
      --hairline:#332e27;
      --brass:#c9a35f;
      --brass-bright:#e8c98a;
      --brass-dim:#7a6540;
      --text:#efe8db;
      --muted:#8c8377;
      --on-glow:rgba(232,201,138,0.55);
      --off-shadow:rgba(0,0,0,0.6);
    }
    *{box-sizing:border-box;}
    html,body{height:100%;}
    body{
      margin:0;
      min-height:100vh;
      display:flex;
      flex-direction:column;
      align-items:center;
      justify-content:center;
      gap:36px;
      font-family:Georgia,'Iowan Old Style','Times New Roman',serif;
      background:
        radial-gradient(circle at 50% 20%, #1c1815 0%, var(--bg-1) 45%, var(--bg-0) 100%);
      color:var(--text);
      overflow:hidden;
      -webkit-tap-highlight-color:transparent;
    }

    /* faint ambient grain / vignette, pure CSS, no assets */
    body::before{
      content:"";
      position:fixed; inset:0;
      background:
        radial-gradient(circle at 50% 100%, rgba(201,163,95,0.06), transparent 60%);
      pointer-events:none;
    }

    .eyebrow{
      font-family:'Trebuchet MS',system-ui,sans-serif;
      font-size:11px;
      letter-spacing:.42em;
      text-transform:uppercase;
      color:var(--brass-dim);
      transition:color .6s ease;
    }
    .eyebrow.live{ color:var(--brass); }

    h1{
      margin:0;
      font-size:22px;
      font-weight:400;
      letter-spacing:.06em;
      color:var(--text);
    }
    h1 em{
      font-style:italic;
      color:var(--brass-bright);
    }

    /* ---- the dial ---- */
    .dial-wrap{
      position:relative;
      width:220px;
      height:220px;
      display:flex;
      align-items:center;
      justify-content:center;
    }

    .ring{
      position:absolute;
      inset:0;
      border-radius:50%;
      padding:3px;
      background:conic-gradient(from 0deg,
        var(--brass-dim), var(--hairline) 15%, var(--brass-dim) 30%,
        var(--hairline) 45%, var(--brass-dim) 60%, var(--hairline) 75%,
        var(--brass-dim) 90%, var(--brass-dim) 100%);
      -webkit-mask:
        linear-gradient(#000 0 0) content-box,
        linear-gradient(#000 0 0);
      -webkit-mask-composite:xor;
      mask-composite:exclude;
      opacity:.55;
      transition:opacity .6s ease;
      animation:slowspin 40s linear infinite;
    }
    .dial-wrap.on .ring{ opacity:1; }

    @keyframes slowspin{ to{ transform:rotate(360deg); } }

    .glow{
      position:absolute;
      inset:-30px;
      border-radius:50%;
      background:radial-gradient(circle, var(--on-glow) 0%, transparent 68%);
      opacity:0;
      transition:opacity .7s ease;
      filter:blur(2px);
    }
    .dial-wrap.on .glow{ opacity:1; animation:breathe 3.2s ease-in-out infinite; }

    @keyframes breathe{
      0%,100%{ transform:scale(1); opacity:.75; }
      50%{ transform:scale(1.08); opacity:1; }
    }

    .btn{
      position:relative;
      width:172px; height:172px;
      border-radius:50%;
      border:1px solid var(--hairline);
      background:
        radial-gradient(circle at 35% 30%, #26221d 0%, var(--panel) 55%, #14110e 100%);
      box-shadow:
        inset 0 1px 1px rgba(255,255,255,0.05),
        inset 0 -12px 24px rgba(0,0,0,0.55),
        0 10px 30px var(--off-shadow);
      cursor:pointer;
      display:flex;
      align-items:center;
      justify-content:center;
      transition:box-shadow .5s ease, transform .15s ease;
      -webkit-appearance:none;
      appearance:none;
      padding:0;
    }
    .btn:active{ transform:scale(.97); }

    .dial-wrap.on .btn{
      border-color:var(--brass);
      box-shadow:
        inset 0 1px 1px rgba(255,255,255,0.08),
        inset 0 -10px 22px rgba(0,0,0,0.4),
        0 0 40px var(--on-glow),
        0 10px 30px var(--off-shadow);
    }

    .btn-core{
      display:flex;
      flex-direction:column;
      align-items:center;
      gap:8px;
      font-family:'Trebuchet MS',system-ui,sans-serif;
    }

    .state-word{
      font-size:13px;
      letter-spacing:.35em;
      color:var(--muted);
      transition:color .5s ease;
    }
    .dial-wrap.on .state-word{ color:var(--brass-bright); }

    .power-glyph{
      width:26px; height:26px;
      stroke:var(--muted);
      stroke-width:1.6;
      fill:none;
      transition:stroke .5s ease, filter .5s ease;
    }
    .dial-wrap.on .power-glyph{
      stroke:var(--brass-bright);
      filter:drop-shadow(0 0 6px var(--on-glow));
    }

    /* ---- status line ---- */
    .status-row{
      display:flex;
      align-items:center;
      gap:9px;
      font-family:'Trebuchet MS',system-ui,sans-serif;
      font-size:12px;
      letter-spacing:.08em;
      color:var(--muted);
    }
    .dot{
      width:6px; height:6px;
      border-radius:50%;
      background:#5c5445;
      transition:background .4s ease, box-shadow .4s ease;
    }
    .dot.connected{
      background:#8fbf7a;
      box-shadow:0 0 6px rgba(143,191,122,.8);
    }
    .dot.pulse{ animation:blink 1s ease-in-out infinite; }
    @keyframes blink{ 50%{ opacity:.25; } }

    footer{
      position:fixed;
      bottom:18px;
      font-family:'Trebuchet MS',system-ui,sans-serif;
      font-size:10px;
      letter-spacing:.25em;
      color:#4a443b;
      text-transform:uppercase;
    }

    @media (prefers-reduced-motion: reduce){
      .ring{ animation:none; }
      .glow{ animation:none; }
    }
  </style>
</head>
<body>

  <div class="eyebrow" id="eyebrow">FAN PROJECT</div>
  <h1>ESP-FanDev <em>Switch</em></h1>

  <div class="dial-wrap off" id="dialWrap">
    <div class="ring"></div>
    <div class="glow"></div>
    <button class="btn" id="toggleBtn" onclick="toggleLED()" aria-label="Toggle light">
      <span class="btn-core">
        <svg class="power-glyph" viewBox="0 0 24 24">
          <path d="M12 3v8" stroke-linecap="round"/>
          <path d="M6.3 6.3a9 9 0 1 0 11.4 0" stroke-linecap="round"/>
        </svg>
        <span class="state-word" id="stateWord">OFF</span>
      </span>
    </button>
  </div>

  <div class="status-row">
    <span class="dot pulse" id="statusDot"></span>
    <span id="statusText">Menghubungkan…</span>
  </div>

  <footer>ESP32 &middot; Fan Controller</footer>

<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;

  function initWebSocket() {
    setDot('connecting');
    websocket = new WebSocket(gateway);

    websocket.onopen = () => {
      setDot('connected');
      document.getElementById('statusText').innerText = 'Terhubung';
      document.getElementById('eyebrow').classList.add('live');
    };

    websocket.onclose = () => {
      setDot('connecting');
      document.getElementById('statusText').innerText = 'Terputus, menyambung ulang…';
      document.getElementById('eyebrow').classList.remove('live');
      setTimeout(initWebSocket, 2000);
    };

    websocket.onmessage = (event) => {
      updateButton(event.data === '1');
    };
  }

  function setDot(state){
    const dot = document.getElementById('statusDot');
    dot.className = 'dot';
    if (state === 'connected') dot.classList.add('connected');
    else dot.classList.add('pulse');
  }

  function updateButton(isOn) {
    const wrap = document.getElementById('dialWrap');
    const word = document.getElementById('stateWord');
    if (isOn) {
      wrap.classList.add('on'); wrap.classList.remove('off');
      word.innerText = 'ON';
    } else {
      wrap.classList.remove('on'); wrap.classList.add('off');
      word.innerText = 'OFF';
    }
  }

  function toggleLED() {
    if (websocket && websocket.readyState === WebSocket.OPEN) {
      websocket.send('toggle');
    }
  }

  window.onload = initWebSocket;
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  ws.textAll(ledState ? "1" : "0");
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client #%u connected\n", client->id());
    client->text(ledState ? "1" : "0"); // kirim status awal ke client baru
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client #%u disconnected\n", client->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String msg = String((char*)data, len);
      if (msg == "toggle") {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        Serial.println(ledState ? "LED ON" : "LED OFF");
        notifyClients();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  });

  server.begin();
  Serial.println("Server started. Buka IP di atas lewat browser.");
}

void loop() {
  ws.cleanupClients();
}
