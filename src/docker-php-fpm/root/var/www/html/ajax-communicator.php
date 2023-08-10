<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try {
  session_start();
  unset($_SESSION['file_id']);
  
  $file_id = NULL;

  header('Content-type: application/json');
  header('Cache-Control: no-cache, no-store, must-revalidate');
  header('Pragma: no-cache');
  header('Expires: 0');
  
  if (!isset($_FILES['fitFile'])) 
    throw new Exception('No file was submitted');

  if (is_uploaded_file($_FILES['fitFile']['tmp_name'])) {

    $file_id = swt\DB::createFileFromUpload('fitFile');
    $path = swt\DB::convertFileIdToPath($file_id);

    $rhandle = fopen($path.'UPLOAD', 'r');
    stream_filter_append($rhandle, 'convert.base64-decode');

    $whandle = fopen($path.'TEMP', 'w');
    stream_copy_to_stream($rhandle, $whandle);
    fclose($rhandle);
    fclose($whandle);

    if (!rename($path.'TEMP', $path.'UPLOAD'))
      throw new Exception('Cannot copy file');

    $activity = new swt\Activity($file_id);
    $activity->saveToDatabase();

    if (!copy($path.'UPLOAD', $path.'EDIT'))
      throw new Exception('Cannot copy file');

    $_SESSION['file_id'] = $file_id;

    $serial_number = $activity->serial_number;
    setcookie('SN', $serial_number, time() + 60*60*24*7);

    echo "{ status: 'ok'}";
  }  
} catch (swt\FileNotValidException $ex) {
  echo "{status: 'error', message: '".$ex->getMessage()."'}";

  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
} catch (Exception $ex) {
  echo "{status:'error',message:'Unexpected error has been logged, try again'}";

  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
}

