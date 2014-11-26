<?php
namespace swt;

class Activity
{
  const SEARCHING = 0;
  const FOUND = 1;
  const SCANNING = 2;

  public $id;
  public $product_id;
  public $software_version;
  public $serial_number;
  public $date;
  public $pool_size_meters;
  public $pool_size_units;
  public $sub_total_num_lengths = 0;
  public $sub_total_time = 0.0;
  public $sub_total_stroke_count = 0;
  public $rest_time = 0.0;
  public $total_num_lengths = 0;
  public $total_time = 0.0;
  public $free_num_lengths = 0;
  public $free_time = 0.0;
  public $free_stroke_count = 0;
  public $back_num_lengths = 0;
  public $back_time = 0.0;
  public $back_stroke_count = 0;
  public $breast_num_lengths = 0;
  public $breast_time = 0.0;
  public $breast_stroke_count = 0;
  public $fly_num_lengths = 0;
  public $fly_time = 0.0;
  public $fly_stroke_count = 0;
  public $unknown_num_lengths = 0;
  public $unknown_time = 0.0;
  public $unknown_stroke_count = 0;
  public $drill_num_lengths = 0;
  public $drill_time = 0.0;
  public $best_50_free_num_lengths = 0;
  public $best_50_free_time = 0.0;
  public $best_50_free_stroke_count = 0;
  public $best_50_free_first_length = NULL;
  public $best_100_free_num_lengths = 0;
  public $best_100_free_time = 0.0;
  public $best_100_free_stroke_count = 0;
  public $best_100_free_first_length = NULL;
  public $best_200_free_num_lengths = 0;
  public $best_200_free_time = 0.0;
  public $best_200_free_stroke_count = 0;
  public $best_200_free_first_length = NULL;
  public $best_400_free_num_lengths = 0;
  public $best_400_free_time = 0.0;
  public $best_400_free_stroke_count = 0;
  public $best_400_free_first_length = NULL;
  public $best_500_free_num_lengths = 0;
  public $best_500_free_time = 0.0;
  public $best_500_free_stroke_count = 0;
  public $best_500_free_first_length = NULL;
  public $best_800_free_num_lengths = 0;
  public $best_800_free_time = 0.0;
  public $best_800_free_stroke_count = 0;
  public $best_800_free_first_length = NULL;
  public $best_1000_free_num_lengths = 0;
  public $best_1000_free_time = 0.0;
  public $best_1000_free_stroke_count = 0;
  public $best_1000_free_first_length = NULL;
  public $best_1500_free_num_lengths = 0;
  public $best_1500_free_time = 0.0;
  public $best_1500_free_stroke_count = 0;
  public $best_1500_free_first_length = NULL;
  public $best_1650_free_num_lengths = 0;
  public $best_1650_free_time = 0.0;
  public $best_1650_free_stroke_count = 0;
  public $best_1650_free_first_length = NULL;
  public $best_100_back_num_lengths = 0;
  public $best_100_back_time = 0.0;
  public $best_100_back_stroke_count = 0;
  public $best_100_back_first_length = NULL;
  public $best_200_back_num_lengths = 0;
  public $best_200_back_time = 0.0;
  public $best_200_back_stroke_count = 0;
  public $best_200_back_first_length = NULL;
  public $best_100_breast_num_lengths = 0;
  public $best_100_breast_time = 0.0;
  public $best_100_breast_stroke_count = 0;
  public $best_100_breast_first_length = NULL;
  public $best_200_breast_num_lengths = 0;
  public $best_200_breast_time = 0.0;
  public $best_200_breast_stroke_count = 0;
  public $best_200_breast_first_length = NULL;
  public $best_100_fly_num_lengths = 0;
  public $best_100_fly_time = 0.0;
  public $best_100_fly_stroke_count = 0;
  public $best_100_fly_first_length = NULL;
  public $best_200_fly_num_lengths = 0;
  public $best_200_fly_time = 0.0;
  public $best_200_fly_stroke_count = 0;
  public $best_200_fly_first_length = NULL;


