<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try {

  session_start();
  ob_start();
  header('Content-type: text/javascript');
  header('Cache-Control: no-cahe, no-store, must-revalidate');
  header('Pragma: no-cache');
  header('Expires: 0');

  $req_id = '' ;
 
  $upload_dir = realpath(swt\Functions::UPLOAD_DIR).DIRECTORY_SEPARATOR;
  $edit_dir = realpath(swt\Functions::EDIT_DIR).DIRECTORY_SEPARATOR;
  
  if (isset($_GET['tqx'])) {
    if (preg_match('/reqId\s?:\s?\d+/', $_GET['tqx'], $matches) == 1) 
      $req_id = $matches[0].',';
  }

  if (!isset($_SESSION['internal_filename'])) {
    echo "google.visualization.Query.setResponse({ $req_id status:'error',"
      ."errors:[{reason:'other',message:'Session timed out'}]})";
    ob_end_flush();
    exit();
  }
  $internal_filename = $_SESSION['internal_filename'];
  
  $swim_file = new swt\SwimFile($edit_dir.$internal_filename);
  
  if (isset($_GET['action'])) {

    switch ($_GET['action']) {

    case 'undoAll':
      if (!copy($upload_dir.$internal_filename, $edit_dir.$internal_filename)) 
        throw new Exception('Could not copy file to editing directory');

      $swim_file = new swt\SwimFile($edit_dir.$internal_filename);    
      break;
    case 'merge':
      $swim_file->merge($_GET['messageIndex']);
      $status = 'Merge completed';
      break;
    case 'split':
      $swim_file->split($_GET['messageIndex']);
      $status = 'Split Completed';
      break;
    case 'changeStroke':
      $swim_file->ChangeStroke($_GET['messageIndex'], $_GET['toSwimStroke'], 
        $_GET['strokeOption']);
      $status = 'Change stroke completed';
      break;
    case 'changePoolLength':
      $newPoolLength = $_GET['newPoolLength'];
      if ($_GET['poolLengthUnits'] == swt\UNITS_STATUTE) {
        $swim_file->changePoolLength($newPoolLength/1.09361, swt\UNITS_STATUTE);
        $status = 'Pool length changed to '.$newPoolLength.'y';
      } else { 
        $swim_file->changePoolLength($newPoolLength, swt\UNITS_METRIC);
        $status = 'Pool length changed to '.$newPoolLength.'m'; 
      }
      break;
    case 'delete':
      $swim_file->delete($_GET['messageIndex']);
      $status = 'Delete Completed';
      break;
    default:

    }
  }

  echo <<<HEREDOC
google.visualization.Query.setResponse({ $req_id status:'ok',table:
{cols:[
{id:'Length',label:'Length',type:'number'},
{type:'string',role:'tooltip','p':{'html':true}},
{id:'Average',label:'Average',type:'number'},
{type:'string',role:'annotation'},
{id:'Time',label:'Time',type:'number'},
{type:'string',role:'style'}
],
rows:[
HEREDOC;

  $lengths = $swim_file->getLengths();

  $num_active_lengths = 0;
  $num_active_lengths_without_drills = 0;
  $moving_time = 0;
  $stroke_count = 0;
  $duplicates = array();

  foreach($lengths as $index => $length) {
    if ($length['type'] == swt\LENGTH_ACTIVE) {
      ++$num_active_lengths;
      $moving_time += $length['time'];
      $stroke_count += $length['stroke_count'];

      if ($length['stroke'] != swt\STROKE_DRILL)
        ++$num_active_lengths_without_drills;
    }
  }
  $avg_time_per_length = $moving_time / $num_active_lengths;

  //$lap_counter = 1;
  $length_counter = 0;
  $start_new_lap = true;
  $resting = false;
  $current_lap = 1;
  //$num_active_lengths_in_lap = 0;
  $duplicates = array();

  foreach($lengths as $index => $length) {
     if ($length['lap'] != $current_lap) {
    //  if ($num_active_lengths_in_lap > 0) {
    //    ++$lap_counter;
        $start_new_lap = true;
    //  }
      ++$current_lap;
    //  $num_active_lengths_in_lap = 0;
    }

    if ($length['type'] == swt\LENGTH_ACTIVE) {
      //++$num_active_lengths_in_lap;
      ++$length_counter;

      $annotation = '';
      if ($start_new_lap) $annotation = $length['lap'];
      elseif ($resting) $annotation = 'R';

      switch ($length['stroke']) {
      case swt\STROKE_FREESTYLE:
        $stroke = 'Freestyle'; $color = '#c43dbf';
        break;
      case swt\STROKE_BACKSTROKE:
        $stroke = 'Backstroke'; $color = '#1f8ef9';
        break;
      case swt\STROKE_BREASTSTROKE:
        $stroke = 'Breaststroke'; $color = '#95de2b';
        break;
      case swt\STROKE_BUTTERFLY:
        $stroke = 'Butterfly'; $color = '#eb3d3d';
        break;
      case swt\STROKE_DRILL:
        $stroke = 'Drill'; $color = '#ff7c05';
        break;
      default:
        $stroke = 'Unknown'; $color = 'gray';
      }

      $can_merge = $length['can_merge'] ? 'true' : 'false';
      $can_edit = $length['can_edit'] ? 'true' : 'false';
      $is_duplicate = 'false';
      if ($length['is_duplicate'] == true) {
        $is_duplicate = 'true';
        $duplicates[] = $length_counter;
      }

      $is_duplicate = $length['is_duplicate'] ? 'true' : 'false';

      $tooltip = sprintf('<div class="tooltip">'.
        '<span class="length">Length: %d</span><br/>'.
        '<span class="%s">%s</span><br/>'.
        '%.1f sec.<br/>'.
        '%d strokes'.
        '</div>',
        $length_counter, strtolower($stroke), $stroke, $length['time'], $length['stroke_count']);

      if (!($length_counter == 1)) 
        echo ',';

      printf("{c:[{v:%d},{v:'%s'},{v:%.1f},{v:'%s'},{v:%.1f},{v:'%s'}],".
        "p:{messageIndex:%d,swimStroke:'%s',canMerge:%s,canEdit:%s,isDuplicate:%s}}\n", 
        $length_counter, $tooltip, $avg_time_per_length, $annotation, $length['time'], 
        $color, $length['message_index'], $stroke, $can_merge, $can_edit, $is_duplicate);

      $start_new_lap = false;
      $resting = false;

    } elseif ($length['type'] == swt\LENGTH_IDLE) {
      $resting = true;
    }
  }

  $pool_length_units = 'm';
  $pool_length = $swim_file->getPoolLength();
  $distance = $pool_length * $num_active_lengths;

  if ($swim_file->getPoolLengthUnits() == swt\UNITS_STATUTE) {
    $pool_length_units = 'y';
    $distance *= 1.09361;
  }

  $time  = new DateTime();
  $time->setTimestamp($moving_time);
  if ($time->format('H') > 0)
    $time = $time->format('G:i:s');
  else
    $time = intval($time->format('i')).':'.$time->format('s');

  $pace = new DateTime();
  $pace->setTimestamp(100 * $avg_time_per_length / $pool_length);
  $pace = intval($pace->format('i')).':'.$pace->format('s');

  $avg_strokes_per_length = ($stroke_count / $num_active_lengths_without_drills);

  if (!isset($status)) $status = 'Loading chart completed';
  if (count($duplicates) > 0) {
    $status = $status.'. <b>Duplicate detected('
      .implode(',', $duplicates).'), suggest you delete</b>';
  }

  if ($pool_length_units == 'y') 
    $pool_length *= 1.09361;

  printf("],p:{status:'%s',poolLength:%.0f,poolLengthUnits:'%s',distance:'%.0f %3\$s',"
    ."swimTime:'%s',avgPace:'%s min/100%3\$s',avgStrokes:'%.0f/length'}}})",
    $status, $pool_length, $pool_length_units, $distance, $time, $pace, $avg_strokes_per_length);

  $swim_file->save($edit_dir.$internal_filename);
  ob_end_flush();

} catch (Exception $ex) {

  swt\Functions::errorLog($ex);
  ob_clean();
  echo "google.visualization.Query.setResponse({ $req_id status:'error',"
    ."errors:[{reason:'other',message:''}]})";
  ob_end_flush();
}
?>
