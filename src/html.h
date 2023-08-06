#pragma once

#include <Arduino.h>  // PROGMEM

const char HTML_CONFIRM_RESET[] PROGMEM = R"rawliteral(%HEAD_TEMPLATE% <figure class=text-center><h1>Erease all Data?</h1></figure><div class="d-grid gap-2"><a class="btn btn-danger"href=/reset role=button>Yes</a><a class="btn btn-primary"href=/settings role=button>No</a></div> %pre_foot_template% <p hidden>Hidden Helper)rawliteral";
const char HTML_FOOT[] PROGMEM = R"rawliteral(<figure class=text-center>Victron2MQTT<a id=software_version>%pre_software_version%</a> By <a href=https://github.com/softwarecrash/EPEver2MQTT/ target=_blank>Softwarecrash</a><a href=http://creativecommons.org/licenses/by-nc-sa/4.0/ rel=license target=_blank><img alt="Creative Commons License"src=https://licensebuttons.net/l/by-nc-sa/4.0/80x15.png style=border-width:0></a></figure><div style="display: none;"id=update_alert><figure class=text-center><a id=fwdownload target=_blank>Download the latest version <b id=gitversion></b></a></figure></div><script>$(document).ready(function(){$.getJSON("https://api.github.com/repos/softwarecrash/Victron2MQTT/releases/latest",function(){}).done(function(a){console.log("get data from github done success");$('#fwdownload').attr('href',a.html_url);$('#gitversion').text(a.tag_name.substring(1));let b=a.tag_name.substring(1).split('.').map(e=>parseInt(e));let c="%pre_swversion%".split('.').map(e=>parseInt(e));let d="";for(i=0;i<b.length;i++){if(b[i]===c[i]){d+="e"}else if(b[i]>c[i]){d+="m"}else{d+="l"}};if(!d.match(/[l|m]/g)){console.log("Git-Version equal, nothing to do.")}else if(d.split('e').join('')[0]=="m"){console.log("Git-Version higher, activate notification.");document.getElementById("update_alert").style.display=''}else{console.log("Git-Version lower, nothing to do.")}}).fail(function(){console.log("error can not get version")})})</script>)rawliteral";
const char HTML_HEAD[] PROGMEM = R"rawliteral(<!doctypehtml><meta content=text/html;charset=UTF-8 http-equiv=content-type><meta content=width=device-width,initial-scale=1 name=viewport><link rel="shortcut icon"href=data:image/x-icon;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAHsSURBVDhPrZLLSxtRFMZHY9A2Wou0FB+oiZUWTaPEVo15qInBjamSqLFVk0rV0ke0saAmgg90o5v42NQHCmYkXehC/wbFuK//gG6yyiIki0Dg69zDEEgp46L+4HKHc8/3zTnnXg7/yf0b7O7tIZVK0XcikaBdCjKIx+MI8jzGxj+hWadHu9mC94NDlHAXZBAIrENTVw+O49LrlaYOfv8cJUnBnV9cYNjlJlGOPBfZMnnaZGrqBw6DQTH131AFLtcHEjDxQ0UBunvsMBhMWFhcoiQpyODj6Fj6rz6fH03NOmpL2/BGmEkLnZdXVGJ+YRFPnj5DKPSLxAzu5uYWkUgEXba3KHxchP39AyhVVVheXoFarYHZ0oHPX76iplYNj2cCptY29DsHRLlYQTKZxOnpGaLRKFRV1dAbjLAJhsUlZdBqX8PpfIeS0jKhCiW+e73gsmQkZpABIxaL4ff1NbXxzeNBVnYOFPmPUP3iJazWTuQ9UNCQJyYnKef4+IR0aQMGzx/Bbu/Fz+0dDA4Nw+0ewfTMLFZX1+imWIw9NEdvPzY2t0iTYeAT7r1Fb4TBaBIeUwfNRal6TiU7HH0UY2dt7WY0NulIk2HwN2y4FosV8tw8XIbDYjQTSYPw1RUC6xvU0mGQF6OZSBrcDfAHIwsaPAvZdQgAAAAASUVORK5CYII=><link crossorigin href=https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css integrity=sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3 rel=stylesheet><script src=https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js></script><script crossorigin integrity=sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p src=https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js></script><title>Victron 2 MQTT</title><body><noscript><strong>We're sorry but it doesn't work properly without JavaScript enabled. Please enable it to continue.</strong></noscript><div class="container-md col-md-4">)rawliteral";
const char HTML_MAIN[] PROGMEM = R"rawliteral(%pre_head_template% <div class="row gx-0 mb-2"style="display: none;"id=vcc_alert><div class="alert alert-danger"style="text-align: center;"role=alert><span><b>WARNING ESP VOLTAGE TO LOW</b></span></div></div><div class="row gx-0 mb-2"><div class=col><button class="btn btn-primary"id=prevInv type=button>⊲</button></div><div class=col-9><figure class=text-center><h3 id=devicename></h3></figure></div><div class=col><button class="btn btn-primary"style="float: right;"id=nextInv type=button>⊳</button></div></div><div class="row gx-0 mb-2"><div class=col><div class=progress style=height:1.8rem;><div aria-valuemax=100 aria-valuemin=0 aria-valuenow=0 class=progress-bar id=SOCbar role=progressbar style=width:0%;height:1.8rem;></div></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Device Time:</div></div><div class=col><div class=bg-light><span id=devtime></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Solar:</div></div><div class=col><div class=bg-light><span id=solarV></span><span id=solarA></span><span id=solarW></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Battery:</div></div><div class=col><div class=bg-light><span id=battV></span><span id=battA></span><span id=battW></span><span id=battSOC></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Load:</div></div><div class=col><div class=bg-light><span id=loadV></span><span id=loadA></span><span id=loadW></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>temperature:</div></div><div class=col><div class=bg-light><span id=deviceTemp></span><span id=battTemp></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Consumed Kwh:</div></div><div class=col><div class=bg-light><span id=consD></span><span id=consM></span><span id=consY></span><span id=consT></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>generated Kwh:</div></div><div class=col><div class=bg-light><span id=genD></span><span id=genM></span><span id=genY></span><span id=genT></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>CO2 Reduction:</div></div><div class=col><div class=bg-light><span id=cored></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Input State:</div></div><div class=col><div class=bg-light><span id=inputstate></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Charge Mode:</div></div><div class=col><div class=bg-light><span id=chrgmode></span></div></div></div><div class="row gx-0 mb-2"><div class=col><div class=bg-light>Load State:</div></div><div class=col><div class="bg-light form-check form-switch"><input class=form-check-input id=loadState role=switch type=checkbox></div></div></div><div class="d-grid gap-2"><a class="btn btn-primary btn-block"href=/settings role=button>Settings</a></div><script>var gateway=`ws://${window.location.hostname}/ws`;var websocket;var selInverter=1;var invQuantity=1;window.addEventListener('load',onLoad);function initWebSocket(){console.log('Trying to open a WebSocket connection...');websocket=new WebSocket(gateway);websocket.onopen=onOpen;websocket.onclose=onClose;websocket.onerror=onError;websocket.onmessage=onMessage}function onOpen(a){console.log('Connection opened');setInterval(checkWS,5000)}function onClose(a){console.log('Connection closed');setTimeout(initWebSocket,3500)}function onError(a){console.log('Connection lost')}function onMessage(a){var b=JSON.parse(a.data);document.getElementById("devicename").innerHTML=b.DEVICE_NAME==null?'No Connection':b.DEVICE_NAME;document.getElementById("devtime").innerHTML=unixTimetoDateTime(b.DEVICE_TIME);document.getElementById("solarV").innerHTML=b.LiveData.SOLAR_VOLTS+ 'V ';document.getElementById("solarA").innerHTML=b.LiveData.SOLAR_AMPS+ 'A  ';document.getElementById("solarW").innerHTML=b.LiveData.SOLAR_WATTS+ 'W  ';document.getElementById("SOCbar").innerHTML=b.LiveData.BATTERY_SOC+ '%%';$('#SOCbar').width(b.LiveData.BATTERY_SOC+ "%").attr('aria-valuenow',b.LiveData.BATTERY_SOC);document.getElementById("battV").innerHTML=b.LiveData.BATT_VOLTS+ 'V ';document.getElementById("battA").innerHTML=b.LiveData.BATT_AMPS+ 'A  ';document.getElementById("battW").innerHTML=b.LiveData.BATT_WATTS+ 'W  ';document.getElementById("loadV").innerHTML=b.LiveData.LOAD_VOLTS+ 'V ';document.getElementById("loadA").innerHTML=b.LiveData.LOAD_AMPS+ 'A  ';document.getElementById("loadW").innerHTML=b.LiveData.LOAD_WATTS+ 'W  ';document.getElementById("deviceTemp").innerHTML=b.DEVICE_TEMPERATURE+ '°C  ';if(b.BATTERY_TEMPERATURE!=25){document.getElementById("battTemp").innerHTML=' | '+ b.BATTERY_TEMPERATURE+ '°C  '};document.getElementById("consD").innerHTML='D:'+ b.StatsData.CONS_ENERGY_DAY+ '  ';document.getElementById("consM").innerHTML='M:'+ b.StatsData.CONS_ENGERY_MON+ '  ';document.getElementById("consY").innerHTML='Y:'+ b.StatsData.CONS_ENGERY_YEAR+ '  ';document.getElementById("consT").innerHTML='T:'+ b.StatsData.CONS_ENGERY_TOT;document.getElementById("genD").innerHTML='D:'+ b.StatsData.GEN_ENERGY_DAY+ '  ';document.getElementById("genM").innerHTML='M:'+ b.StatsData.GEN_ENERGY_MON+ '  ';document.getElementById("genY").innerHTML='Y:'+ b.StatsData.GEN_ENERGY_YEAR+ '  ';document.getElementById("genT").innerHTML='T:'+ b.StatsData.GEN_ENERGY_TOT;document.getElementById("cored").innerHTML=b.StatsData.CO2_REDUCTION+ 't ';document.getElementById("inputstate").innerHTML=b.CHARGER_INPUT_STATUS;document.getElementById("chrgmode").innerHTML=b.CHARGER_MODE;document.getElementById("loadState").checked=b.LOAD_STATE;invQuantity=b.DEVICE_QUANTITY;if(invQuantity<=1){document.getElementById('prevInv').style.visibility='hidden';document.getElementById('nextInv').style.visibility='hidden'};if(b.ESP_VCC<2.6){document.getElementById("vcc_alert").style.display=''}else{document.getElementById("vcc_alert").style.display='none'}}function onLoad(a){initWebSocket();initButton()}function initButton(){document.getElementById('loadState').addEventListener('click',LoadSwitch);document.getElementById('prevInv').addEventListener('click',SelInvp);document.getElementById('nextInv').addEventListener('click',SelInvn)}function LoadSwitch(){let a;if(document.getElementById('loadState').checked){a=1}else{a=0};websocket.send('loadSwitch_'+ selInverter+ '_'+ a)}function SelInvp(){if(selInverter>1){selInverter--;websocket.send('wsSelInv_'+ selInverter)}}function SelInvn(){if(selInverter<invQuantity){selInverter++;websocket.send('wsSelInv_'+ selInverter)}}function unixTimetoDateTime(a){var b=new Date(a*1000);var c=b.getFullYear()+ "-"+ (b.getMonth()+ 1<10?'0':'')+ (b.getMonth()+ 1)+ "-"+ (b.getDate()<10?'0':'')+ b.getDate()+ " "+ (b.getHours()<10?'0':'')+ b.getHours()+ ":"+ (b.getMinutes()<10?'0':'')+ b.getMinutes()+ ":"+ (b.getSeconds()<10?'0':'')+ b.getSeconds();return c}function checkWS(){websocket.send("A9")}</script> %pre_foot_template% <p hidden>Hidden Helper)rawliteral";
const char HTML_REBOOT[] PROGMEM = R"rawliteral(%pre_head_template% <figure class=text-center><h1>Rebooting</h1><h2 id=wait>.</h2></figure><div class="d-grid gap-2"><a class="btn btn-primary"href=/ role=button>Main</a></div><script>$(document).ready(function(){window.dotsGoingUp=true;var a=window.setInterval(function(){var d=document.getElementById("wait");if(window.dotsGoingUp)d.innerHTML+=".";else{d.innerHTML=d.innerHTML.substring(1,d.innerHTML.length);if(d.innerHTML===".")window.dotsGoingUp=true};if(d.innerHTML.length>9)window.dotsGoingUp=false},100);setTimeout(b,2000);function b(){setInterval(c,1000)}function c(){$.get("/").done(function(){window.location="/";console.log("success")}).fail(function(){console.log("failed.")})}})</script> %pre_foot_template% <p hidden>Hidden Helper)rawliteral";
const char HTML_SETTINGS[] PROGMEM = R"rawliteral(%pre_head_template% <figure class=text-center><h1>Settings</h1></figure><div class="d-grid gap-2"><form action=/update enctype=multipart/form-data method=POST><div class=input-group><input aria-describedby=inputGroupFileAddon04 aria-label=Upload class=form-control id=inputGroupFile04 name=update type=file><input class="btn btn-outline-secondary"id=inputGroupFileAddon04 type=submit value=Update></div></form><a class="btn btn-primary"href=/settingsedit role=button>Configure</a><a class="btn btn-primary"onclick=SendDateString(); role=button>Set device time from computer</a><a class="btn btn-warning"href=/reboot role=button>Reboot</a><a class="btn btn-primary"href=/confirmreset role=button>Reset ESP</a><a class="btn btn-primary"href=/webserial role=button target=_blank>WebSerial</a><a class="btn btn-primary"href=/ role=button>Back</a></div><script>function SendDateString(){var a=new Date();var b;b=a.getFullYear().toString().slice(2,4)+ (a.getMonth()+ 1<10?'0':'')+ (a.getMonth()+ 1)+ (a.getDate()<10?'0':'')+ a.getDate()+ (a.getHours()<10?'0':'')+ a.getHours()+ (a.getMinutes()<10?'0':'')+ a.getMinutes()+ (a.getSeconds()<10?'0':'')+ a.getSeconds();var c=new XMLHttpRequest();c.onreadystatechange=()=>{if(c.readyState===4){window.location.href='/'}};c.open("GET","/set?datetime="+ b,true);c.send()}</script> %pre_foot_template% <p hidden>Hidden Helper)rawliteral";
const char HTML_SETTINGS_EDIT[] PROGMEM = R"rawliteral(%pre_head_template% <figure class=text-center><h1>Edit Configuration</h1></figure><form action=/settingssave enctype=multipart/form-data method=POST><div class="input-group mb-3"><span class="input-group-text w-50"id=devicenamedesc>Device Name</span><input aria-describedby=devicenamedesc class=form-control id=devicename maxlength=35 name=post_deviceName value=%pre_device_name%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=devicequantitydesc>Device Quantity</span><input aria-describedby=devicequantitydesc class=form-control id=devicequantity maxlength=2 name=post_deviceQuanttity value=%pre_device_quantity%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqttserverdesc>MQTT Server</span><input aria-describedby=mqttserverdesc class=form-control id=mqttserver maxlength=35 name=post_mqttServer value=%pre_mqtt_server%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqttportdesc>MQTT Port</span><input aria-describedby=mqttportdesc class=form-control id=mqttport maxlength=5 name=post_mqttPort value=%pre_mqtt_port%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqttuserdesc>MQTT User</span><input aria-describedby=mqttuserdesc class=form-control id=mqttuser maxlength=35 name=post_mqttUser value=%pre_mqtt_user%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqttpassworddesc>MQTT Password</span><input aria-describedby=mqttpassworddesc class=form-control id=mqttpassword maxlength=35 name=post_mqttPassword type=password value=%pre_mqtt_pass%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqtttopicdesc>MQTT Topic</span><input aria-describedby=mqtttopicdesc class=form-control id=mqtttopic maxlength=35 name=post_mqttTopic value=%pre_mqtt_topic%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqttrefreshdesc>MQTT Refresh (sec)</span><input aria-describedby=mqttrefreshdesc class=form-control id=mqttrefresh maxlength=5 name=post_mqttRefresh value=%pre_mqtt_refresh%></div><div class="input-group mb-2"><span class="input-group-text w-50"id=mqtttriggerdesc>MQTT Data Trigger Path</span><input aria-describedby=mqtttrigerdesc class=form-control id=mqtttrigger maxlength=80 name=post_mqtttrigger value=%pre_mqtt_mqtttrigger%></div><div class="input-group mb-3"><span class="input-group-text w-50"id=mqttjsondesc>MQTT Json Style</span><div class="form-switch form-control mqtt-settings-switch"style="width:50%%; text-align: center;"><input class="form-check-input form control"%pre_mqtt_json% aria-describedby=mqttjsondesc id=mqttjson name=post_mqttjson role=switch type=checkbox value=true></div></div><div class="d-grid gap-2"><input class="btn btn-primary"value="Save settings"type=submit><a class="btn btn-primary"href=/settings role=button>Back</a></div> %pre_foot_template% <p hidden>Hidden Helper)rawliteral";