  private $test_lengths = [
    ['lap' => 1, 'time' => 68, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 67, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 66, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 65, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 64, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 63, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 62, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 61, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 60, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 59, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 58, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 57, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 56, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 55, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 54, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 53, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 52, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 51, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 50, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 49, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 48, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 47, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 46, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 45, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 44, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 43, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 42, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 41, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 40, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 39, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 38, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 37, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 36, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 35, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 34, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 33, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 32, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 31, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 30, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 29, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 28, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 27, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 26, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 25, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 24, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 23, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 22, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 21, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 20, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 19, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 18, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 17, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 16, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 15, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 14, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 13, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 12, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 11, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 10, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 9, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 8, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 7, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 6, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 5, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 4, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 3, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 2, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0],
    ['lap' => 1, 'time' => 1, 'stroke' => 0, 'stroke_count' => 1, 'rest' => 0]
  ];



  function __construct($file_id) {

    $path = DB::convertFileIdToPath($file_id);
    if (file_exists($path.'DOWNLOAD'))
      $swim_file = new SwimFile($path.'DOWNLOAD');
    else
      $swim_file = new SwimFile($path.'UPLOAD');

    $this->id = $file_id;
    $this->product_id = $swim_file->getProduct();
    $this->software_version = $swim_file->getSoftwareVersion();
    $this->serial_number = $swim_file->getSerialNumber();
    $date = new \DateTime();
    $date->setTimezone(new \DateTimeZone('UTC'));
    $date->setTimestamp($swim_file->getDateCreated());
    $this->date = $date;
    $this->pool_size_meters = $swim_file->getPoolSize();
    $this->pool_size_units = $swim_file->getPoolSizeUnits();
    $this->lengths = $swim_file->getLengths();
    $this->getStrokeSummaries();
    $this->getBestTimes();

  }

