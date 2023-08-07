#pragma once

#include <Arduino.h>  // PROGMEM

const char HTML_CONFIRM_RESET[] PROGMEM = R"rawliteral(%HEAD_TEMPLATE%
<figure class="text-center">
    <h1>Erease all Data?</h1>
</figure>
<div class="d-grid gap-2">
    <a class="btn btn-danger" href="/reset" role="button">Yes</a>
    <a class="btn btn-primary" href="/settings" role="button">No</a>
</div>
%pre_foot_template%
<p hidden>Hidden Helper</p>)rawliteral";
const char HTML_FOOT[] PROGMEM = R"rawliteral(<figure class="text-center">
    Victron2MQTT<a id="software_version">%pre_software_version%</a> By <a
        href="https://github.com/softwarecrash/EPEver2MQTT/" target="_blank">Softwarecrash</a>
    <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/" target="_blank"><img
            alt="Creative Commons License" style="border-width:0"
            src="https://licensebuttons.net/l/by-nc-sa/4.0/80x15.png" /></a>
</figure>
</div>
<div id="update_alert" style="display: none;">
    <figure class="text-center"><a id="fwdownload" target="_blank">Download the latest version <b
                id="gitversion"></b></a></figure>
</div>
<script>
    $(document).ready(function () {
        $.getJSON("https://api.github.com/repos/softwarecrash/Victron2MQTT/releases/latest", function () {
        })
            .done(function (data) {
                console.log("get data from github done success");
                $('#fwdownload').attr('href', data.html_url);
                $('#gitversion').text(data.tag_name.substring(1));
                let x = data.tag_name.substring(1).split('.').map(e => parseInt(e));
                let y = "%pre_swversion%".split('.').map(e => parseInt(e));
                let z = "";
                for (i = 0; i < x.length; i++) { if (x[i] === y[i]) { z += "e"; } else if (x[i] > y[i]) { z += "m"; } else { z += "l"; } }
                if (!z.match(/[l|m]/g)) {
                    console.log("Git-Version equal, nothing to do.");
                } else if (z.split('e').join('')[0] == "m") {
                    console.log("Git-Version higher, activate notification.");
                    document.getElementById("update_alert").style.display = '';
                } else { console.log("Git-Version lower, nothing to do."); }
            })
            .fail(function () {
                console.log("error can not get version");
            });
    });
</script>
</body>

</html>)rawliteral";
const char HTML_HEAD[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>

<head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="shortcut icon"
        href="data:image/x-icon;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsQAAA7EAZUrDhsAAAHsSURBVDhPrZLLSxtRFMZHY9A2Wou0FB+oiZUWTaPEVo15qInBjamSqLFVk0rV0ke0saAmgg90o5v42NQHCmYkXehC/wbFuK//gG6yyiIki0Dg69zDEEgp46L+4HKHc8/3zTnnXg7/yf0b7O7tIZVK0XcikaBdCjKIx+MI8jzGxj+hWadHu9mC94NDlHAXZBAIrENTVw+O49LrlaYOfv8cJUnBnV9cYNjlJlGOPBfZMnnaZGrqBw6DQTH131AFLtcHEjDxQ0UBunvsMBhMWFhcoiQpyODj6Fj6rz6fH03NOmpL2/BGmEkLnZdXVGJ+YRFPnj5DKPSLxAzu5uYWkUgEXba3KHxchP39AyhVVVheXoFarYHZ0oHPX76iplYNj2cCptY29DsHRLlYQTKZxOnpGaLRKFRV1dAbjLAJhsUlZdBqX8PpfIeS0jKhCiW+e73gsmQkZpABIxaL4ff1NbXxzeNBVnYOFPmPUP3iJazWTuQ9UNCQJyYnKef4+IR0aQMGzx/Bbu/Fz+0dDA4Nw+0ewfTMLFZX1+imWIw9NEdvPzY2t0iTYeAT7r1Fb4TBaBIeUwfNRal6TiU7HH0UY2dt7WY0NulIk2HwN2y4FosV8tw8XIbDYjQTSYPw1RUC6xvU0mGQF6OZSBrcDfAHIwsaPAvZdQgAAAAASUVORK5CYII=" />
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"
        crossorigin="anonymous"></script>
    <title>Victron 2 MQTT</title>

<body>
    <noscript>
        <strong>We're sorry but it doesn't work properly without JavaScript enabled. Please enable it to
            continue.</strong>
    </noscript>
    <div class="container-md col-md-4">)rawliteral";
const char HTML_MAIN[] PROGMEM = R"rawliteral(%pre_head_template%
<div class="row gx-0 mb-2" id="vcc_alert" style="display: none;">
    <div class="alert alert-danger" role="alert" style="text-align: center;">
        <span><b>WARNING ESP VOLTAGE TO LOW</b></span>
    </div>
</div>

<figure class="text-center">
    <h2 id="devicename"></h2>
