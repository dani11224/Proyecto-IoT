#include "web_server.h"
#include "wifi_manager.h"

// ════════════════════════════════════════════════════════
//  HTML DEL DASHBOARD (invernadero operacional)
// ════════════════════════════════════════════════════════
String WebServerManager::_buildDashboardHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>🌿 Invernadero IoT</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap');

:root{
  --bg:#0a0f0d;
  --surface:#111a14;
  --card:#162019;
  --border:#1e3325;
  --green:#22c55e;
  --green2:#16a34a;
  --teal:#2dd4bf;
  --yellow:#facc15;
  --red:#f87171;
  --blue:#60a5fa;
  --text:#e2f0e8;
  --muted:#6b8f74;
  --mono:'JetBrains Mono',monospace;
}
*{margin:0;padding:0;box-sizing:border-box;}
body{
  font-family:'Space Grotesk',sans-serif;
  background:var(--bg);
  color:var(--text);
  min-height:100vh;
}

/* ── Header ── */
header{
  padding:16px 24px;
  border-bottom:1px solid var(--border);
  display:flex;
  align-items:center;
  justify-content:space-between;
  background:var(--surface);
}
.logo{font-size:20px;font-weight:700;letter-spacing:-0.5px;}
.logo span{color:var(--green);}
.status-dot{
  width:10px;height:10px;border-radius:50%;
  background:var(--green);
  box-shadow:0 0 8px var(--green);
  display:inline-block;margin-right:8px;
  animation:pulse 2s infinite;
}
@keyframes pulse{0%,100%{opacity:1;}50%{opacity:0.4;}}
.time-display{font-family:var(--mono);font-size:13px;color:var(--muted);}

/* ── Layout ── */
main{
  max-width:1100px;
  margin:0 auto;
  padding:24px 16px;
}

/* ── Modo banner ── */
.mode-bar{
  display:flex;
  align-items:center;
  justify-content:space-between;
  background:var(--card);
  border:1px solid var(--border);
  border-radius:14px;
  padding:14px 20px;
  margin-bottom:24px;
}
.mode-label{font-size:14px;color:var(--muted);}
.mode-value{font-weight:600;font-size:16px;}
.mode-auto{color:var(--green);}
.mode-manual{color:var(--yellow);}
.toggle-btn{
  padding:8px 20px;
  border-radius:8px;
  border:1px solid var(--border);
  background:transparent;
  color:var(--text);
  font-family:'Space Grotesk',sans-serif;
  font-size:14px;
  cursor:pointer;
  transition:.2s;
}
.toggle-btn:hover{background:var(--border);}

