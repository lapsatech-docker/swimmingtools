<?php
require_once('swt/db.php');
require_once('swt/activity.php');
require_once('swt/summary.php');

$upload_dir = '/home/stef/swt/www/data/upload/';
$download_dir = '/home/stef/swt/www/data/download/';

$dh = opendir($upload_dir);

while (($file = readdir($dh)) !== false) {
  try {
    if ($file != '.' && $file != '..') {
      $swim_file = new swt\SwimFile($upload_dir.$file);
      $swim_file->getLengths();
    }
  } catch (Exception $ex) {
    echo $file.': '.$ex.PHP_EOL;
    unlink($upload_dir.$file);
    if (file_exists($download_dir.$file))
      unlink($download_dir.$file);
  }
}
closedir($dh);


$dh = opendir($upload_dir);

while ($file = readdir($dh)) {
  try {
    if ($file != '.' && $file != '..') {

      if (preg_match('/^\d*_(.*\.fit)[[:xdigit:]]*/i', $file, $matches) === 1) {
        $filename = $matches[1];
      } else {
        $filename = $file;
      }

      $file_id = swt\DB::addFile($filename, filemtime($upload_dir.$file));
      $path = swt\DB::convertFileIdToPath($file_id);
      if (!rename($upload_dir.$file, $path.'UPLOAD'))
        throw new Exception('Cannot copy upload file '.$file);

      if (file_exists($download_dir.$file)) {
        if (!rename($download_dir.$file, $path.'DOWNLOAD'))
          throw new Exception('Cannot copy download file '.$file);
      }
      $activity = new swt\Activity($file_id);
    }
  } catch (Exception $ex) {
    echo $ex.PHP_EOL;
  }
}
