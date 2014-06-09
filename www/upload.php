<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try {
  session_start();

  unset($_SESSION['client_filename']);
  unset($_SESSION['internal_filename']);

  $error = '';
  $temp_dir = realpath(swt\Functions::TEMP_DIR).DIRECTORY_SEPARATOR;
  $upload_dir = realpath(swt\Functions::UPLOAD_DIR).DIRECTORY_SEPARATOR;
  $edit_dir = realpath(swt\Functions::EDIT_DIR).DIRECTORY_SEPARATOR;

  if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    if ($_FILES['fitFile']['size'] == 0) {
      $error = 'Select a file to upload';
    } else {

      $client_filename = $_FILES['fitFile']['name'];

      // IE sometimes send complete path to filename on user machine, if
      // this is the case, we just want the filename, this pattern match
      // a windows path  
      $pattern =  '/^(?:[a-zA-Z]\:(\\\\|\/)|file\:\/\/|\\\\\\\\|\.(\/|\\\\))([^\\\\\\/\:\*\?\<\>\"\|]+(\\\\|\/){0,1})+$/';

      if (preg_match($pattern, $client_filename) == 1) {
        if (preg_match('/[^\\\\\/]*$/', $client_filename, $matches) == 1) 
          $client_filename = $matches[0];
      } 

      // Remove illegal character when saving file to windows
      $temp_filename = preg_replace('/[\\\\\/\:\*\?\<\>\"\|]/', '', $client_filename);
      $temp_filename .= uniqid();

      if (!move_uploaded_file($_FILES['fitFile']['tmp_name'], $temp_dir.$temp_filename)) {
        throw new Exception('Cannot move uploaded file to temp directory');
      }
      $swim_file = new swt\SwimFile($temp_dir.$temp_filename);
      $internal_filename = $swim_file->getSerialNumber().'_'.$temp_filename;

      if (!(copy($temp_dir.$temp_filename, $upload_dir.$internal_filename) && 
        rename($temp_dir.$temp_filename, $edit_dir.$internal_filename))) {
          throw new Exception('Cannot copy file to editing directory');
      }

      $_SESSION['internal_filename'] = $internal_filename;
      $_SESSION['client_filename'] = $client_filename;

      $serial_number = $swim_file->getSerialNumber();
      setcookie('SN', $serial_number, time() + 60*60*24*7);
      header('Location: editor');
      exit;
    }
  }
} catch (swt\FileNotValidException $ex) {
  if (isset($_FILES['fitFile']['name']))
    $error = $_FILES['fitFile']['name'];

  swt\Functions::errorLog($ex, $error);
  $error .= ': '.$ex->getMessage();
} catch (Exception $ex) {
  if (isset($_FILES['fitFile']['name']))
    $error = $_FILES['fitFile']['name'];

  swt\Functions::errorLog($ex, $error);
  $error = 'Unexpected error has been logged, try again or '
    .'<a href="contact">Contact web site owner</a>';
}

\swt\Layout::header('Swimming Watch Data Editor - Upload', \swt\Layout::TAB_EDITOR);
?>
<div class="section">
  <h2>
    Upload Activity file from Device 
  </h2>
  <div class="warning" style="width:756px; margin: 10px auto">
    If you have installed <b>Garmin Express</b> software, you will have to 
    manually upload your activity files. Check the
    <a href="help" target="_blank">User's Guide</a>
    for how to proceed
  </div>
  <div id="garminDisplayContainer">
    <div id="garminDisplay">
    </div>
  </div>
</div>
<div class="section">
  <h2>
    Upload Activity file from Computer (Manual Upload)
  </h2>
  <ul>
    <li>Upload from Computer if you are using Garmin Express or 
    you don't want to install the Communicator Plug-in</li>
    <li>Only activity files (FIT format) from Garmin Swim/Forerunner
    910/Fénix 2 devices are allowed, </li>
    <li>TCX file exported from Garmin Connect will NOT work.</li>
    <li>If you don't know where to find activity files on your computer,
    check those FAQs from garmin:<br>

    <p><strong>Garmin Express users: </strong> 
    <a href="http://support.garmin.com/support/searchSupport/case.faces?caseId={afea30d0-9b70-11e3-d5f4-000000000000}" 
      target="_blank">Where does Garmin Express store my activity files</a></p>
    <p><strong>Garmin ANT Agent users: </strong> 
    <a href="https://support.garmin.com/support/searchSupport/case.faces?caseId=%7B6e1aa610-6d25-11dc-6782-000000000000%7D" 
      target="_blank">Where does ANT agent store my history</a></p>
    </li>
  </ul>
<?php
if (!empty($error)) {
  echo '<p class="warning">'.$error.'</p>';
}
?>
  <div class="centered">
    <form name="manualUploadForm" action="" enctype="multipart/form-data" method="post">
      <br>
      <input type="file" name="fitFile"><br>
      <br>
      <input class="button" type="submit" name="manualUpload" value="Upload">
    </form>
  </div>