</figure>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="progress" style="height:1.8rem;">
            <div id="SOCbar" class="progress-bar" role="progressbar" style="width:0%;height:1.8rem;" aria-valuenow="0"
                aria-valuemin="0" aria-valuemax="100"></div>
        </div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Device Time: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="devtime"></span></br></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Solar: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="solarV"></span><span id="solarA"></span><span id="solarW"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Battery: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="battV"></span><span id="battA"></span><span id="battW"></span><span
                id="battSOC"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Load: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="loadV"></span><span id="loadA"></span><span id="loadW"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">temperature: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="deviceTemp"></span><span id="battTemp"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Consumed Kwh: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="consD"></span><span id="consM"></span><span id="consY"></span><span
                id="consT"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">generated Kwh: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="genD"></span><span id="genM"></span><span id="genY"></span><span
                id="genT"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">CO2 Reduction: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="cored"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Input State: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="inputstate"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Charge Mode: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="chrgmode"></span></div>
    </div>
</div>

<div class="row gx-0 mb-2">
    <div class="col">
        <div class="bg-light">Load State: </div>
    </div>
    <div class="col">
        <div class="bg-light form-check form-switch"><input class="form-check-input" type="checkbox" role="switch"
                id="loadState" /></div>
    </div>
</div>
<div class="d-grid gap-2">
    <a class="btn btn-primary btn-block" href="/settings" role="button">Settings</a>
</div>

<script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var selInverter = 1;
    var invQuantity = 1;
    window.addEventListener('load', onLoad);
    function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen = onOpen;
        websocket.onclose = onClose;
        websocket.onerror = onError;
        websocket.onmessage = onMessage;
    }
    function onOpen(event) {
        console.log('Connection opened');
        setInterval(checkWS, 5000);
    }
    function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 3500);
    }
    function onError(event) {
        console.log('Connection lost');
    }

    function onMessage(event) {
        var data = JSON.parse(event.data);

        document.getElementById("devicename").innerHTML = data.Device_name == null ? 'No Connection' : data.Device_name;
        //document.getElementById("devtime").innerHTML = unixTimetoDateTime(data.DEVICE_TIME);

        //document.getElementById("solarV").innerHTML = data.LiveData.SOLAR_VOLTS + 'V ';
        //document.getElementById("solarA").innerHTML = data.LiveData.SOLAR_AMPS + 'A  ';
        //document.getElementById("solarW").innerHTML = data.LiveData.SOLAR_WATTS + 'W  ';
        //document.getElementById("battSOC").innerHTML = data.LiveData.BATTERY_SOC + '%%';

        document.getElementById("SOCbar").innerHTML = data.SOC + '%%';
        $('#SOCbar').width(Number(data.SOC) + "%").attr('aria-valuenow', Number(data.SOC));

        //document.getElementById("battV").innerHTML = data.LiveData.BATT_VOLTS + 'V ';
        //document.getElementById("battA").innerHTML = data.LiveData.BATT_AMPS + 'A  ';
        //document.getElementById("battW").innerHTML = data.LiveData.BATT_WATTS + 'W  ';

        //document.getElementById("loadV").innerHTML = data.LiveData.LOAD_VOLTS + 'V ';
        //document.getElementById("loadA").innerHTML = data.LiveData.LOAD_AMPS + 'A  ';
        //document.getElementById("loadW").innerHTML = data.LiveData.LOAD_WATTS + 'W  ';


        //document.getElementById("inputstate").innerHTML = data.CHARGER_INPUT_STATUS;
        //document.getElementById("chrgmode").innerHTML = data.CHARGER_MODE;

        if (data.ESP_Data.ESP_VCC < 2.6) {
            document.getElementById("vcc_alert").style.display = '';
        } else {
            document.getElementById("vcc_alert").style.display = 'none';
        }

    }

    function onLoad(event) {
        initWebSocket();
    }

    function checkWS() {
        websocket.send("A9");
    }
</script>
%pre_foot_template%
<p hidden>Hidden Helper</p>)rawliteral";
const char HTML_REBOOT[] PROGMEM = R"rawliteral(%pre_head_template%
<figure class="text-center">
    <h1>Rebooting </h1>
    <h2 id="wait">.</h2>
</figure>
<div class="d-grid gap-2">
    <a class="btn btn-primary" href="/" role="button">Main</a>
</div>

<script>
    $(document).ready(function () {
        window.dotsGoingUp = true;
        var dots = window.setInterval(function () {
            var wait = document.getElementById("wait");
            if (window.dotsGoingUp)
                wait.innerHTML += ".";
            else {
                wait.innerHTML = wait.innerHTML.substring(1, wait.innerHTML.length);
                if (wait.innerHTML === ".")
                    window.dotsGoingUp = true;
            }
            if (wait.innerHTML.length > 9)
                window.dotsGoingUp = false;
        }, 100);

        setTimeout(startCheck, 2000);


        function startCheck() {
            setInterval(serverReachable, 1000);
        }

        function serverReachable() {
            $.get("/").done(function () {
                window.location = "/";
                console.log("success");
            }).fail(function () {
                console.log("failed.");
            });
        }

    });
</script>