  // LCM 50, 100, 200, 400, 800, 1500
  // SCM 50, 100, 200, 400, 800, 1500
  // SCY 50, 100, 200, 500, 1000, 1650
  private function getBestTimes()
  {
    if ($this->pool_size_meters == 25.0 || $this->pool_size_meters == 22.86) {

      $this->getRecord($this->best_50_free_num_lengths = 2,
        STROKE_FREESTYLE,
        $this->best_50_free_time,
        $this->best_50_free_stroke_count,
        $this->best_50_free_first_length);

      $this->getRecord($this->best_100_free_num_lengths = 4,
        STROKE_FREESTYLE,
        $this->best_100_free_time,
        $this->best_100_free_stroke_count,
        $this->best_100_free_first_length);

      $this->getRecord($this->best_200_free_num_lengths = 8,
        STROKE_FREESTYLE,
        $this->best_200_free_time,
        $this->best_200_free_stroke_count,
        $this->best_200_free_first_length);

      $this->getRecord($this->best_50_back_num_lengths = 2,
        STROKE_BACKSTROKE,
        $this->best_50_back_time,
        $this->best_50_back_stroke_count,
        $this->best_50_back_first_length);

      $this->getRecord($this->best_100_back_num_lengths = 4,
        STROKE_BACKSTROKE,
        $this->best_100_back_time,
        $this->best_100_back_stroke_count,
        $this->best_100_back_first_length);

      $this->getRecord($this->best_200_back_num_lengths = 8,
        STROKE_BACKSTROKE,
        $this->best_200_back_time,
        $this->best_200_back_stroke_count,
        $this->best_200_back_first_length);

      $this->getRecord($this->best_50_breast_num_lengths = 2,
        STROKE_BREASTSTROKE,
        $this->best_50_breast_time,
        $this->best_50_breast_stroke_count,
        $this->best_50_breast_first_length);

      $this->getRecord($this->best_100_breast_num_lengths = 4,
        STROKE_BREASTSTROKE,
        $this->best_100_breast_time,
        $this->best_100_breast_stroke_count,
        $this->best_100_breast_first_length);

      $this->getRecord($this->best_200_breast_num_lengths = 8,
        STROKE_BREASTSTROKE,
        $this->best_200_breast_time,
        $this->best_200_breast_stroke_count,
        $this->best_200_breast_first_length);

      $this->getRecord($this->best_50_fly_num_lengths = 2,
        STROKE_BUTTERFLY,
        $this->best_50_fly_time,
        $this->best_50_fly_stroke_count,
        $this->best_50_fly_first_length);

      $this->getRecord($this->best_100_fly_num_lengths = 4,
        STROKE_BUTTERFLY,
        $this->best_100_fly_time,
        $this->best_100_fly_stroke_count,
        $this->best_100_fly_first_length);

      $this->getRecord($this->best_200_fly_num_lengths = 8,
        STROKE_BUTTERFLY,
        $this->best_200_fly_time,
        $this->best_200_fly_stroke_count,
        $this->best_200_fly_first_length);
    }

    if ($this->pool_size_meters == 25) {

      $this->getRecord($this->best_400_free_num_lengths = 16,
        STROKE_FREESTYLE,
        $this->best_400_free_time,
        $this->best_400_free_stroke_count,
        $this->best_400_free_first_length);

      $this->getRecord($this->best_800_free_num_lengths = 32,
        STROKE_FREESTYLE,
        $this->best_800_free_time,
        $this->best_800_free_stroke_count,
        $this->best_800_free_first_length);

      $this->getRecord($this->best_1500_free_num_lengths = 60,
        STROKE_FREESTYLE,
        $this->best_1500_free_time,
        $this->best_1500_free_stroke_count,
        $this->best_1500_free_first_length);
    }

    if ($this->pool_size_meters == 22.86) {

      $this->getRecord($this->best_500_free_num_lengths = 20,
        STROKE_FREESTYLE,
        $this->best_500_free_time,
        $this->best_500_free_stroke_count,
        $this->best_500_free_first_length);

      $this->getRecord($this->best_1000_free_num_lengths = 40,
        STROKE_FREESTYLE,
        $this->best_1000_free_time,
        $this->best_1000_free_stroke_count,
        $this->best_1000_free_first_length);

      $this->getRecord($this->best_1650_free_num_lengths = 66,
        STROKE_FREESTYLE,
        $this->best_1650_free_time,
        $this->best_1650_free_stroke_count,
        $this->best_1650_free_first_length);
    }

    if ($this->pool_size_meters == 50.0) {

      $this->getRecord($this->best_50_free_num_lengths = 1,
        STROKE_FREESTYLE,
        $this->best_50_free_time,
        $this->best_50_free_stroke_count,
        $this->best_50_free_first_length);

      $this->getRecord($this->best_100_free_num_lengths = 2,
        STROKE_FREESTYLE,
        $this->best_100_free_time,
        $this->best_100_free_stroke_count,
        $this->best_100_free_first_length);

      $this->getRecord($this->best_200_free_num_lengths = 4,
        STROKE_FREESTYLE,
        $this->best_200_free_time,
        $this->best_200_free_stroke_count,
        $this->best_200_free_first_length);

      $this->getRecord($this->best_400_free_num_lengths = 8,
        STROKE_FREESTYLE,
        $this->best_400_free_time,
        $this->best_400_free_stroke_count,
        $this->best_400_free_first_length);

      $this->getRecord($this->best_800_free_num_lengths = 16,
        STROKE_FREESTYLE,
        $this->best_800_free_time,
        $this->best_800_free_stroke_count,
        $this->best_800_free_first_length);

      $this->getRecord($this->best_1500_free_num_lengths = 30,
        STROKE_FREESTYLE,
        $this->best_1500_free_time,
        $this->best_1500_free_stroke_count,
        $this->best_1500_free_first_length);

      $this->getRecord($this->best_50_back_num_lengths = 1,
        STROKE_BACKSTROKE,
        $this->best_50_back_time,
        $this->best_50_back_stroke_count,
        $this->best_50_back_first_length);

      $this->getRecord($this->best_100_back_num_lengths = 2,
        STROKE_BACKSTROKE,
        $this->best_100_back_time,
        $this->best_100_back_stroke_count,
        $this->best_100_back_first_length);

      $this->getRecord($this->best_200_back_num_lengths = 4,
        STROKE_BACKSTROKE,
        $this->best_200_back_time,
        $this->best_200_back_stroke_count,
        $this->best_200_back_first_length);

      $this->getRecord($this->best_50_breast_num_lengths = 1,
        STROKE_BREASTSTROKE,
        $this->best_50_breast_time,
        $this->best_50_breast_stroke_count,
        $this->best_50_breast_first_length);

      $this->getRecord($this->best_100_breast_num_lengths = 2,
        STROKE_BREASTSTROKE,
        $this->best_100_breast_time,
        $this->best_100_breast_stroke_count,
        $this->best_100_breast_first_length);

      $this->getRecord($this->best_200_breast_num_lengths = 4,
        STROKE_BREASTSTROKE,
        $this->best_200_breast_time,
        $this->best_200_breast_stroke_count,
        $this->best_200_breast_first_length);

      $this->getRecord($this->best_50_fly_num_lengths = 1,
        STROKE_BUTTERFLY,
        $this->best_50_fly_time,
        $this->best_50_fly_stroke_count,
        $this->best_50_fly_first_length);

      $this->getRecord($this->best_100_fly_num_lengths = 2,
        STROKE_BUTTERFLY,
        $this->best_100_fly_time,
        $this->best_100_fly_stroke_count,
        $this->best_100_fly_first_length);

      $this->getRecord($this->best_200_fly_num_lengths = 4,
        STROKE_BUTTERFLY,
        $this->best_200_fly_time,
        $this->best_200_fly_stroke_count,
        $this->best_200_fly_first_length);
    }
  }