/* ── Grid sensores ── */
.grid-3{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(220px,1fr));
  gap:16px;
  margin-bottom:24px;
}
.sensor-card{
  background:var(--card);
  border:1px solid var(--border);
  border-radius:16px;
  padding:20px;
  position:relative;
  overflow:hidden;
}
.sensor-card::before{
  content:'';
  position:absolute;
  top:0;left:0;right:0;
  height:3px;
  border-radius:16px 16px 0 0;
}
.sensor-temp::before{background:linear-gradient(90deg,#f87171,#fb923c);}
.sensor-hum::before{background:linear-gradient(90deg,#60a5fa,#818cf8);}
.sensor-luz::before{background:linear-gradient(90deg,#facc15,#f97316);}

.sensor-icon{font-size:28px;margin-bottom:10px;}
.sensor-label{font-size:12px;color:var(--muted);text-transform:uppercase;letter-spacing:1px;margin-bottom:4px;}
.sensor-value{font-family:var(--mono);font-size:32px;font-weight:500;}
.sensor-unit{font-size:14px;color:var(--muted);margin-left:4px;}
.sensor-sub{font-size:12px;color:var(--muted);margin-top:6px;}

/* ── Actuadores ── */
.section-title{
  font-size:13px;
  text-transform:uppercase;
  letter-spacing:1.5px;
  color:var(--muted);
  margin-bottom:14px;
}
.grid-actuators{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(200px,1fr));
  gap:16px;
  margin-bottom:24px;
}
.act-card{
  background:var(--card);
  border:1px solid var(--border);
  border-radius:16px;
  padding:18px 20px;
  display:flex;
  align-items:center;
  justify-content:space-between;
}
.act-info{display:flex;align-items:center;gap:12px;}
.act-icon{font-size:24px;}
.act-name{font-size:15px;font-weight:500;}
.act-state{font-size:12px;font-family:var(--mono);}
.act-on{color:var(--green);}
.act-off{color:var(--muted);}

/* Toggle switch */
.switch{position:relative;display:inline-block;width:48px;height:26px;}
.switch input{opacity:0;width:0;height:0;}
.slider{
  position:absolute;inset:0;
  background:#1e3325;
  border-radius:26px;
  cursor:pointer;
  transition:.3s;
}
.slider:before{
  content:'';
  position:absolute;
  width:18px;height:18px;
  left:4px;bottom:4px;
  background:var(--muted);
  border-radius:50%;
  transition:.3s;
}
input:checked + .slider{background:var(--green2);}
input:checked + .slider:before{transform:translateX(22px);background:#fff;}
.switch-disabled .slider{opacity:0.4;cursor:not-allowed;}

/* ── Umbrales ── */
.umbral-grid{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(240px,1fr));
  gap:16px;
  margin-bottom:24px;
}
.umbral-card{
  background:var(--card);
  border:1px solid var(--border);
  border-radius:16px;
  padding:18px 20px;
}
.umbral-title{font-size:13px;color:var(--muted);margin-bottom:10px;}
.umbral-row{display:flex;align-items:center;gap:10px;}
.umbral-input{
  flex:1;
  background:#0d1a10;
  border:1px solid var(--border);
  border-radius:8px;
  padding:8px 12px;
  color:var(--text);
  font-family:var(--mono);
  font-size:14px;
}
.umbral-input:focus{outline:none;border-color:var(--green);}
.umbral-btn{
  padding:8px 14px;
  background:var(--green2);
  border:none;
  border-radius:8px;
  color:#fff;
  font-size:13px;
  cursor:pointer;
  transition:.2s;
}
.umbral-btn:hover{background:var(--green);}

/* ── Log ── */
.log-box{
  background:var(--card);
  border:1px solid var(--border);
  border-radius:16px;
  padding:18px 20px;
  margin-bottom:24px;
}
.log-entries{
  font-family:var(--mono);
  font-size:12px;
  color:var(--muted);
  max-height:120px;
  overflow-y:auto;
  display:flex;
  flex-direction:column-reverse;
}
.log-entry{padding:3px 0;border-bottom:1px solid var(--border);}
.log-entry:last-child{border:none;}
.log-ok{color:var(--green);}
.log-warn{color:var(--yellow);}
.log-err{color:var(--red);}

/* ── Toast ── */
.toast{
  position:fixed;bottom:24px;right:24px;
  background:var(--surface);
  border:1px solid var(--border);
  border-radius:12px;
  padding:12px 20px;
  font-size:14px;
  opacity:0;
  transform:translateY(10px);
  transition:.3s;
  pointer-events:none;
  z-index:999;
}
.toast.show{opacity:1;transform:translateY(0);}
</style>
</head>
<body>

<header>
  <div class="logo">🌿 <span>Invernadero</span> IoT</div>
  <div>
    <span class="status-dot" id="hcDot"></span>
    <span class="time-display" id="clockDisp">--:--:--</span>
  </div>
</header>

<main>

  <!-- Modo -->
  <div class="mode-bar">
    <div>
      <div class="mode-label">Modo de operación</div>
      <div class="mode-value" id="modeLabel">—</div>
    </div>
    <button class="toggle-btn" id="modeBtn" onclick="toggleMode()">Cambiar modo</button>
  </div>

  <!-- Sensores -->
  <div class="section-title">📡 Sensores</div>
  <div class="grid-3">
    <div class="sensor-card sensor-temp">
      <div class="sensor-icon">🌡️</div>
      <div class="sensor-label">Temperatura</div>
      <div><span class="sensor-value" id="valTemp">--</span><span class="sensor-unit">°C</span></div>
      <div class="sensor-sub" id="subTemp">DHT22</div>
    </div>
    <div class="sensor-card sensor-hum">
      <div class="sensor-icon">💧</div>
      <div class="sensor-label">Humedad</div>
      <div><span class="sensor-value" id="valHum">--</span><span class="sensor-unit">%</span></div>
      <div class="sensor-sub" id="subHum">DHT22</div>
    </div>
    <div class="sensor-card sensor-luz">
      <div class="sensor-icon">☀️</div>
      <div class="sensor-label">Luminosidad</div>
      <div><span class="sensor-value" id="valLuz">--</span><span class="sensor-unit">%</span></div>
      <div class="sensor-sub" id="subLuz">LDR ADC</div>
    </div>
  </div>

  <!-- Actuadores -->
  <div class="section-title">⚙️ Actuadores</div>
  <div class="grid-actuators">
    <div class="act-card">
      <div class="act-info">
        <span class="act-icon">💧</span>
        <div>
          <div class="act-name">Bomba</div>
          <div class="act-state act-off" id="stBomba">OFF</div>
        </div>
      </div>
      <label class="switch switch-disabled" id="swLabelBomba">
        <input type="checkbox" id="swBomba" onchange="ctrlActuator('bomba',this.checked)" disabled>
        <span class="slider"></span>
      </label>
    </div>
    <div class="act-card">
      <div class="act-info">
        <span class="act-icon">💡</span>
        <div>
          <div class="act-name">LED</div>
          <div class="act-state act-off" id="stLed">OFF</div>
        </div>
      </div>
      <label class="switch switch-disabled" id="swLabelLed">
        <input type="checkbox" id="swLed" onchange="ctrlActuator('led',this.checked)" disabled>
        <span class="slider"></span>
      </label>
    </div>
    <div class="act-card">
      <div class="act-info">
        <span class="act-icon">🌀</span>
        <div>
          <div class="act-name">Ventilador</div>
          <div class="act-state act-off" id="stVent">OFF</div>
        </div>
      </div>
      <label class="switch switch-disabled" id="swLabelVent">
        <input type="checkbox" id="swVentilador" onchange="ctrlActuator('ventilador',this.checked)" disabled>
        <span class="slider"></span>
      </label>
    </div>
  </div>

  <!-- Umbrales -->
  <div class="section-title">🎚️ Umbrales automáticos</div>
  <div class="umbral-grid">
    <div class="umbral-card">
      <div class="umbral-title">🌡️ Temp. máx. (°C) — activa ventilador</div>
      <div class="umbral-row">
        <input class="umbral-input" type="number" id="uTemp" step="0.5" value="30">
        <button class="umbral-btn" onclick="saveUmbral('temp')">OK</button>
      </div>
    </div>
    <div class="umbral-card">
      <div class="umbral-title">💧 Humedad mín. (%) — activa bomba</div>
      <div class="umbral-row">
        <input class="umbral-input" type="number" id="uHum" step="1" value="40">
        <button class="umbral-btn" onclick="saveUmbral('hum')">OK</button>
      </div>
    </div>
    <div class="umbral-card">
      <div class="umbral-title">☀️ Luz mín. (0-100%) — activa LED</div>
      <div class="umbral-row">
        <input class="umbral-input" type="number" id="uLuz" step="1" min="0" max="100" value="30">
        <button class="umbral-btn" onclick="saveUmbral('luz')">OK</button>
      </div>
    </div>
  </div>

  <!-- Log -->
  <div class="section-title">📋 Registro de eventos</div>
  <div class="log-box">
    <div class="log-entries" id="logBox"></div>
  </div>

</main>

<div class="toast" id="toast"></div>

<script>
let modoActual = "auto";

// ── Reloj ──────────────────────────────────
function updateClock(){
  document.getElementById("clockDisp").textContent =
    new Date().toLocaleTimeString("es-CO");
}
setInterval(updateClock, 1000);
updateClock();

// ── Toast ──────────────────────────────────
function showToast(msg, type="ok"){
  const t = document.getElementById("toast");
  t.textContent = msg;
  t.className = "toast show";
  setTimeout(()=>t.className="toast",2500);
}

// ── Log ────────────────────────────────────
function addLog(msg, type="ok"){
  const box = document.getElementById("logBox");
  const d = document.createElement("div");
  d.className = `log-entry log-${type}`;
  d.textContent = `[${new Date().toLocaleTimeString("es-CO")}] ${msg}`;
  box.prepend(d);
  if(box.children.length > 50) box.removeChild(box.lastChild);
}

// ── Fetch sensores ──────────────────────────
async function fetchSensors(){
  try{
    const r = await fetch("/api/sensors");
    const d = await r.json();
    document.getElementById("valTemp").textContent =
      d.temperatura > -100 ? d.temperatura.toFixed(1) : "ERR";
    document.getElementById("valHum").textContent =
      d.humedad > -100 ? d.humedad.toFixed(1) : "ERR";
    document.getElementById("valLuz").textContent = d.luzPct;
    document.getElementById("subLuz").textContent =
      `ADC: ${d.luzRaw} / 4095`;
  } catch(e){ addLog("Error leyendo sensores","err"); }
}

// ── Fetch actuadores / modo ─────────────────
async function fetchStatus(){
  try{
    const r = await fetch("/api/status");
    const d = await r.json();
    document.getElementById("hcDot").style.background = "#22c55e";

    modoActual = d.modo;
    const esManual = (modoActual === "manual");

    document.getElementById("modeLabel").textContent =
      esManual ? "🟡 MANUAL" : "🟢 AUTO";
    document.getElementById("modeLabel").className =
      `mode-value ${esManual ? "mode-manual" : "mode-auto"}`;

    // Switches: habilitar solo en manual
    ["Bomba","Led","Vent"].forEach(n=>{
      document.getElementById("swLabel"+n).className =
        "switch" + (esManual ? "" : " switch-disabled");
      document.getElementById("sw"+n).disabled = !esManual;
    });

    setActState("Bomba", d.bomba);
    setActState("Led",   d.led);
    setActState("Vent",  d.ventilador);

  } catch(e){
    document.getElementById("hcDot").style.background = "#f87171";
  }
}

function setActState(name, on){
  const sw = document.getElementById("sw"+(name==="Vent"?"Ventilador":name));
  const st = document.getElementById("st"+(name==="Vent"?"Vent":name.toLowerCase()));
  if(sw) sw.checked = on;
  if(st){
    st.textContent = on ? "ON" : "OFF";
    st.className   = on ? "act-state act-on" : "act-state act-off";
  }
}

// ── Cambiar modo ────────────────────────────
async function toggleMode(){
  const nuevo = modoActual === "auto" ? "manual" : "auto";
  try{
    await fetch("/api/control", {
      method:"POST",
      headers:{"Content-Type":"application/json"},
      body: JSON.stringify({accion:"modo", valor: nuevo})
    });
    addLog(`Modo cambiado a ${nuevo.toUpperCase()}`);
    showToast(`Modo → ${nuevo.toUpperCase()}`);
    fetchStatus();
  } catch(e){ showToast("Error cambiando modo","err"); }
}

// ── Control actuador manual ─────────────────
async function ctrlActuator(actuador, estado){
  try{
    await fetch("/api/control",{
      method:"POST",
      headers:{"Content-Type":"application/json"},
      body: JSON.stringify({accion: actuador, valor: estado ? "1" : "0"})
    });
    addLog(`${actuador} → ${estado?"ON":"OFF"}`);
    showToast(`${actuador} ${estado?"encendido":"apagado"}`);
  } catch(e){ showToast("Error enviando comando","err"); }
}

// ── Guardar umbral ──────────────────────────
async function saveUmbral(tipo){
  let body = {};
  if(tipo==="temp") body = {tempMax: parseFloat(document.getElementById("uTemp").value)};
  if(tipo==="hum")  body = {humedadMin: parseFloat(document.getElementById("uHum").value)};
  if(tipo==="luz"){
    const pct = parseInt(document.getElementById("uLuz").value);
    body = {luzMin: Math.round(pct * 40.95)};  // % → ADC 12bit
  }
  try{
    await fetch("/api/umbrales",{
      method:"POST",
      headers:{"Content-Type":"application/json"},
      body: JSON.stringify(body)
    });
    addLog(`Umbral actualizado: ${JSON.stringify(body)}`);
    showToast("✅ Umbral guardado");
  } catch(e){ showToast("Error guardando umbral","err"); }
}

// ── Polling ────────────────────────────────
fetchStatus();
fetchSensors();
setInterval(fetchSensors, 5000);
setInterval(fetchStatus,  5000);
addLog("Dashboard iniciado","ok");
</script>
</body>
</html>
)rawliteral";
}

// ════════════════════════════════════════════════════════
//  HTML DEL PORTAL CAPTIVO (configuración WiFi)
// ════════════════════════════════════════════════════════
String WebServerManager::_buildSetupHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>🌿 Invernadero — Setup WiFi</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;600;700&display=swap');
:root{--bg:#0a0f0d;--card:#162019;--border:#1e3325;--green:#22c55e;--text:#e2f0e8;--muted:#6b8f74;}
*{margin:0;padding:0;box-sizing:border-box;}
body{min-height:100vh;display:flex;align-items:center;justify-content:center;
  background:var(--bg);color:var(--text);font-family:'Space Grotesk',sans-serif;}
.card{width:90%;max-width:400px;background:var(--card);border:1px solid var(--border);
  border-radius:24px;padding:32px;animation:up .5s ease;}
@keyframes up{from{opacity:0;transform:translateY(16px)}to{opacity:1;transform:translateY(0)}}
h1{font-size:22px;margin-bottom:4px;}
.sub{color:var(--muted);font-size:13px;margin-bottom:24px;}
label{display:block;font-size:13px;color:var(--muted);margin-bottom:6px;}
select,input{width:100%;padding:12px;background:#0a0f0d;border:1px solid var(--border);
  border-radius:10px;color:var(--text);font-size:14px;margin-bottom:14px;font-family:inherit;}
select option{color:#000;}
select:focus,input:focus{outline:none;border-color:var(--green);}
button{width:100%;padding:13px;border:none;border-radius:10px;font-size:15px;
  font-weight:600;cursor:pointer;margin-top:4px;transition:.2s;font-family:inherit;}
.btn-primary{background:var(--green);color:#000;}
.btn-primary:hover{background:#16a34a;color:#fff;}
.btn-sec{background:transparent;border:1px solid var(--border);color:var(--text);margin-top:8px;}
.btn-sec:hover{background:var(--border);}
.btn-danger{background:#7f1d1d;color:#fca5a5;margin-top:8px;}
#estado{margin-top:14px;font-size:13px;text-align:center;min-height:20px;}
.loader{display:inline-block;width:14px;height:14px;border:2px solid rgba(255,255,255,.3);
  border-top-color:#fff;border-radius:50%;animation:spin 1s linear infinite;margin-right:6px;vertical-align:middle;}
@keyframes spin{to{transform:rotate(360deg)}}
</style>
</head>
<body>
<div class="card">
  <div style="font-size:40px;margin-bottom:12px">🌿</div>
  <h1>Invernadero IoT</h1>
  <div class="sub">Conecta el ESP32 a tu red WiFi</div>
  <label>Red WiFi</label>
  <select id="ssid"></select>
  <label>Contraseña</label>
  <input type="password" id="pwd" placeholder="Contraseña de la red">
  <button class="btn-primary" onclick="conectar()">Conectar</button>
  <button class="btn-sec" onclick="scan()">🔄 Actualizar redes</button>
  <button class="btn-danger" onclick="reset()">♻ Restablecer</button>
  <div id="estado"></div>
</div>
<script>
function rssiIcon(r){return r>=-55?"📶 Excelente":r>=-70?"📶 Buena":r>=-85?"📶 Media":"📶 Baja";}
function scan(){
  document.getElementById("estado").innerHTML='<span class="loader"></span>Buscando redes...';
  fetch('/scan').then(r=>r.json()).then(d=>{
    const sel=document.getElementById("ssid");
    sel.innerHTML="";
    d.forEach(n=>{const o=document.createElement("option");o.value=n.ssid;
      o.text=n.ssid+" • "+rssiIcon(n.rssi);sel.appendChild(o);});
    document.getElementById("estado").innerHTML="✅ Redes encontradas";
  }).catch(()=>document.getElementById("estado").innerHTML="❌ Error escaneando");
}
function conectar(){
  const s=document.getElementById("ssid").value;
  const p=document.getElementById("pwd").value;
  document.getElementById("estado").innerHTML='<span class="loader"></span>Conectando...';
  fetch('/connect',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ssid:s,password:p})})
  .then(r=>r.text()).then(m=>document.getElementById("estado").innerHTML=m)
  .catch(()=>document.getElementById("estado").innerHTML="❌ Error");
}
function reset(){
  if(confirm("¿Borrar configuración WiFi guardada?")){
    fetch('/reset').then(()=>document.getElementById("estado").innerHTML="♻ Reiniciando...");
  }
}
scan();
</script>
</body>
</html>
)rawliteral";
}

// ════════════════════════════════════════════════════════
//  BEGIN
// ════════════════════════════════════════════════════════
void WebServerManager::begin(Sensors* sensors, Actuators* actuators,
                              MqttManager* mqtt, bool configMode) {
  _sensors    = sensors;
  _actuators  = actuators;
  _mqtt       = mqtt;
  _configMode = configMode;

  if (_configMode) {
    // ── Portal captivo ──
    _server.on("/",        [this](){ _handleSetupRoot(); });
    _server.on("/scan",    [this](){ _handleScan(); });
    _server.on("/connect", HTTP_POST, [this](){ _handleConnect(); });
    _server.on("/reset",   [this](){ _handleReset(); });
    _server.onNotFound([this](){ _server.send(200,"text/html",_buildSetupHTML()); });

  } else {
    // ── Dashboard operacional ──
    _server.on("/",              [this](){ _handleDashboard(); });
    _server.on("/api/status",    [this](){ _handleApiStatus(); });
    _server.on("/api/sensors",   [this](){ _handleApiSensors(); });
    _server.on("/api/actuators", [this](){ _handleApiActuators(); });
    _server.on("/api/control",   HTTP_POST, [this](){ _handleApiControl(); });
    _server.on("/api/umbrales",  HTTP_POST, [this](){ _handleApiUmbrales(); });
    _server.onNotFound([this](){ _server.send(200,"text/html",_buildDashboardHTML()); });
  }

  _server.begin();
  Serial.println("[WebServer] ✅ HTTP servidor iniciado en puerto 80");
}

void WebServerManager::loop() { _server.handleClient(); }

void WebServerManager::_cors() {
  _server.sendHeader("Access-Control-Allow-Origin", "*");
  _server.sendHeader("Access-Control-Allow-Methods", "GET,POST");
  _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// ════════════════════════════════════════════════════════
//  HANDLERS — Portal captivo
// ════════════════════════════════════════════════════════
void WebServerManager::_handleSetupRoot() {
  _server.send(200, "text/html", _buildSetupHTML());
}

void WebServerManager::_handleScan() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + WiFi.RSSI(i) + "}";
  }
  json += "]";
  _cors();
  _server.send(200, "application/json", json);
}

void WebServerManager::_handleConnect() {
  String body = _server.arg("plain");
  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, body)) {
    _server.send(400, "text/plain", "❌ JSON inválido");
    return;
  }
  String ssid = doc["ssid"].as<String>();
  String pass = doc["password"].as<String>();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) { delay(500); tries++; }

  if (WiFi.status() == WL_CONNECTED) {
    WifiManager::saveCredentials(ssid, pass);
    _server.send(200, "text/plain", "✅ Conectado! Reiniciando...");
    delay(1200);
    ESP.restart();
  } else {
    _server.send(200, "text/plain", "❌ No se pudo conectar. Verifica la contraseña.");
  }
}

