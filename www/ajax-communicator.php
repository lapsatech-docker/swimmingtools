<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try {
  session_start();

  unset($_SESSION['internal_filename']);
  unset($_SESSION['client_filename']);

  $temp_dir = realpath(swt\Functions::TEMP_DIR).DIRECTORY_SEPARATOR;
  $upload_dir = realpath(swt\Functions::UPLOAD_DIR).DIRECTORY_SEPARATOR;
  $download_dir = realpath(swt\Functions::DOWNLOAD_DIR).DIRECTORY_SEPARATOR;
  
  header('Content-type: application/json');
  header('Cache-Control: no-cache, no-store, must-revalidate');
  header('Pragma: no-cache');
  header('Expires: 0');
  
  if (!isset($_FILES['fitFile'])) 
    throw new Exception('No file was submitted');

  if (is_uploaded_file($_FILES['fitFile']['tmp_name'])) {

    $internal_filename = $_POST['SN'].'_'.$_FILES['fitFile']['name'].uniqid();
    $temp_filename = $temp_dir.$internal_filename;
    $client_filename = $_FILES['fitFile']['name'];
    
    $rhandle = fopen($_FILES['fitFile']['tmp_name'], 'r');
    stream_filter_append($rhandle, 'convert.base64-decode');

    $whandle = fopen($temp_filename, 'w');
    stream_copy_to_stream($rhandle, $whandle);
    fclose($rhandle);
    fclose($whandle);

    $swim_file = new swt\SwimFile($temp_filename);

    if (!(copy($temp_filename, $upload_dir.$internal_filename) && 
      rename($temp_filename, $download_dir.$internal_filename)))
      throw new Exception('Cannot copy file');

    $_SESSION['internal_filename'] = $internal_filename;
    $_SESSION['client_filename'] = $client_filename;

    $serial_number = $swim_file->getSerialNumber();
    setcookie('SN', $serial_number, time() + 60*60*24*7);

    echo "{ status: 'ok'}";
  }  
} catch (swt\FileNotValidException $ex) {
  echo "{status: 'error', message: '".$ex->getMessage()."'}";

  $filename = isset($_FILES['fitFile']['name']) ? $_FILES['fitFile']['name'] : '';
  swt\Functions::errorLog($ex, $filename);
} catch (Exception $ex) {
  echo "{status:'error',message:'Unexpected error has been logged, try again or "
    .'<a href="contact">Contact web site owner</a>'
    ."'}";

  $filename = isset($_FILES['fitFile']['name']) ? $_FILES['fitFile']['name'] : '';
  swt\Functions::errorLog($ex, $filename);
}