%pre_foot_template%
<p hidden>Hidden Helper</p>)rawliteral";
const char HTML_SETTINGS[] PROGMEM = R"rawliteral(%pre_head_template%
<figure class="text-center">
  <h1>Settings</h1>
</figure>
<div class="d-grid gap-2">
  <form method="POST" action="/update" enctype="multipart/form-data">
    <div class="input-group">
      <input class="form-control" id="inputGroupFile04" aria-describedby="inputGroupFileAddon04" aria-label="Upload"
        type="file" name="update"><input class="btn btn-outline-secondary" id="inputGroupFileAddon04" type="submit"
        value="Update">
    </div>
  </form>
  <a class="btn btn-primary" href="/settingsedit" role="button">Configure</a>
  <a class="btn btn-primary" onclick='SendDateString();' role="button">Set device time from computer</a>
  <a class="btn btn-warning" href="/reboot" role="button">Reboot</a>
  <a class="btn btn-primary" href="/confirmreset" role="button">Reset ESP</a>
  <a class="btn btn-primary" href="/webserial" role="button" target="_blank">WebSerial</a>
  <a class="btn btn-primary" href="/" role="button">Back</a>
</div>

<script>
  function SendDateString() {
    var today = new Date();
    var dateString;
    dateString = (today.getFullYear().toString().slice(2, 4)) +
      ((today.getMonth() + 1) < 10 ? '0' : '') + (today.getMonth() + 1) +
      (today.getDate() < 10 ? '0' : '') + today.getDate() +
      (today.getHours() < 10 ? '0' : '') + today.getHours() +
      (today.getMinutes() < 10 ? '0' : '') + today.getMinutes() +
      (today.getSeconds() < 10 ? '0' : '') + today.getSeconds();
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = () => {
      if (xhr.readyState === 4) {
        window.location.href = '/';
      }
    }
    xhr.open("GET", "/set?datetime=" + dateString, true);
    xhr.send();
  }
</script>
%pre_foot_template%
<p hidden>Hidden Helper</p>)rawliteral";
const char HTML_SETTINGS_EDIT[] PROGMEM = R"rawliteral(%pre_head_template%
<figure class="text-center">
    <h1>Edit Configuration</h1>
</figure>
<form method="POST" action="/settingssave" enctype="multipart/form-data">
    <div class="input-group mb-3">
        <span class="input-group-text w-50" id="devicenamedesc">Device Name</span>
        <input type="text" class="form-control" aria-describedby="devicenamedesc" id="devicename" name="post_deviceName"
            maxlength="35" value="%pre_device_name%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqttserverdesc">MQTT Server</span>
        <input type="text" class="form-control" aria-describedby="mqttserverdesc" id="mqttserver" name="post_mqttServer"
            maxlength="35" value="%pre_mqtt_server%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqttportdesc">MQTT Port</span>
        <input type="text" class="form-control" aria-describedby="mqttportdesc" id="mqttport" name="post_mqttPort"
            maxlength="5" value="%pre_mqtt_port%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqttuserdesc">MQTT User</span>
        <input type="text" class="form-control" aria-describedby="mqttuserdesc" id="mqttuser" name="post_mqttUser"
            maxlength="35" value="%pre_mqtt_user%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqttpassworddesc">MQTT Password</span>
        <input type="password" class="form-control" aria-describedby="mqttpassworddesc" id="mqttpassword" maxlength="35"
            name="post_mqttPassword" value="%pre_mqtt_pass%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqtttopicdesc">MQTT Topic</span>
        <input type="text" class="form-control" aria-describedby="mqtttopicdesc" id="mqtttopic" name="post_mqttTopic"
            maxlength="35" value="%pre_mqtt_topic%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqttrefreshdesc">MQTT Refresh (sec)</span>
        <input type="text" class="form-control" aria-describedby="mqttrefreshdesc" id="mqttrefresh" maxlength="5"
            name="post_mqttRefresh" value="%pre_mqtt_refresh%">
    </div>
    <div class="input-group mb-2">
        <span class="input-group-text w-50" id="mqtttriggerdesc">MQTT Data Trigger Path</span>
        <input type="text" class="form-control" aria-describedby="mqtttrigerdesc" id="mqtttrigger" maxlength="80"
            name="post_mqtttrigger" value="%pre_mqtt_mqtttrigger%">
    </div>
    <div class="input-group mb-3">
        <span class="input-group-text w-50" id="mqttjsondesc">MQTT Json Style</span>
        <div class="form-switch form-control mqtt-settings-switch" style="width:50%%; text-align: center;">
            <input type="checkbox" class="form-check-input form control" aria-describedby="mqttjsondesc" role="switch"
                id="mqttjson" name="post_mqttjson" value="true" %pre_mqtt_json%>
        </div>
    </div>


    <div class="d-grid gap-2">
        <input class="btn btn-primary" type="submit" value="Save settings">
</form>
<a class="btn btn-primary" href="/settings" role="button">Back</a>
</div>
%pre_foot_template%
<p hidden>Hidden Helper</p>)rawliteral";