  private function getStrokeSummaries()
  {
    foreach ($this->lengths as $length) {
      $this->total_num_lengths++;
      $this->total_time += ($length['time'] + $length['rest']);
      $this->rest_time += $length['rest'];

      switch ($length['stroke']) {
      case STROKE_FREESTYLE:
        $this->free_num_lengths++;
        $this->free_time += $length['time'];
        $this->free_stroke_count += $length['stroke_count'];
        break;

      case STROKE_BACKSTROKE:
        $this->back_num_lengths++;
        $this->back_time += $length['time'];
        $this->back_stroke_count += $length['stroke_count'];
        break;

      case STROKE_BREASTSTROKE:
        $this->breast_num_lengths++;
        $this->breast_time += $length['time'];
        $this->breast_stroke_count += $length['stroke_count'];
        break;

      case STROKE_BUTTERFLY:
        $this->fly_num_lengths++;
        $this->fly_time += $length['time'];
        $this->fly_stroke_count += $length['stroke_count'];
        break;

      case STROKE_MIXED:
      case STROKE_UNKNOWN:
        $this->unknown_num_lengths++;
        $this->unknown_time += $length['time'];
        $this->unknown_stroke_count += $length['stroke_count'];
        break;

      case STROKE_DRILL:
        $this->drill_num_lengths++;
        $this->drill_time += $length['time'];
        break;

      default:
        throw new \Exception('Unknown stroke type');

      }
    }
    $this->sub_total_num_lengths = $this->free_num_lengths + $this->back_num_lengths
      + $this->breast_num_lengths + $this->fly_num_lengths;
    $this->sub_total_time = $this->free_time + $this->back_time
      + $this->breast_time + $this->fly_time;
    $this->sub_total_stroke_count = $this->free_stroke_count + $this->back_stroke_count
      + $this->breast_stroke_count + $this->fly_stroke_count;

  }

