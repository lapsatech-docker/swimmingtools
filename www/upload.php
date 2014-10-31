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
  $show_gcp = false;



  if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    if (isset($_COOKIE['show_gcp']))
      setcookie('show_gcp', '0', time() - 3600, '/');
    
    if ($_FILES['fitFile']['size'] == 0) {
      $error = 'Select a file to upload';
    } else if (is_uploaded_file($_FILES['fitFile']['tmp_name'])) {
      // Garmin Connect exports activity file as  zip files containing the
      // original fit file. This code Checks if the uploaded file is a zip
      // file and extract the fit file
      $zip_file = new ZipArchive();
      if ( $zip_file->open($_FILES['fitFile']['tmp_name']) === true) {
        $client_filename = basename($zip_file->getNameIndex(0));

        // Remove illegal character when saving file to windows
        $temp_filename = preg_replace('/[\\\\\/\:\*\?\<\>\"\|]/', '', $client_filename);
        $temp_filename .= uniqid();
        if (!copy ('zip://'.$zip_file->filename.'#'.$zip_file->getNameIndex(0), $temp_dir.$temp_filename)) {
          throw new Exception('Cannot extract fit file from zip');
        }
        $zip_file->close();
      } else {
        // File is a fit file
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
  } else { // HTTP_METHOD = 'GET'
    if (isset($_GET['show_gcp']) || isset($_COOKIE['show_gcp'])) {
      setcookie('show_gcp', '1', time() + 60*60*24*90, '/');
      $show_gcp = true;
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
if ($show_gcp) {
?>
<div class="section">
  <h2>
    Upload Activity file using Garmin Communicator Plug-in
  </h2>
  <div id="garminDisplayContainer">
    <div id="garminDisplay">
    </div>
  </div>
</div>
<?php
} else {
  echo '<div class="warning" style="margin: 20px auto">'
  .'Still using Old Garmin Communicator Plug-in? '
  .'<a href="upload?show_gcp=1">Click here</a>'
  .'</div>';
}
?>
<div class="section">
  <h2>
    Upload Activity file
  </h2>
 <ul>
    <li>Only Lap Swimming activity files from the following devices are allowed: 
    Garmin Swim ,Forerunner 910/920, Fénix 2, </li>
    <li>Files must be FIT files, or zip files exported from Garmin Connect,
    <li>Activity files can be exported form Garmin Connect (Modern version) by going
    to the activity page, clicking on the gear icon to the left, and selecting
    'export original'. Check <a href="help#upload" target="_blank">User's Guide</a>
    for details.</li>
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
<?php
if ($show_gcp) {
?>
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
}
swt\Layout::analytics();
swt\Layout::footer();
