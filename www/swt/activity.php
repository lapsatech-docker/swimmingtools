<?php
namespace swt;

class Activity
{
  const SEARCHING = 0;
  const FOUND = 1;
  const SCANNING = 2;

  const SUM_FREE = 0;
  const SUM_BACK = 1;
  const SUM_BREAST = 2;
  const SUM_FLY = 3;
  const SUM_UNK = 4;
  const SUM_SUB = 5;
  const SUM_DRILL = 6;
  const SUM_REST = 7;
  const SUM_TOTAL = 8;

  public static $BEST_PARAMS = [
    ['distance' => '50', 'stroke' => STROKE_FREESTYLE, 'lcm' => 1, 'scm' => 2, 'scy' => 2],
    ['distance' => '100', 'stroke' => STROKE_FREESTYLE, 'lcm' => 2, 'scm' => 4, 'scy' => 4],
    ['distance' => '200', 'stroke' => STROKE_FREESTYLE, 'lcm' => 4, 'scm' => 8, 'scy' => 8],
    ['distance' => '400', 'stroke' => STROKE_FREESTYLE, 'lcm' => 8, 'scm' => 16, 'scy' => 16],
    ['distance' => '500', 'stroke' => STROKE_FREESTYLE, 'lcm' => NULL, 'scm' => NULL, 'scy' => 20],
    ['distance' => '800', 'stroke' => STROKE_FREESTYLE, 'lcm' => 16, 'scm' => 32, 'scy' => 32],
    ['distance' => '1000', 'stroke' => STROKE_FREESTYLE, 'lcm' => NULL, 'scm' => NULL, 'scy' => 40],
    ['distance' => '1500', 'stroke' => STROKE_FREESTYLE, 'lcm' => 30, 'scm' => 60, 'scy' => 60],
    ['distance' => '1650', 'stroke' => STROKE_FREESTYLE, 'lcm' => NULL, 'scm' => NULL, 'scy' => 66],
    ['distance' => '50', 'stroke' => STROKE_BACKSTROKE, 'lcm' => 1, 'scm' => 2, 'scy' => 2],
    ['distance' => '100', 'stroke' => STROKE_BACKSTROKE, 'lcm' => 2, 'scm' => 4, 'scy' => 4],
    ['distance' => '200', 'stroke' => STROKE_BACKSTROKE, 'lcm' => 4, 'scm' => 8, 'scy' => 8],
    ['distance' => '50', 'stroke' => STROKE_BREASTSTROKE, 'lcm' => 1, 'scm' => 2, 'scy' => 2],
    ['distance' => '100', 'stroke' => STROKE_BREASTSTROKE, 'lcm' => 2, 'scm' => 4, 'scy' => 4],
    ['distance' => '200', 'stroke' => STROKE_BREASTSTROKE, 'lcm' => 4, 'scm' => 8, 'scy' => 8],
    ['distance' => '50', 'stroke' => STROKE_BUTTERFLY, 'lcm' => 1, 'scm' => 2, 'scy' => 2],
    ['distance' => '100', 'stroke' => STROKE_BUTTERFLY, 'lcm' => 2, 'scm' => 4, 'scy' => 4],
    ['distance' => '200', 'stroke' => STROKE_BUTTERFLY, 'lcm' => 4, 'scm' => 8, 'scy' => 8]
  ];

  public $id;
  public $product_id;
  public $software_version;
  public $serial_number;
  public $date;
  public $pool_size_meters;
  public $pool_size_units;
  public $pool_size;
  public $summaries = array();
  public $bests = array();

  function __construct($file_id)
  {

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
    $this->pool_size = $this->pool_size_meters;
    if ($this->pool_size_units == UNITS_STATUTE)
      $this->pool_size = $this->pool_size_meters * 1.09361;
    $this->pool_size = round($this->pool_size, 2);

    $this->getSummaries();
    $this->getBestTimes();

  }

  private function getBestTimes()
  {
    $course = '';
    if ($this->pool_size == 50.0 && $this->pool_size_units == UNITS_METRIC) {
      $course = 'lcm';
    }

    if ($this->pool_size == 25.0 && $this->pool_size_units == UNITS_METRIC) {
      $course = 'scm';
    }

    if ($this->pool_size == 25.0 && $this->pool_size_units == UNITS_STATUTE) {
      $course = 'scy';
    }

    if ($course != '') {
      foreach(self::$BEST_PARAMS as $best_index => $params) {
        $this->bests[$best_index] = $this->getBestTime($best_index, $course);
      }
    }
  }