  private function getRecord($num_lengths, $stroke, &$record_time, &$record_stroke_count, &$record_first_length)
  {
    $first_ptr = 0;
    $last_ptr = 0;
    $first_length;
    $last_length;
    $stroke_count = 0;
    $time = 0.0;
    $one_day = 3600.0;
    $state = self::SEARCHING;

    $record_time = 3600.0;
    $record_stroke_count = 0;
    $record_first_length = NULL;

    while ($last_ptr < count($this->lengths)) {
      $first_length = $this->lengths[$first_ptr];
      $last_length = $this->lengths[$last_ptr];

      $reset = FALSE;

      switch ($state) {
      case self::SEARCHING:

        if ($first_length['stroke'] == $stroke) {
          $time = $first_length['time'];
          $stroke_count = $first_length['stroke_count'];

          if ($num_lengths == 1) {
            if ($time < $record_time) {
              $record_time = $first_length['time'];
              $record_stroke_count = $first_length['stroke_count'];
              $record_first_length = $first_ptr;
            }
            $last_ptr++;
            $state = self::SCANNING;

          } else {
            if ($first_length['rest'] == 0) {
              $last_ptr++;
              $state = self::FOUND;
            } else {
              $reset = TRUE;
            }
          }
        } else {
          $first_ptr++;
          $last_ptr = $first_ptr;
        }
        break;

      case self::FOUND:
        if ($last_length['stroke'] == $stroke) {

          $time += $last_length['time'];
          $stroke_count += $last_length['stroke_count'];

          if (($last_ptr - $first_ptr) == ($num_lengths - 1)) {
            if ($time < $record_time) {
              $record_time = $time;
              $record_stroke_count = $stroke_count;
              $record_first_length = $first_ptr;
            }

            if ($last_length['rest'] == 0) {
              $last_ptr++;
              $state = self::SCANNING;
            } else { // rest > 0
              $reset = TRUE;
            }
          } else if (($last_ptr - $first_ptr) < ($num_lengths - 1)) {
            if ($last_length['rest'] == 0) {
              $last_ptr++;
            } else { //rest > 0
              $reset = TRUE;
            }
          }
        } else { // different stroke
          $reset = TRUE;
        }

        break;

      case self::SCANNING:

        if ($last_length['stroke'] == $stroke) {
          $stroke_count += $last_length['stroke_count'];
          $time += $last_length['time'];
          $stroke_count -= $first_length['stroke_count'];
          $time -= $first_length['time'];

          if ($time < $record_time) {
            $record_time = $time;
            $record_stroke_count = $stroke_count;
            $record_first_length = $first_ptr + 1;
          }
          if ($last_length['rest'] == 0) {
            $first_ptr++;
            $last_ptr++;
          } else {
            $reset = TRUE;
          }
        } else {
          $reset = TRUE;
        }
        break;
      }

      if ($reset) {
        $time = 0;
        $stroke_count = 0;
        $last_ptr++;
        $first_ptr = $last_ptr;
        $state = self::SEARCHING;
      }
    }
    if ($record_time == $one_day) $record_time = 0.0;
  }

  public function saveToDatabase() {

    $dbh = DB::getConnection();

    $sql = 'select activity_id from activities where activity_id = ?';
    $sth = $dbh->prepare($sql);
    $sth->execute([$this->id]);

    if (!$sth->fetch()) {
      $sql = 'INSERT INTO activities
        (activity_id, product_id, software_version, serial_number, date,
        pool_size_meters, pool_size_units)
        VALUES (?,?,?,?,?,?,?)';
      $sth = $dbh->prepare($sql);
      $sth->execute([$this->id, $this->product_id, $this->software_version, $this->serial_number,
        $this->date->format('Y-m-d H:i:s'), $this->pool_size_meters, $this->pool_size_units]);
    } else {
      $sql = 'UPDATE activities
        SET  product_id = ?, software_version = ?, serial_number = ?, date = ?,
        pool_size_meters = ?, pool_size_units = ?
        WHERE activity_id = ?';
      $sth = $dbh->prepare($sql);
      $sth->execute([$this->product_id, $this->software_version, $this->serial_number,
        $this->date->format('Y-m-d H:i:s'), $this->pool_size_meters, $this->pool_size_units,$this->id]);
    }
  }

}


