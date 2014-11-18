<?php
namespace swt;


class Functions {

  public static $stroke_lookup = [
    STROKE_FREESTYLE => ['string' => 'Freestyle', 'color' => '#c43dbf'],
    STROKE_BACKSTROKE => ['string' => 'Backstroke', 'color' => '#1f8ef9'],
    STROKE_BREASTSTROKE => ['string' => 'Breaststroke', 'color' => '#95de2b'],
    STROKE_BUTTERFLY => ['string' => 'Butterfly', 'color' => '#eb3d3d'],
    STROKE_DRILL => ['string' => 'Drill', 'color' => '#ff7c05'],
    STROKE_MIXED => ['string' => 'Mixed', 'color' => 'gray'],
    STROKE_UNKNOWN => ['string' => 'Unknown', 'color' => 'gray']];
  

  static function errorHandler($errno, $errstr, $errfile, $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
  }

  static function registerErrorHandler() {
    set_error_handler("\swt\Functions::errorHandler");
  }

    
}  

