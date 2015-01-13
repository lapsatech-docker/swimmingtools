<?php
namespace swt;


class Functions {

  public static $pool_size = NULL;

  public static $stroke_lookup = [
    STROKE_FREESTYLE => ['string' => 'Freestyle', 'color' => '#c43dbf'],
    STROKE_BACKSTROKE => ['string' => 'Backstroke', 'color' => '#1f8ef9'],
    STROKE_BREASTSTROKE => ['string' => 'Breaststroke', 'color' => '#95de2b'],
    STROKE_BUTTERFLY => ['string' => 'Butterfly', 'color' => '#eb3d3d'],
    STROKE_DRILL => ['string' => 'Drill', 'color' => '#ff7c05'],
    STROKE_MIXED => ['string' => 'Mixed', 'color' => 'gray'],
    STROKE_UNKNOWN => ['string' => 'Unknown', 'color' => 'gray']];


  public static function errorHandler($errno, $errstr, $errfile, $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
  }

  public static function registerErrorHandler() {
    set_error_handler("\swt\Functions::errorHandler");
  }

  public static function formatTime($time_in_seconds, $show_fraction_second = FALSE)
  {
    $deciseconds = ($time_in_seconds - floor($time_in_seconds)) * 10;
    $seconds = (int) floor($time_in_seconds);
    $minutes = (int) ($seconds / 60);
    $hours = (int) ($minutes / 60);

    $seconds = $seconds % 60;
    $minutes = $minutes % 60;

    if ($hours > 0) 
      $time_string = sprintf('%d:%\'02d:%\'02d', $hours, $minutes, $seconds);
    else
      $time_string = sprintf('%d:%\'02d', $minutes, $seconds);

    if ($show_fraction_second)
      $time_string = sprintf('%s.%.0f', $time_string, $deciseconds);

    return $time_string;
  }

  public static function getPace($distance, $time, $as_string = false)
  {
    $pace = NULL;
    if ($distance > 0) {
      $pace = 100 * $time / $distance;
      if ($as_string) 
        $pace = self::formatTime($pace);
    }
    return $pace;
  }

  public static function getSpm($stroke_count, $time)
  {
    $spm = NULL;
    if ($time > 0) 
      $spm = 60 * $stroke_count / $time;

    return $spm;
  }

  public static function getSpl($stroke_count, $num_lengths)
  {
    $spl = NULL;
    if ($num_lengths > 0) 
      $spl = $stroke_count / $num_lengths;

    return $spl;
  }

  public static function getDistance($num_lengths)
  {
    $distance = NULL;
    if (self::$pool_size == 33.33) {
      $distance = $num_lengths * 33;
      $distance += floor($num_lengths/3);
      if (($num_lengths % 3) == 1)
        $distance += 0.33;
      else if (($num_lengths % 3) == 2)
        $distance += 0.66;
    } else {
      $distance = $num_lengths * self::$pool_size;
    }
    return $distance;
  }

  public static function getComputedFields($num_lengths, $time, $stroke_count, $stroke = STROKE_UNKNOWN)
  {
    $fields = array();
    $fields['stroke'] = self::$stroke_lookup[$stroke]['string'];
    $fields['color'] = self::$stroke_lookup[$stroke]['color'];
    $fields['distance'] = self::getDistance($num_lengths);
    $fields['pace'] = self::getPace($fields['distance'], $time, FALSE);
    $fields['pace_str'] = self::getPace($fields['distance'], $time, TRUE);
    $fields['spl'] = self::getSpl($stroke_count, $num_lengths);
    $fields['spm'] = self::getSpm($stroke_count, $time);
    $fields['time'] = self::formatTime($time);
    return $fields;
  }

  public static function getOrdinalNumber($num) {
    if (!in_array(($num % 100),array(11,12,13))){
      switch ($num % 10) {
        //Handle 1st, 2nd, 3rd
      case 1:  return $num.'st';
      case 2:  return $num.'nd';
      case 3:  return $num.'rd';
      }
    }
    return $num.'th';
  }
}  
