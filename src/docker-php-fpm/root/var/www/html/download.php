<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try
{
  session_start();
  $file_id = NULL;
  $error ='';
  $convert = false;

  if (!isset($_SESSION['file_id'])) {
    $error  = '<p class="warning">Session has expired. Your data is discarded '
      .'after 20 minutes of inactivity. <a href="upload">Try again</a></p>';
  } else {

    if (isset($_GET['garmin']))
      $convert = true;

    $file_id = $_SESSION['file_id'];
    $path = swt\DB::convertFileIdToPath($file_id);

    if (!(copy($path.'EDIT', $path.'DOWNLOAD'))) 
      throw new Exception('Cannot update file');

    $activity = new swt\Activity($file_id);
    $activity->saveToDatabase();

    $filename = swt\DB::getFilename($file_id);

    if (preg_match('/\.fit$/i', $filename) == 1) {
      $filename = preg_replace('/\.fit/i', '_NEW.FIT', $filename);
    } else {
      $filename .= '_NEW.FIT';
    }

    if ($convert) {
      $swim_file = new swt\SwimFile($path.'EDIT');
      $swim_file->save($path.'GARMIN', true);
      $filename = preg_replace('/_NEW\.FIT$/', '_GARMIN.FIT', $filename);
    }

    swt\DB::addEditorLogEntry($file_id, swt\DB::EDITOR_ACTION_DOWNLOAD, NULL, NULL,
      NULL, NULL, NULL, NULL);

    header('Content-type: application/octet-stream');;
    header('Cache-Control: no-cache, no-store, must-revalidate');
    header('Pragma: no-cache');
    header('Expires: 0');
    header('Content-Disposition: filename="'.$filename.'"');
    if ($convert) 
      $handle = fopen($path.'GARMIN', 'rb');
    else 
      $handle = fopen($path.'DOWNLOAD', 'rb');
    fpassthru($handle);
    exit();
  }

} catch (Exception $ex) {
  $error = '<p class="warning">Unexpected error, the error have been '
    .'logged, sorry for the inconvenience.</p>';
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
}

echo $error;
swt\Layout::footer();
