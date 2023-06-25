const char HTML_MAIN[] PROGMEM = R"rawliteral(
    %HEAD_TEMPLATE%
<div class="row gx-0 mb-2" id="vcc_alert" style="display: none;">
    <div class="alert alert-danger" role="alert" style="text-align: center;">
    <span><b>WARNING ESP VOLTAGE TO LOW</b></span>
    </div>
</div>

<div class="row gx-0 mb-2">
    <figure class="text-center">
        <h3 id="devicename"></h3>
    </figure>
</div>


<div class="row gx-0 mb-2" id="devtime">
    <div class="col">
        <div class="bg-light">Device Time: </div>
    </div>
    <div class="col">
        <div class="bg-light"><span id="devtime"></span></br></div>
    </div>
</div>

<div class="row gx-0 mb-2" id="solar">
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
                id="loadState" disabled></div>
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
        websocket.onmessage = onMessage;
    }
    function onOpen(event) {
        console.log('Connection opened');
    }
    function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
    }
    function onMessage(event) {
        var data = JSON.parse(event.data);


//https://stackoverflow.com/questions/20804163/check-if-a-key-exists-inside-a-json-object

        document.getElementById("devicename").innerHTML = data.DEVICE_NAME == null ? 'No Connection' : data.DEVICE_NAME;
    
if("SOLAR_VOLTS" in data.LiveData && "SOLAR_WATTS" in data.LiveData){ 
        document.getElementById("solarV").innerHTML = data.LiveData.SOLAR_VOLTS + 'V ';
        document.getElementById("solarW").innerHTML = data.LiveData.SOLAR_WATTS + 'W  ';
} else {
    document.getElementById("solar").style.display = 'none';
}

//working
document.getElementById("devtime").style.display = 'none';

        document.getElementById("battV").innerHTML = data.LiveData.BATT_VOLTS + 'V ';
        document.getElementById("battA").innerHTML = data.LiveData.BATT_AMPS + 'A  ';

        document.getElementById("genD").innerHTML = 'D:' + data.StatsData.GEN_ENERGY_DAY + '  ';
        document.getElementById("genT").innerHTML = 'T:' + data.StatsData.GEN_ENERGY_TOT;

        document.getElementById("loadState").checked = data.LOAD_STATE;

        if (data.ESP_VCC < 2.6) {
            document.getElementById("vcc_alert").style.display = '';
        }else{
            document.getElementById("vcc_alert").style.display = 'none';
        }

    }

    function onLoad(event) {
        initWebSocket();
        initButton();
    }

    function initButton() {
        document.getElementById('loadState').addEventListener('click', LoadSwitch);
    }


    function LoadSwitch() {
        let loadSwitch;
        if (document.getElementById('loadState').checked) { loadSwitch = 1 }
        else { loadSwitch = 0 }

        websocket.send('loadSwitch_' + selInverter + '_' + loadSwitch);
    }

    function SelInvp() {
        if (selInverter > 1) {
            selInverter--;
            websocket.send('wsSelInv_' + selInverter);
        }
    }
    function SelInvn() {
        if (selInverter < invQuantity) {
            selInverter++;
            websocket.send('wsSelInv_' + selInverter);
        }
    }

    function unixTimetoDateTime(unixTime) {
        var deviceDate = new Date(unixTime * 1000);
        var formatedTime = (deviceDate.getFullYear()) + "-" +
            ((deviceDate.getMonth() + 1) < 10 ? '0' : '') + (deviceDate.getMonth() + 1) + "-" +
            (deviceDate.getDate() < 10 ? '0' : '') + deviceDate.getDate() + " " +
            (deviceDate.getHours() < 10 ? '0' : '') + deviceDate.getHours() + ":" +
            (deviceDate.getMinutes() < 10 ? '0' : '') + deviceDate.getMinutes() + ":" +
            (deviceDate.getSeconds() < 10 ? '0' : '') + deviceDate.getSeconds();
        return formatedTime;
    }
</script>
%FOOT_TEMPLATE%
)rawliteral";
