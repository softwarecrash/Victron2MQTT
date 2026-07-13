#pragma once

#include <Arduino.h>

static const char HTML_WEBSERIAL[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>WebSerial</title>
  <style>
    *,::before,::after{box-sizing:border-box}
    html,body{height:100%}
    body{margin:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Arial,sans-serif;background:#eef1f4;color:#16202a;overscroll-behavior:none}
    button,input,textarea{font:inherit}
    .app{min-height:100dvh;display:flex;flex-direction:column;gap:.65rem;padding:.75rem}
    .topbar{display:flex;align-items:center;gap:.5rem}
    .title{font-size:1rem;font-weight:650;margin:0;line-height:1}
    .status{font-size:.82rem;color:#5f6975;margin-left:auto;white-space:nowrap}
    .panel{position:relative;flex:1;min-height:0;display:flex;flex-direction:column;gap:.5rem;background:#fff;border:1px solid #d9dee5;border-radius:8px;padding:.5rem;box-shadow:0 6px 18px rgba(20,28,36,.08)}
    .toolbar{position:absolute;top:.5rem;right:.5rem;display:flex;gap:.35rem;z-index:2}
    .icon-btn{width:2.25rem;height:2.25rem;display:inline-flex;align-items:center;justify-content:center;border:0;border-radius:6px;background:#596370;color:#fff;cursor:pointer}
    .icon-btn:hover{background:#46515d}
    .icon-btn.active{background:#0067f4}
    .icon-btn svg{width:1.05rem;height:1.05rem;fill:currentColor}
    #record{flex:1;width:100%;min-height:18rem;resize:none;border:1px solid #d7dce3;border-radius:6px;background:#111820;color:#dbe7ef;padding:3rem .65rem .65rem;font-family:Consolas,"SFMono-Regular",Menlo,monospace;font-size:.82rem;line-height:1.35;white-space:pre;overflow:auto}
    .command{display:grid;grid-template-columns:minmax(0,1fr) auto;gap:.5rem}
    #command-text{min-width:0;border:1px solid #cdd3db;border-radius:6px;padding:.55rem .65rem}
    #command-button{border:0;border-radius:6px;background:#0067f4;color:#fff;padding:.55rem .9rem;cursor:pointer;font-weight:650}
    #command-button:disabled,#command-text:disabled{opacity:.55;cursor:not-allowed}
    @media (max-width:560px){.app{padding:.5rem}.status{display:none}.toolbar{position:static;order:-1;justify-content:flex-end}#record{padding:.65rem}.panel{gap:.5rem}}
  </style>
</head>
<body>
  <main class="app">
    <div class="topbar">
      <h1 class="title">WebSerial</h1>
      <span class="status" id="status">Disconnected</span>
    </div>
    <section class="panel">
      <div class="toolbar">
        <button class="icon-btn" type="button" title="Clear log" aria-label="Clear log" onclick="terminalClean()">
          <svg viewBox="64 64 896 896" aria-hidden="true"><path d="M864 256H736v-80c0-35.3-28.7-64-64-64H352c-35.3 0-64 28.7-64 64v80H160c-17.7 0-32 14.3-32 32v32c0 4.4 3.6 8 8 8h60.4l24.7 523c1.6 34.1 29.8 61 63.9 61h454c34.2 0 62.3-26.8 63.9-61l24.7-523H888c4.4 0 8-3.6 8-8v-32c0-17.7-14.3-32-32-32zm-200 0H360v-72h304v72z"/></svg>
        </button>
        <button class="icon-btn" type="button" title="Download log" aria-label="Download log" onclick="downloadLog()">
          <svg viewBox="64 64 896 896" aria-hidden="true"><path d="M505.7 661a8 8 0 0012.6 0l112-141.7c4.1-5.2.4-12.9-6.3-12.9h-74.1V168c0-4.4-3.6-8-8-8h-60c-4.4 0-8 3.6-8 8v338.3H400c-6.7 0-10.4 7.7-6.3 12.9l112 141.8z"/><path d="M878 626h-60c-4.4 0-8 3.6-8 8v154H214V634c0-4.4-3.6-8-8-8h-60c-4.4 0-8 3.6-8 8v198c0 17.7 14.3 32 32 32h684c17.7 0 32-14.3 32-32V634c0-4.4-3.6-8-8-8z"/></svg>
        </button>
        <button class="icon-btn toggle" id="flow-lock-button" type="button" title="Toggle auto-scroll" aria-label="Toggle auto-scroll" onclick="toggleFlowLock()">
          <svg viewBox="64 64 896 896" aria-hidden="true"><path d="M832 464h-68V240c0-70.7-57.3-128-128-128H388c-70.7 0-128 57.3-128 128v224h-68c-17.7 0-32 14.3-32 32v384c0 17.7 14.3 32 32 32h640c17.7 0 32-14.3 32-32V496c0-17.7-14.3-32-32-32zM332 240c0-30.9 25.1-56 56-56h248c30.9 0 56 25.1 56 56v224H332V240zm460 600H232V536h560v304z"/></svg>
        </button>
        <button class="icon-btn toggle" id="timestamp-button" type="button" title="Toggle timestamps" aria-label="Toggle timestamps" onclick="toggleTimestamp()">
          <svg viewBox="64 64 896 896" aria-hidden="true"><path d="M512 64C264.6 64 64 264.6 64 512s200.6 448 448 448 448-200.6 448-448S759.4 64 512 64zm0 820c-205.4 0-372-166.6-372-372s166.6-372 372-372 372 166.6 372 372-166.6 372-372 372z"/><path d="M686.7 638.6L544.1 535.5V288c0-4.4-3.6-8-8-8H488c-4.4 0-8 3.6-8 8v275.4c0 2.6 1.2 5 3.3 6.5l165.4 120.6c3.6 2.6 8.6 1.8 11.2-1.7l28.6-39c2.6-3.7 1.8-8.7-1.8-11.2z"/></svg>
        </button>
        <button class="icon-btn toggle active" id="ansi-button" type="button" title="Toggle ANSI cleanup" aria-label="Toggle ANSI cleanup" onclick="toggleAnsiCleanup()">
          <svg viewBox="64 64 896 896" aria-hidden="true"><path d="M899.1 869.6l-53-305.6H864c14.4 0 26-11.6 26-26V346c0-14.4-11.6-26-26-26H618V138c0-14.4-11.6-26-26-26H432c-14.4 0-26 11.6-26 26v182H160c-14.4 0-26 11.6-26 26v192c0 14.4 11.6 26 26 26h17.9l-53 305.6c-0.3 1.5-0.4 3-0.4 4.4 0 14.4 11.6 26 26 26h723c1.5 0 3-0.1 4.4-0.4 14.2-2.4 23.7-15.9 21.2-30zM204 390h272V182h72v208h272v104H204V390zm468 440V674c0-4.4-3.6-8-8-8h-48c-4.4 0-8 3.6-8 8v156H416V674c0-4.4-3.6-8-8-8h-48c-4.4 0-8 3.6-8 8v156H202.8l45.1-260H776l45.1 260H672z"/></svg>
        </button>
      </div>
      <textarea id="record" readonly spellcheck="false" title="record"></textarea>
      <div class="command">
        <input id="command-text" name="cmd" autocomplete="off" disabled>
        <button id="command-button" type="button" disabled>SEND</button>
      </div>
    </section>
  </main>
  <script>
    let websocket,pingTimeout,connectTimeout;
    const gateway=(window.location.protocol==="https:"?"wss://":"ws://")+window.location.host+(window.location.pathname.endsWith("/")?window.location.pathname.slice(0,-1):window.location.pathname)+"ws";
    const textArea=document.getElementById("record");
    const statusEl=document.getElementById("status");
    let enableFlowLock=false;
    let enableTimestamp=false;
    let enableStripAnsiEscapeCodes=true;
    let commandHistory=[];
    let commandHistoryIdx=0;

    function ansiRegex({onlyFirst=false}={}){return new RegExp("(?:\\u001B\\][\\s\\S]*?(?:\\u0007|\\u001B\\u005C|\\u009C))|[\\u001B\\u009B][[\\]()#;?]*(?:\\d{1,4}(?:[;:]\\d{0,4})*)?[\\dA-PR-TZcf-nq-uy=><~]",onlyFirst?void 0:"g")}
    function setStatus(text){statusEl.textContent=text}
    function syncButtonStates(){
      document.getElementById("flow-lock-button").classList.toggle("active",enableFlowLock);
      document.getElementById("timestamp-button").classList.toggle("active",enableTimestamp);
      document.getElementById("ansi-button").classList.toggle("active",enableStripAnsiEscapeCodes);
    }
    function initWebPage(){
      initCommandHistory();
      initWebSocket();
      document.getElementById("command-button").addEventListener("click",sendCommand);
      trapKeyPress();
      syncButtonStates();
    }
    function initCommandHistory(){
      const stored=localStorage.history;
      commandHistory=stored?JSON.parse(stored):[];
      commandHistoryIdx=commandHistory.length;
    }
    function initWebSocket(){
      clearTimeout(connectTimeout);
      clearTimeout(pingTimeout);
      pingTimeout=false;
      setStatus("Connecting");
      connectTimeout=setTimeout(()=>{terminalWrite("[WebSerial] Connect timeout.");websocket.close();initWebSocket()},3000);
      terminalWrite("[WebSerial] Connecting...");
      websocket=new WebSocket(gateway);
      websocket.onopen=onOpen;
      websocket.onclose=onClose;
      websocket.onmessage=onMessage;
      websocket.onerror=onError;
    }
    function trapKeyPress(){
      document.getElementById("command-text").addEventListener("keypress",event=>{
        if(event.code==="Enter"){event.preventDefault();document.getElementById("command-button").click()}
      });
      document.addEventListener("keydown",event=>{
        if(!document.activeElement||document.activeElement.id!=="command-text")return;
        if(event.code==="ArrowUp"){
          commandHistoryIdx--;
          if(commandHistoryIdx<0)commandHistoryIdx=commandHistory.length>0?commandHistory.length-1:0;
          if(commandHistoryIdx>=0&&commandHistoryIdx<commandHistory.length)document.getElementById("command-text").value=commandHistory[commandHistoryIdx];
        }else if(event.code==="ArrowDown"){
          commandHistoryIdx++;
          if(commandHistoryIdx>=commandHistory.length)commandHistoryIdx=0;
          if(commandHistoryIdx>=0&&commandHistoryIdx<commandHistory.length)document.getElementById("command-text").value=commandHistory[commandHistoryIdx];
        }
      });
    }
    function onOpen(){
      clearTimeout(connectTimeout);
      setStatus("Connected");
      terminalWrite("[WebSerial] Connected...");
      document.getElementById("command-button").disabled=false;
      document.getElementById("command-text").disabled=false;
    }
    function onError(event){console.log("[WebSerial] Error!",event);setStatus("Error");websocket.close()}
    function onClose(event){console.log("[WebSerial] Connection closed.",event);setStatus("Disconnected")}
    function onMessage(event){event.data==="pong"?(clearTimeout(pingTimeout),pingTimeout=false):terminalWrite(event.data)}
    function terminalWrite(text){
      if(enableTimestamp)text="["+(new Date).toLocaleTimeString()+"] "+text;
      if(enableStripAnsiEscapeCodes&&typeof text==="string")text=text.replace(ansiRegex(),"");
      textArea.value+=text+"\n";
      if(!enableFlowLock)textArea.scrollTop=textArea.scrollHeight;
    }
    function terminalClean(){textArea.value="";textArea.scrollTop=textArea.scrollHeight}
    function toggleFlowLock(){enableFlowLock=!enableFlowLock;syncButtonStates()}
    function toggleTimestamp(){enableTimestamp=!enableTimestamp;syncButtonStates()}
    function toggleAnsiCleanup(){enableStripAnsiEscapeCodes=!enableStripAnsiEscapeCodes;syncButtonStates()}
    function downloadLog(){
      const value=textArea.value;
      const now=new Date();
      const stamp=now.getFullYear()+"-"+String(now.getMonth()+1).padStart(2,"0")+"-"+String(now.getDate()).padStart(2,"0")+"_"+String(now.getHours()).padStart(2,"0")+"-"+String(now.getMinutes()).padStart(2,"0")+"-"+String(now.getSeconds()).padStart(2,"0");
      const blob=new Blob([value],{type:"text/plain;charset=utf-8"});
      const url=URL.createObjectURL(blob);
      const link=document.createElement("a");
      link.href=url;
      link.download="Victron2MQTT-webserial-"+stamp+".txt";
      document.body.appendChild(link);
      link.click();
      link.remove();
      setTimeout(()=>URL.revokeObjectURL(url),1000);
    }
    function sendCommand(){
      const input=document.getElementById("command-text");
      const command=input.value;
      websocket.send(command);
      commandHistory.push(command);
      if(commandHistory.length>20)commandHistory.splice(0,commandHistory.length-20);
      commandHistoryIdx=commandHistory.length;
      input.value="";
      localStorage.history=JSON.stringify(commandHistory);
    }
    setInterval(()=>{if(!pingTimeout&&websocket.readyState===WebSocket.OPEN){pingTimeout=setTimeout(()=>{terminalWrite("[WebSerial] Ping timeout.");websocket.close();initWebSocket()},3000);websocket.send("ping")}},2000);
    window.addEventListener("DOMContentLoaded",initWebPage,false);
  </script>
</body>
</html>
)rawliteral";
