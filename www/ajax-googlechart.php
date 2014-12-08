<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();

try {

  session_start();
  $file_id = NULL;

  ob_start();
  header('Content-type: text/javascript');
  header('Cache-Control: no-cahe, no-store, must-revalidate');
  header('Pragma: no-cache');
  header('Expires: 0');

  $req_id = '' ;

  if (isset($_GET['tqx'])) {
    if (preg_match('/reqId\s?:\s?\d+/', $_GET['tqx'], $matches) == 1)
      $req_id = $matches[0].',';
  }

  if (!isset($_SESSION['file_id'])) {
    echo "google.visualization.Query.setResponse({ $req_id status:'error',"
      ."errors:[{reason:'other',message:'Session timed out'}]})";
    ob_end_flush();
    exit();
  }
  $file_id = $_SESSION['file_id'];
  $path = swt\DB::convertFileIdToPath($file_id);

  $swim_file = new swt\SwimFile($path.'EDIT');

  if (isset($_GET['action'])) {
    $action = $_GET['action'];
    $length_index = isset($_GET['lengthIndex']) ?  $_GET['lengthIndex'] : NULL;
    $change_stroke_from = isset($_GET['changeStrokeFrom']) ? $_GET['changeStrokeFrom'] : NULL;
    $change_stroke_to = isset($_GET['changeStrokeTo']) ?  $_GET['changeStrokeTo'] : NULL;
    $change_stroke_option = isset($_GET['changeStrokeOption']) ?  $_GET['changeStrokeOption'] : NULL;
    $new_pool_size = isset($_GET['newPoolSize']) ?  $_GET['newPoolSize'] : NULL;
    $new_pool_size_units = isset($_GET['newPoolSizeUnits']) ?  $_GET['newPoolSizeUnits'] : NULL;

    switch ($action) {

    case swt\dB::EDITOR_ACTION_UNDO_ALL:
      if (!copy($path.'UPLOAD', $path.'EDIT'))
        throw new Exception('Could not copy file to editing directory');

      $swim_file = new swt\SwimFile($path.'EDIT');
      break;
    case swt\dB::EDITOR_ACTION_MERGE:
      $swim_file->merge($length_index);
      $status = 'Merge completed';
      break;
    case swt\dB::EDITOR_ACTION_SPLIT;
      $swim_file->split($length_index);
      $status = 'Split Completed';
      break;
    case swt\dB::EDITOR_ACTION_CHANGE_STROKE:
      $swim_file->ChangeStroke($length_index, $change_stroke_to, $change_stroke_option);
      $status = 'Change stroke completed';
      break;
    case swt\dB::EDITOR_ACTION_CHANGE_POOL_SIZE:
      if ($new_pool_size_units == swt\UNITS_STATUTE) {
        $swim_file->changePoolSize($new_pool_size/1.09361, swt\UNITS_STATUTE);
        $status = 'Pool size changed to '.$new_pool_size.'y';
      } else {
        $swim_file->changePoolSize($new_pool_size, swt\UNITS_METRIC);
        $status = 'Pool size changed to '.$new_pool_size.'m';
      }
      break;
    case swt\dB::EDITOR_ACTION_DELETE:
      $swim_file->delete($length_index);
      $status = 'Delete Completed';
      break;
    default:

    }
  }

  echo <<<HEREDOC
google.visualization.Query.setResponse({ $req_id status:'ok',table:
{cols:[
{id:'Length',label:'Length',type:'number'},
{type: 'string', role: 'tooltip'},
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
    ++$num_active_lengths;
    $moving_time += $length['time'];
    $stroke_count += $length['stroke_count'];

    if ($length['stroke'] != swt\STROKE_DRILL)
      ++$num_active_lengths_without_drills;
  }
  $avg_time_per_length = $moving_time / $num_active_lengths;

  $length_counter = 0;
  $start_new_lap = true;
  $current_lap = 1;
  $resting = false;
  $duplicates = array();

  foreach($lengths as $index => $length) {
    if ($length['lap'] != $current_lap) {
      $start_new_lap = true;
      ++$current_lap;
    }

    ++$length_counter;

    $annotation = '';
    if ($start_new_lap) $annotation = $length['lap'];
    elseif ($resting) $annotation = 'R';

    $color = swt\Functions::$stroke_lookup[$length['stroke']]['color'];
    $can_merge = $length['can_merge'] ? 'true' : 'false';
    $can_edit = $length['can_edit'] ? 'true' : 'false';
    $is_duplicate = 'false';
    if ($length['is_duplicate'] == true) {
      $is_duplicate = 'true';
      $duplicates[] = $length_counter;
    }

    $stroke = swt\Functions::$stroke_lookup[$length['stroke']]['string'];
    $tooltip = sprintf('Length: %d\n'.
       '%s\n'.
      '%.1f sec.\n'.
      '%d strokes',
      $length_counter, $stroke, $length['time'], $length['stroke_count']);

    if (!($length_counter == 1))
      echo ',';

    printf("{c:[{v:%d},{v:'%s'},{v:%.1f},{v:'%s'},{v:%.1f},{v:'%s'}],".
      "p:{lengthIndex:%d,swimStroke:%s,canMerge:%s,canEdit:%s,isDuplicate:%s}}\n",
      $length_counter, $tooltip, $avg_time_per_length, $annotation, $length['time'],
      $color, $length['length_index'], $length['stroke'], $can_merge, $can_edit, $is_duplicate);

    $resting = $length['rest'] > 0 ? true : false;
    $start_new_lap = false;
  }

  $pool_size_units = 'm';
  $pool_size = $swim_file->getPoolSize();
  $distance = $pool_size * $num_active_lengths;

  if ($swim_file->getPoolSizeUnits() == swt\UNITS_STATUTE) {
    $pool_size_units = 'y';
    $pool_size *= 1.09361;
    $distance *= 1.09361;
  }

  $time  = new DateTime();
  $time->setTimezone(new DateTimeZone('UTC'));
  $time->setTimestamp($moving_time);
  if ($time->format('H') > 0)
    $time = $time->format('G:i:s');
  else
    $time = intval($time->format('i')).':'.$time->format('s');

  $pace = new DateTime();
  $pace->setTimezone(new DateTimeZone('UTC'));
  $pace->setTimestamp(100 * $avg_time_per_length / $pool_size);
  $pace = intval($pace->format('i')).':'.$pace->format('s');

  $avg_strokes_per_length = ($stroke_count / $num_active_lengths_without_drills);

  if (!isset($status)) $status = 'Loading chart completed';
  if (count($duplicates) > 0) {
    $status = $status.'. <b>Duplicate detected('
      .implode(',', $duplicates).'), suggest you delete</b>';
  }

  printf("],p:{status:'%s',poolSize:%.0f,poolSizeUnits:'%s',distance:'%.0f %3\$s',"
    ."swimTime:'%s',avgPace:'%s min/100%3\$s',avgStrokes:'%.0f/length'}}})",
    $status, $pool_size, $pool_size_units, $distance, $time, $pace, $avg_strokes_per_length);

  $swim_file->save($path.'EDIT');
  if (isset($action)) {
    swt\DB::addEditorLogEntry($file_id, $action, $length_index,
      $change_stroke_from, $change_stroke_to, $change_stroke_option,
      $new_pool_size, $new_pool_size_units);
  }
  ob_end_flush();

} catch (Exception $ex) {

  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  ob_clean();
  echo "google.visualization.Query.setResponse({ $req_id status:'error',"
    ."errors:[{reason:'other',message:''}]})";
  ob_end_flush();
}
?>
