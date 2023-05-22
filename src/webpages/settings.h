const char HTML_CONFIRM_RESET[] PROGMEM = R"rawliteral(
    %HEAD_TEMPLATE%
<figure class="text-center"><h1>Erease all Data?</h1></figure>
<div class="d-grid gap-2">
<a class="btn btn-danger" href="/reset" role="button">Yes</a>
<a class="btn btn-primary" href="/settings" role="button">No</a>
</div>
%FOOT_TEMPLATE%
)rawliteral";


const char HTML_SETTINGS[] PROGMEM = R"rawliteral(
    %HEAD_TEMPLATE%
<figure class="text-center"><h1>Settings</h1></figure>
<div class="d-grid gap-2">
<form method="POST" action="/update" enctype="multipart/form-data">
<div class="input-group">
<input class="form-control" id="inputGroupFile04" aria-describedby="inputGroupFileAddon04" aria-label="Upload" type="file" name="update"><input class="btn btn-outline-secondary" id="inputGroupFileAddon04" type="submit" value="Update">
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
dateString =  (today.getFullYear().toString().slice(2,4)) +
    ((today.getMonth()+1)< 10 ? '0' : '') + (today.getMonth()+1) +
    (today.getDate()< 10 ? '0' : '') + today.getDate() +
    (today.getHours()< 10 ? '0' : '') + today.getHours() +
    (today.getMinutes()< 10 ? '0' : '') + today.getMinutes() +
    (today.getSeconds()< 10 ? '0' : '') + today.getSeconds();
var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = () => {
    if (xhr.readyState === 4) {
      window.location.href = '/';
    }
  }
xhr.open("GET", "/set?datetime="+dateString, true); 
xhr.send();
}
</script>
%FOOT_TEMPLATE%
)rawliteral";