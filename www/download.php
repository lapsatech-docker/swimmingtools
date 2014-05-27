<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try
{
  session_start();
  $error ='';

  if (!isset($_SESSION['internal_filename'])) {
    $error  = '<p class="warning">Session has expired. Your data is discarded '
      .'after 20 minutes of inactivity. <a href="upload">Try again</a></p>';
  } else {

    $internal_filename = realpath(swt\Functions::DOWNLOAD_DIR.$_SESSION['internal_filename']);
    $client_filename = $_SESSION['client_filename'];

    if (preg_match('/\.fit$/i', $client_filename) == 1) {
      $client_filename = preg_replace('/\.fit/i', '_NEW.FIT', $client_filename);
    } else {
      $client_filename .= '_NEW.FIT';
    }

    header('Content-type: application/octet-stream');;
    header('Cache-Control: no-cache, no-store, must-revalidate');
    header('Pragma: no-cache');
    header('Expires: 0');
    header('Content-Disposition: filename="'.$client_filename.'"');
    $handle = fopen($internal_filename,'rb');
    fpassthru($handle);
    exit();
  }

} catch (Exception $ex) {
  $error = '<p class="warning">Unexpected error, the error have been '
    .'logged, sorry for the inconvenience.</p>';
  \swt\Functions::logError($ex);
}

\swt\Layout::header('Swimming Watch Data Editor - Contact', \swt\Layout::TAB_CONTACT);
echo $error;
\swt\Layout::footer();
