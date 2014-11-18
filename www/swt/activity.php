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
  public $summaries;
  public $sub_total_nb_lengths;
  public $sub_total_time;
  public $sub_total_stroke_count;
  public $total_nbr_lengths;
  public $total_time;

  function __construct($file_id) {

    foreach (Summary::$TYPES as $type) 
      $this->summaries[$type] = new Summary($type);

    $this->id = $file_id;
    $path = DB::convertFileIdToPath($file_id);
    $swim_file = new SwimFile($path.'UPLOAD');
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
    $this->saveToDatabase();

  }



  // LCM 50, 100, 200, 400, 800, 1500
  // SCM 50, 100, 200, 400, 800, 1500
  // SCY 50, 100, 200, 500, 1000, 1650
  private function GetBestTimes()
  {
    if ($this->pool_size_meters == 25.0 || $this->pool_size_meters == 22.86) {

      $this->GetRecord($this->summaries['freestyle50']->nbr_lengths = 2, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle50']->time, 
        $this->summaries['freestyle50']->stroke_count, 
        $this->summaries['freestyle50']->first_length);

      $this->GetRecord($this->summaries['freestyle100']->nbr_lengths = 4, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle100']->time, 
        $this->summaries['freestyle100']->stroke_count, 
        $this->summaries['freestyle100']->first_length);

      $this->GetRecord($this->summaries['freestyle200']->nbr_lengths = 8, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle200']->time, 
        $this->summaries['freestyle200']->stroke_count, 
        $this->summaries['freestyle200']->first_length);

      $this->GetRecord($this->summaries['backstroke50']->nbr_lengths = 2, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke50']->time, 
        $this->summaries['backstroke50']->stroke_count, 
        $this->summaries['backstroke50']->first_length);

      $this->GetRecord($this->summaries['backstroke100']->nbr_lengths = 4, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke100']->time, 
        $this->summaries['backstroke100']->stroke_count, 
        $this->summaries['backstroke100']->first_length);

      $this->GetRecord($this->summaries['backstroke200']->nbr_lengths = 8, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke200']->time, 
        $this->summaries['backstroke200']->stroke_count, 
        $this->summaries['backstroke200']->first_length);

      $this->GetRecord($this->summaries['breaststroke50']->nbr_lengths = 2, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke50']->time, 
        $this->summaries['breaststroke50']->stroke_count, 
        $this->summaries['breaststroke50']->first_length);

      $this->GetRecord($this->summaries['breaststroke100']->nbr_lengths = 4, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke100']->time, 
        $this->summaries['breaststroke100']->stroke_count, 
        $this->summaries['breaststroke100']->first_length);

      $this->GetRecord($this->summaries['breaststroke200']->nbr_lengths = 8, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke200']->time, 
        $this->summaries['breaststroke200']->stroke_count, 
        $this->summaries['breaststroke200']->first_length);

      $this->GetRecord($this->summaries['butterfly50']->nbr_lengths = 22, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly50']->time, 
        $this->summaries['butterfly50']->stroke_count, 
        $this->summaries['butterfly50']->first_length);

      $this->GetRecord($this->summaries['butterfly100']->nbr_lengths = 4, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly100']->time, 
        $this->summaries['butterfly100']->stroke_count, 
        $this->summaries['butterfly100']->first_length);

      $this->GetRecord($this->summaries['butterfly200']->nbr_lengths = 8, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly200']->time, 
        $this->summaries['butterfly200']->stroke_count, 
        $this->summaries['butterfly200']->first_length);
    }

    if ($this->pool_size_meters == 25) {

      $this->GetRecord($this->summaries['freestyle400']->nbr_lengths = 16, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle400']->time, 
        $this->summaries['freestyle400']->stroke_count, 
        $this->summaries['freestyle400']->first_length);

      $this->GetRecord($this->summaries['freestyle800']->nbr_lengths = 32, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle800']->time, 
        $this->summaries['freestyle800']->stroke_count, 
        $this->summaries['freestyle800']->first_length);

      $this->GetRecord($this->summaries['freestyle1500']->nbr_lengths = 60, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle1500']->time, 
        $this->summaries['freestyle1500']->stroke_count, 
        $this->summaries['freestyle1500']->first_length);
    }

    if ($this->pool_size_meters == 22.86) {

      $this->GetRecord($this->summaries['freestyle500']->nbr_lengths = 20, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle500']->time, 
        $this->summaries['freestyle500']->stroke_count, 
        $this->summaries['freestyle500']->first_length);

      $this->GetRecord($this->summaries['freestyle1000']->nbr_lengths = 40, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle1000']->time, 
        $this->summaries['freestyle1000']->stroke_count, 
        $this->summaries['freestyle1000']->first_length);

      $this->GetRecord($this->summaries['freestyle1650']->nbr_lengths = 66, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle1650']->time, 
        $this->summaries['freestyle1650']->stroke_count, 
        $this->summaries['freestyle1650']->first_length);
    }

    if ($this->pool_size_meters == 50.0) {

      $this->GetRecord($this->summaries['freestyle50']->nbr_lengths = 1, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle50']->time, 
        $this->summaries['freestyle50']->stroke_count, 
        $this->summaries['freestyle50']->first_length);

      $this->GetRecord($this->summaries['freestyle100']->nbr_lengths = 2, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle100']->time, 
        $this->summaries['freestyle100']->stroke_count, 
        $this->summaries['freestyle100']->first_length);

      $this->GetRecord($this->summaries['freestyle200']->nbr_lengths = 4, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle200']->time, 
        $this->summaries['freestyle200']->stroke_count, 
        $this->summaries['freestyle200']->first_length);

      $this->GetRecord($this->summaries['freestyle400']->nbr_lengths = 8, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle400']->time, 
        $this->summaries['freestyle400']->stroke_count, 
        $this->summaries['freestyle400']->first_length);

      $this->GetRecord($this->summaries['freestyle800']->nbr_lengths = 16, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle800']->time, 
        $this->summaries['freestyle800']->stroke_count, 
        $this->summaries['freestyle800']->first_length);

      $this->GetRecord($this->summaries['freestyle1500']->nbr_lengths = 30, 
        STROKE_FREESTYLE, 
        $this->summaries['freestyle1500']->time, 
        $this->summaries['freestyle1500']->stroke_count, 
        $this->summaries['freestyle1500']->first_length);

      $this->GetRecord($this->summaries['backstroke50']->nbr_lengths = 1, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke50']->time, 
        $this->summaries['backstroke50']->stroke_count, 
        $this->summaries['backstroke50']->first_length);

      $this->GetRecord($this->summaries['backstroke100']->nbr_lengths = 2, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke100']->time, 
        $this->summaries['backstroke100']->stroke_count, 
        $this->summaries['backstroke100']->first_length);

      $this->GetRecord($this->summaries['backstroke200']->nbr_lengths = 4, 
        STROKE_BACKSTROKE, 
        $this->summaries['backstroke200']->time, 
        $this->summaries['backstroke200']->stroke_count, 
        $this->summaries['backstroke200']->first_length);

      $this->GetRecord($this->summaries['breaststroke50']->nbr_lengths = 1, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke50']->time, 
        $this->summaries['breaststroke50']->stroke_count, 
        $this->summaries['breaststroke50']->first_length);

      $this->GetRecord($this->summaries['breaststroke100']->nbr_lengths = 2, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke100']->time, 
        $this->summaries['breaststroke100']->stroke_count, 
        $this->summaries['breaststroke100']->first_length);

      $this->GetRecord($this->summaries['breaststroke200']->nbr_lengths = 4, 
        STROKE_BREASTSTROKE, 
        $this->summaries['breaststroke200']->time, 
        $this->summaries['breaststroke200']->stroke_count, 
        $this->summaries['breaststroke200']->first_length);

      $this->GetRecord($this->summaries['butterfly50']->nbr_lengths = 1, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly50']->time, 
        $this->summaries['butterfly50']->stroke_count, 
        $this->summaries['butterfly50']->first_length);

      $this->GetRecord($this->summaries['butterfly100']->nbr_lengths = 2, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly100']->time, 
        $this->summaries['butterfly100']->stroke_count, 
        $this->summaries['butterfly100']->first_length);

      $this->GetRecord($this->summaries['butterfly200']->nbr_lengths = 4, 
        STROKE_BUTTERFLY, 
        $this->summaries['butterfly200']->time, 
        $this->summaries['butterfly200']->stroke_count, 
        $this->summaries['butterfly200']->first_length);
    }
  }

  private function getStrokeSummaries()
  {
    foreach ($this->lengths as $length) {
      $this->total_time += $length['time'];

      if ($length['type'] == LENGTH_ACTIVE) {

        $this->total_nbr_lengths++;

        switch ($length['stroke']) {
        case STROKE_FREESTYLE:
          $this->summaries['freestyle']->nbr_lengths++;
          $this->summaries['freestyle']->time += $length['time'];
          $this->summaries['freestyle']->stroke_count += $length['stroke_count'];
          break;

        case STROKE_BACKSTROKE:
          $this->summaries['backstroke']->nbr_lengths++;
          $this->summaries['backstroke']->time += $length['time'];
          $this->summaries['backstroke']->stroke_count += $length['stroke_count'];
          break;

        case STROKE_BREASTSTROKE:
          $this->summaries['breaststroke']->nbr_lengths++;
          $this->summaries['breaststroke']->time += $length['time'];
          $this->summaries['breaststroke']->stroke_count += $length['stroke_count'];
          break;

        case STROKE_BUTTERFLY:
          $this->summaries['butterfly']->nbr_lengths++;
          $this->summaries['butterfly']->time += $length['time'];
          $this->summaries['butterfly']->stroke_count += $length['stroke_count'];
          break;

        case STROKE_UNKNOWN:
          $this->summaries['unknown']->nbr_lengths++;
          $this->summaries['unknown']->time += $length['time'];
          $this->summaries['unknown']->stroke_count += $length['stroke_count'];
          break;

        case STROKE_DRILL:
          $this->summaries['drill']->nbr_lengths++;
          $this->summaries['drill']->time += $length['time'];
          break;

        default:
        }
      } else if ($length['type'] == LENGTH_IDLE) {
        $this->summaries['rest']->time += $length['time'];
      }
    }
  }

  private function GetRecord($nbr_lengths, $stroke, &$record_time, &$record_stroke_count, &$record_first_length)
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
    $record_stroke_count = NULL;
    $record_first_length = NULL;

    while ($last_ptr < count($this->lengths)) {
      $first_length = $this->lengths[$first_ptr];
      $last_length = $this->lengths[$last_ptr];

      switch ($state) {
      case self::SEARCHING:

        if ($first_length['stroke'] != $stroke) {
          $first_ptr++;
          $last_ptr++;

        } else {
          $stroke_count = $first_length['stroke_count'];
          $time = $first_length['time'];
          $last_ptr++;

          if ($nbr_lengths == 1) {
            if ($time < $record_time) {
              $record_time = $time;
              $record_stroke_count = $stroke_count;
              $record_first_length = $first_length['length_index'];
            }
            $state = self::SCANNING;

          } else {
            $state = self::FOUND;
          }
        }
        break;

      case self::FOUND:
        if ($last_length['stroke'] == $stroke && (($last_ptr - $first_ptr) < ($nbr_lengths - 1))) {
          $stroke_count += $last_length['stroke_count'];
          $time += $last_length['time'];
          $last_ptr++;

        } else if ($last_length['stroke'] == $stroke && (($last_ptr - $first_ptr) == ($nbr_lengths - 1))) {
          $stroke_count += $last_length['stroke_count'];
          $time += $last_length['time'];

          if ($time < $record_time) {
            $record_time = $time;
            $record_stroke_count = $stroke_count;
            $record_first_length = $first_length['length_index'];
          }
          $last_ptr++;
          $state = self::SCANNING;

        } else {
          $stroke_count = 0;
          $time = 0.0;
          $last_ptr++;
          $first_ptr = $last_ptr;
          $state = self::SEARCHING;
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
            $record_first_length = $this->lengths[$first_ptr + 1]['length_index'];
          }

          $first_ptr++;
          $last_ptr++;
        } else {
          $stroke_count = 0;
          $time = 0.0;
          $last_ptr++;
          $first_ptr = $last_ptr;
          $state = self::SEARCHING;
        }
        break;
      }
    }
    if ($record_time == $one_day) $record_time = NULL;
  }

  private function saveToDatabase() {

    $dbh = DB::getConnection();
    $sql = 'INSERT INTO activities (activity_id, product_id, software_version, serial_number, 
      date, pool_size_meters, pool_size_units) VALUES (?,?,?,?,?,?,?)';
    $sth = $dbh->prepare($sql);
    $sth->execute([$this->id, $this->product_id, $this->software_version, $this->serial_number, 
      $this->date->format('Y-m-d H:i:s'), $this->pool_size_meters, $this->pool_size_units]);

  }
  
}