  private function getBestTime($best_index, $course)
  {
    if (self::$BEST_PARAMS[$best_index][$course] == NULL) {
      return NULL;
    } else {

      $first_ptr = 0;
      $last_ptr = 0;
      $first_length;
      $last_length;
      $stroke_count = 0;
      $time = 0.0;
      $one_day = 3600.0;
      $state = self::SEARCHING;

      $num_lengths = self::$BEST_PARAMS[$best_index][$course];
      $stroke = self::$BEST_PARAMS[$best_index]['stroke'];

      $best_time = 3600.0;
      $best_stroke_count = 0;
      $best_first_length = NULL;

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
              if ($time < $best_time) {
                $best_time = $first_length['time'];
                $best_stroke_count = $first_length['stroke_count'];
                $best_first_length = $first_ptr;
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
              if ($time < $best_time) {
                $best_time = $time;
                $best_stroke_count = $stroke_count;
                $best_first_length = $first_ptr;
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

            if ($time < $best_time) {
              $best_time = $time;
              $best_stroke_count = $stroke_count;
              $best_first_length = $first_ptr + 1;
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
      if ($best_time == $one_day) 
        return NULL;
      else 
        return ['num_lengths' => $num_lengths, 'time' => $best_time, 
          'stroke_count' => $best_stroke_count, 'first_length' => $best_first_length];
    }
  }


  private function getSummaries()
  {
    $sums =  &$this->summaries;
    $sums[self::SUM_FREE] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_BACK] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_BREAST] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_FLY] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_UNK] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_SUB] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_DRILL] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_REST] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];
    $sums[self::SUM_TOTAL] = ['num_lengths' => 0, 'time' => 0.0, 'stroke_count' => 0];


    foreach ($this->lengths as $length) {
      $sums[self::SUM_TOTAL]['num_lengths']++;
      $sums[self::SUM_TOTAL]['time'] += ($length['time'] + $length['rest']);
      $sums[self::SUM_REST]['time'] += $length['rest'];

      switch ($length['stroke']) {
      case STROKE_FREESTYLE:
        $sums[self::SUM_FREE]['num_lengths']++;
        $sums[self::SUM_FREE]['time'] += $length['time'];
        $sums[self::SUM_FREE]['stroke_count'] += $length['stroke_count'];
        break;

      case STROKE_BACKSTROKE:
        $sums[self::SUM_BACK]['num_lengths']++;
        $sums[self::SUM_BACK]['time'] += $length['time'];
        $sums[self::SUM_BACK]['stroke_count'] += $length['stroke_count'];
        break;

      case STROKE_BREASTSTROKE:
        $sums[self::SUM_BREAST]['num_lengths']++;
        $sums[self::SUM_BREAST]['time'] += $length['time'];
        $sums[self::SUM_BREAST]['stroke_count'] += $length['stroke_count'];
        break;

      case STROKE_BUTTERFLY:
        $sums[self::SUM_FLY]['num_lengths']++;
        $sums[self::SUM_FLY]['time'] += $length['time'];
        $sums[self::SUM_FLY]['stroke_count'] += $length['stroke_count'];
        break;

      case STROKE_MIXED:
      case STROKE_UNKNOWN:
        $sums[self::SUM_UNK]['num_lengths']++;
        $sums[self::SUM_UNK]['time'] += $length['time'];
        $sums[self::SUM_UNK]['stroke_count'] += $length['stroke_count'];
        break;

      case STROKE_DRILL:
        $sums[self::SUM_DRILL]['num_lengths']++;
        $sums[self::SUM_DRILL]['time'] += $length['time'];
        break;

      default:
        throw new \Exception('Unknown stroke type');

      }
    }
    $sums[self::SUM_SUB]['num_lengths'] =
      $sums[self::SUM_FREE]['num_lengths'] + 
      $sums[self::SUM_BACK]['num_lengths'] + 
      $sums[self::SUM_BREAST]['num_lengths'] + 
      $sums[self::SUM_FLY]['num_lengths']; 
      
    $sums[self::SUM_SUB]['time'] =
      $sums[self::SUM_FREE]['time'] + 
      $sums[self::SUM_BACK]['time'] + 
      $sums[self::SUM_BREAST]['time'] + 
      $sums[self::SUM_FLY]['time']; 

    $sums[self::SUM_SUB]['stroke_count'] =
      $sums[self::SUM_FREE]['stroke_count'] + 
      $sums[self::SUM_BACK]['stroke_count'] + 
      $sums[self::SUM_BREAST]['stroke_count'] + 
      $sums[self::SUM_FLY]['stroke_count']; 
  }

  public function saveToDatabase()
  {

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


