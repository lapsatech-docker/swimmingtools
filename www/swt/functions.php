<?php
namespace swt;


class Functions {
  
  const TEMP_DIR = 'data/temp/';
  const UPLOAD_DIR = 'data/upload/';
  const DOWNLOAD_DIR = 'data/download/';

  static function errorHandler($errno, $errstr, $errfile, $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
  }

  static function registerErrorHandler() {
    set_error_handler("\swt\Functions::errorHandler");
  }

  static function errorLog(\Exception $ex, $error = '') {
    try {
      if (!empty($error)) 
        $error .= PHP_EOL;
      if (isset($_COOKIE['SN'])) 
        $error .= 'SN:'.$_COOKIE['SN'].PHP_EOL;
      if (isset($_SESSION['internal_filename']))
        $error .= 'FILE:'.$_SESSION['internal_filename'].PHP_EOL;
      $error .= $ex;

      \error_log($error);
    } catch (\Exception $ex) {
    }
  }
}  