void WebServerManager::_handleReset() {
  Preferences prefs;
  prefs.begin("wifi", false);
  prefs.clear();
  prefs.end();
  _server.send(200, "text/plain", "Reseteado");
  delay(800);
  ESP.restart();
}

// ════════════════════════════════════════════════════════
//  HANDLERS — Dashboard API
// ════════════════════════════════════════════════════════

// GET /api/status  — healthcheck
void WebServerManager::_handleApiStatus() {
  struct tm ti;
  char ts[25] = "sin-ntp";
  if (getLocalTime(&ti)) strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &ti);

  StaticJsonDocument<256> doc;
  doc["status"]     = "ok";
  doc["uptime_s"]   = millis() / 1000;
  doc["timestamp"]  = ts;
  doc["modo"]       = _mqtt->modoManual() ? "manual" : "auto";
  doc["bomba"]      = _actuators->getBomba();
  doc["led"]        = _actuators->getLed();
  doc["ventilador"] = _actuators->getVentilador();
  doc["ip"]         = WiFi.localIP().toString();

  String out;
  serializeJson(doc, out);
  _cors();
  _server.send(200, "application/json", out);
}

// GET /api/sensors
void WebServerManager::_handleApiSensors() {
  SensorData d = _sensors->read();

  StaticJsonDocument<128> doc;
  doc["temperatura"] = d.dhtOk ? d.temperatura : (float)-999;
  doc["humedad"]     = d.dhtOk ? d.humedad     : (float)-999;
  doc["luzRaw"]      = d.luz;
  doc["luzPct"]      = (int)((d.luz / 4095.0f) * 100);
  doc["dhtOk"]       = d.dhtOk;

  String out;
  serializeJson(doc, out);
  _cors();
  _server.send(200, "application/json", out);
}

