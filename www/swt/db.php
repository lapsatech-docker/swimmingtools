<?php
namespace swt;

class DB {

  //Make sure this is synchronized with values from editor_actions table
  const EDITOR_ACTION_MERGE = 1;
  const EDITOR_ACTION_SPLIT = 2;
  const EDITOR_ACTION_CHANGE_STROKE = 3;
  const EDITOR_ACTION_DELETE = 4;
  const EDITOR_ACTION_CHANGE_POOL_SIZE = 5;
  const EDITOR_ACTION_UNDO_ALL = 6;
  const EDITOR_ACTION_DOWNLOAD = 7;

  private static $dbh;

  public static function getConnection()
  {
    if (!isset(self::$dbh)) {
      self::$dbh = new \PDO('mysql:host=localhost;dbname=swtdb', 'apache', 'b1ozvc30');
      self::$dbh->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
    }
    return self::$dbh;
  }

  public static function addEditorLogEntry($file_id, $action, $length_index,
    $change_stroke_from, $change_stroke_to, $change_stroke_option,
    $new_pool_size, $new_pool_size_units)
  {
      $sbh = self::getConnection();
      $sql = 'INSERT INTO editor_log (activity_id, action_id, length_index, 
        change_stroke_from, change_stroke_to, change_stroke_option,
        new_pool_size, new_pool_size_units) VALUES (?,?,?,?,?,?,?,?)';
      $sth = $sbh->prepare($sql);
      $sth->execute([$file_id, $action, $length_index,
        $change_stroke_from, $change_stroke_to, $change_stroke_option,
        $new_pool_size, $new_pool_size_units]);
    }

  public static function addErrorLogEntry($file_id, $script, $message)
  {
    try {
      $dbh = self::getConnection();
      $sql = 'INSERT INTO error_log (file_id, script, message) values (?, ?, ?)';
      $sth = $dbh->prepare($sql);
      $sth->execute([$file_id, $script, $message]);
    } catch(\Exception $ex) {
      error_log($ex);
    }
  }

  public static function addFile($filename, $uploaded = NULL)
  {
    $dbh = self::getConnection();
    $sql = 'INSERT INTO files (name, uploaded) VALUES (?,?)';
    $sth = $dbh->prepare($sql);
    if (is_null($uploaded)) {
      $sth->execute([$filename, NULL]);
    } else {
      $sth->execute([$filename, date('Y-m-d H:i:s', $uploaded)]);
    }
    return $dbh->lastInsertId();
  }

  public static function convertFileIdToPath($file_id)
  {
    $sub_dir = $file_id % 1000;
    $sub_dir = sprintf('%03d', $sub_dir);
    return 'data'.DIRECTORY_SEPARATOR.$sub_dir.DIRECTORY_SEPARATOR.$file_id;
  }

  public static function createFileFromUpload($upload)
  {
    // Garmin Connect exports activity file as  zip files containing the
    // original fit file. This code Checks if the uploaded file is a zip
    // file and extract the fit file
    $zip_file = new \ZipArchive();
    if ( $zip_file->open($_FILES[$upload]['tmp_name']) === true) {
      $filename = basename($zip_file->getNameIndex(0));
      $copy_from = 'zip://'.$zip_file->filename.'#'.$zip_file->getNameIndex(0);
      $zip_file->close();

      $file_id = self::addFile($filename);
      $copy_to = self::convertFileIdToPath($file_id).'UPLOAD';

      if (!copy($copy_from, $copy_to)) {
        throw new \Exception('Cannot extract file from zip');
      }
    } else {
      // File is a regular file
      $filename = $_FILES[$upload]['name'];

      // IE sometimes send complete path to filename on user machine, if
      // this is the case, we just want the filename, this pattern match
      // a windows path
      $pattern =  '/^(?:[a-zA-Z]\:(\\\\|\/)|file\:\/\/|\\\\\\\\|\.(\/|\\\\))([^\\\\\\/\:\*\?\<\>\"\|]+(\\\\|\/){0,1})+$/';
      if (preg_match($pattern, $filename) == 1) {
        if (preg_match('/[^\\\\\/]*$/', $filename, $matches) == 1)
          $filename = $matches[0];
      }

      $file_id = self::addFile($filename);
      $copy_to = self::ConvertFileIdToPath($file_id).'UPLOAD';
      if (!move_uploaded_file($_FILES[$upload]['tmp_name'], $copy_to)) {
        throw new \Exception('Cannot move uploaded file to data directory');
      }
    }
    return $file_id;
  }

  public static function getFilename($file_id)
  {
    $dbh = self::getConnection();
    $sql = 'SELECT name FROM files WHERE file_id = ?';
    $sth = $dbh->prepare($sql);
    $sth->execute([$file_id]);
    if ($filename = $sth->fetchColumn())
      return $filename;
    else
     throw new \Exception('Cannot retrieve filename for file_id = '.$file_id);
  }
}