</div>
<script type="text/javascript" src="scripts/prototype/prototype.js"></script>
<script type="text/javascript" src="scripts/garmin/device/GarminDeviceDisplay.js"></script>
<script type="text/javascript">
function onGetSendOptions(options, deviceXml, data) {

  var filename = data.substr(17, data.indexOf('\n', 17) - 17);
  var SN = deviceXml.substr(deviceXml.indexOf('<Id>') + 4, 
    deviceXml.indexOf('</Id>') - (deviceXml.indexOf('<Id>') + 4));

  var boundary = 'SwimmingWatchDataEditorYKCl7kEu';

  options.contentType = 'multipart/form-data; boundary=' + boundary;
  options.encoding = "";
  options.postBody = '--' + boundary + '\r\n';
  options.postBody += 'Content-Disposition: form-data; name="SN";' + '\r\n\r\n';
  options.postBody += SN + '\r\n';
  options.postBody += '--' + boundary + '\r\n';
  options.postBody += 'Content-Disposition: form-data; name="fitFile"; filename="' + filename + '"' + '\r\n';
  options.postBody += 'Content-Type: application/octet-stream' + '\r\n\r\n';
  options.postBody += data.substr(data.indexOf('\n') + 1, data.lastIndexOf('\n====') - (data.indexOf('\n') + 1)) + '\r\n';
  options.postBody += ('--' + boundary + '--' + '\r\n');

  return options;
}

function onAfterFinishSendData(response, activityStatusElement) {
  if (response.status != 200) {
    $('statusText').innerHTML = '<span class="warning">Unexpected Error have'
      + ' been logged, <a href="upload">try again</a> or <a href="contact">Contact</a>'
      + ' developer</span>';
    activityStatusElement.innerHTML = 'Error';
  } else {
    if (response.responseJSON.status == 'ok') {
      $('statusText').innerHTML = 'Upload Completed, Starting Editor...';
      activityStatusElement.innerHTML = 'Upload Completed';
      window.location = 'editor';
    } else if (response.responseJSON.status == 'error') {
      $('statusText').innerHTML = '<span class="warning">' + response.responseJSON.message + '</span>';
      activityStatusElement.innerHTML = 'Error';
    } 
  }
}

function onAfterFinishUploads(display) {

  display.fileTypeRead = Garmin.DeviceControl.FILE_TYPES.fitDir;
  display.readSelectedButton.disabled = false;
  display.readSelectedButton.show();

  for (var boxIndex = 0; boxIndex < display.activityDirectory.size(); boxIndex++) {
    $("activityItemCheckbox" + boxIndex).checked = false;
  }
}

function onLoad() {

  document.manualUploadForm.onsubmit = manualUploadForm_Onsubmit;

  var display = new Garmin.DeviceDisplay("garminDisplay", {
    pathKeyPairsArray: [ 
      "http://swimmingwatchtools.com", "f932686320cb1c84380a727ab138d8e9", 
      "http://www.swimmingwatchtools.com","234847838b750932f86b19fcdd1c8c08",
      "http://dev.swimmingwatchtools.com","a5ec3957473a5d45dec6da8e7553e3d3"],
      showReadDataElement: true,
      showProgressBar: true,
      showFindDevicesElement: true,
      showFindDevicesButton: false,
      showDeviceButtonsOnLoad: false,
      showDeviceButtonsOnFound: false,
      autoFindDevices: true,
      useDeviceBrowser: false,
      showDeviceSelectOnLoad: false,
      autoHideUnusedElements: true,
      readDataTypes: ['FitDirectory'],
      deviceSelectLabel: "Choose Device <br/>",
      readDataButtonText: "List Activities",
      showCancelReadDataButton: false,
      statusCellProcessingImg: '<img src="content/ajax-loader.gif")" width="15" height="15" />&nbsp;Uploading...',
      lookingForDevices: 'Searching for Device <br/><br/> <img src="content/ajax-loader.gif"/>',
      uploadsFinished: "Transfer Completed",
      uploadSelectedActivities: true,
      uploadMaximum: 1,
      dataFound: "#{tracks} activities found on device",
      showReadDataElementOnDeviceFound: true,
      getSendOptions: onGetSendOptions,
      sendDataOptions: new Object(),
      sendDataUrl: 'ajax-communicator.php',
      afterFinishSendData: onAfterFinishSendData,
      afterFinishUploads: onAfterFinishUploads
      //detectNewActivities: true,
      //syncDataUrl: 'AjaxMatch.cshtml',
      //syncDataOptions: {}

  });
}

function manualUploadForm_Onsubmit() {
  if (document.manualUploadForm.fitFile.value == "") {
    alert("Select a file");
    return false;
  }
  else return true;
}

document.body.onload = onLoad;
</script>
<?php 
swt\Layout::analytics();
swt\Layout::footer();