// GET /api/actuators
void WebServerManager::_handleApiActuators() {
  StaticJsonDocument<64> doc;
  doc["bomba"]      = _actuators->getBomba();
  doc["led"]        = _actuators->getLed();
  doc["ventilador"] = _actuators->getVentilador();

  String out;
  serializeJson(doc, out);
  _cors();
  _server.send(200, "application/json", out);
}

// POST /api/control  body: {"accion":"modo"|"bomba"|"led"|"ventilador", "valor":"auto"|"manual"|"1"|"0"}
void WebServerManager::_handleApiControl() {
  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, _server.arg("plain"))) {
    _server.send(400, "text/plain", "JSON inválido");
    return;
  }
  String accion = doc["accion"].as<String>();
  String valor  = doc["valor"].as<String>();

  if (accion == "modo") {
    _mqtt->setModo(valor);
    _cors();
    _server.send(200, "application/json", "{\"ok\":true}");
    return;
  }

  // Para control local inmediato publicamos directamente vía MQTT
  // El mensaje vuelve al ESP32 por la suscripción y actúa
  // Aquí hacemos el cambio directo también para respuesta inmediata:
  bool estado = (valor == "1" || valor == "true" || valor == "ON");

  if (accion == "bomba")      _actuators->setBomba(estado);
  if (accion == "led")        _actuators->setLed(estado);
  if (accion == "ventilador") _actuators->setVentilador(estado);

  _cors();
  _server.send(200, "application/json", "{\"ok\":true}");
}

// POST /api/umbrales  body: {"tempMax":30, "humedadMin":40, "luzMin":300}
void WebServerManager::_handleApiUmbrales() {
  StaticJsonDocument<128> doc;
  if (deserializeJson(doc, _server.arg("plain"))) {
    _server.send(400, "text/plain", "JSON inválido");
    return;
  }
  auto& u = _mqtt->umbrales();
  if (doc.containsKey("tempMax"))    u.tempMax    = doc["tempMax"].as<float>();
  if (doc.containsKey("humedadMin")) u.humedadMin = doc["humedadMin"].as<float>();
  if (doc.containsKey("luzMin"))     u.luzMin     = doc["luzMin"].as<int>();

  Serial.printf("[WebServer] Umbrales → T>%.1f | H<%.1f | L<%d\n",
    u.tempMax, u.humedadMin, u.luzMin);

  _cors();
  _server.send(200, "application/json", "{\"ok\":true}");
}

void WebServerManager::_handleDashboard() {
  _server.send(200, "text/html", _buildDashboardHTML());
}
