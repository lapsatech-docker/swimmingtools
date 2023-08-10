<?php
spl_autoload_register();
\swt\Functions::registerErrorHandler();

function summaryTable()
{
  global $activity;

  $table =  <<<'NOWDOC'
<table>
  <thead>
    <tr>
      <th>Style </th>
      <th>Lengths </th>
      <th>Distance </th>
      <th>Time </th>
      <th>Pace </th>
      <th>SPM </th>
      <th>SPL </th>
    </thead>
    <tbody>

NOWDOC;

  $num_sums = 0;
  $sum = $activity->summaries[swt\Activity::SUM_FREE];
  $template = '<tr class="%s"><td>%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%.1f</td><td>%.1f</td></tr>'.PHP_EOL;
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'freestyle', 'Freestyle', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $sum = $activity->summaries[swt\Activity::SUM_BACK];
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'backstroke', 'Backstroke', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $sum = $activity->summaries[swt\Activity::SUM_BREAST];
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'breaststroke', 'BreastStroke', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $sum = $activity->summaries[swt\Activity::SUM_FLY];
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'butterfly', 'Butterfly', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $sum = $activity->summaries[swt\Activity::SUM_MIXED];
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'mixed', 'Mixed', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $sum = $activity->summaries[swt\Activity::SUM_UNK];
  if ($sum['num_lengths'] > 0) {
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'unknown', 'Unknown', $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    $num_sums++;
  }
  $rest_time = $activity->summaries[swt\Activity::SUM_REST]['time'];
  $drill_time = $activity->summaries[swt\Activity::SUM_DRILL]['time'];
  $sum = $activity->summaries[swt\Activity::SUM_SUB];
  if ($num_sums > 1) {
    if ($rest_time == 0.0 && $drill_time == 0.0) 
      $label = 'Total';
    else 
      $label = 'Sub Total';
    
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, 'total', $label, $sum['num_lengths'], $fields['distance'], 
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
  }
  $sum = $activity->summaries[swt\Activity::SUM_DRILL];
  if ($drill_time > 0) {
    $template = '<tr class="drill"><td>Drill</td><td>%d</td><td>%s</td><td>%s</td><td></td><td></td><td></td></tr>'.PHP_EOL;
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, $sum['num_lengths'], $fields['distance'], $fields['time']);
  }
  $sum = $activity->summaries[swt\Activity::SUM_REST];
  if ($rest_time > 0) {
    $template = '<tr class="rest"><td>Rest</td><td></td><td></td><td>%s</td><td></td><td></td><td></td></tr>'.PHP_EOL;
    $table .= sprintf($template, swt\Functions::formatTime($rest_time, TRUE));
  }
  $sum = $activity->summaries[swt\Activity::SUM_TOTAL];
  if ($rest_time > 0 || $drill_time > 0) {
    $template = '<tr class="total"><td>Total</td><td>%d</td><td>%s</td><td>%s</td><td></td><td></td><td></td></tr>'.PHP_EOL;
    $fields = swt\Functions::getComputedFields($sum['num_lengths'], $sum['time'], $sum['stroke_count']);
    $table .= sprintf($template, $sum['num_lengths'], $fields['distance'],$fields['time']);
  }
  $table .= '</tbody></table>';
  return $table;
}

function bestTimesTable()
{
  global $activity;
  $table =  <<<'NOWDOC'
  <table id="best">
    <thead>
    <tr>
      <th colspan=2>Best </th>
      <th>Time </th>
      <th>Pace </th>
      <th>SPM </th>
      <th>SPL </th>
      <th>lengths</th>
    </thead>
    <tbody>

NOWDOC;

  $template = <<<'NOWDOC'
<tr class="%s">
  <td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%.1f</td><td>%.1f</td><td>%d-%d</td>
</tr>

NOWDOC;

  $num_bests = 0;
  foreach($activity->bests as $index => $best) {
    if($best !== NULL) {
      $stroke =  swt\Activity::$BEST_PARAMS[$index]['stroke'];
      $fields = swt\Functions::getComputedFields($best['num_lengths'], 
        $best['time'], $best['stroke_count'], $stroke);
      $table .= sprintf($template,
        strtolower($fields['stroke']), $fields['distance'], $fields['stroke'], 
        $fields['time'], $fields['pace_str'], $fields['spm'],$fields['spl'], 
        $best['first_length'] + 1, $best['first_length'] + $best['num_lengths']);
      $num_bests++;
    }
  }
  $table .= '</tbody></table>'.PHP_EOL;
  if ($num_bests == 0) 
    return '<p>Best times are only available for standard pool sizes (25m, 50m, 25y)</p>';
  else
    return $table;
}

function lengthsTable() 
{
  global $activity;
  $table =  <<<'NOWDOC'
  <table id="lengths">
    <thead>
    <tr>
      <th>#</th>
      <th colspan=2>Distance</th>
      <th>Time</th>
      <th>Pace</th>
      <th>SPM</th>
      <th>SPL</th>
  </tr>
    </thead>
    <tbody>
NOWDOC;

  $current_lap = 1;
  $lap_num_lengths = 0;
  $lap_moving_time = 0.0;
  $lap_stroke_count = 0;
  $lap_stroke = swt\STROKE_UNKNOWN;
  $num_lengths = count($activity->lengths);
  foreach($activity->lengths as $index => $length) {
    $lap_num_lengths++;
    $lap_moving_time += round($length['time'],1);
    $lap_stroke_count += $length['stroke_count'];
    if ($lap_stroke == swt\STROKE_UNKNOWN) {
      $lap_stroke = $length['stroke'];
    } else {
      if ($lap_stroke != $length['stroke'])
        $lap_stroke = swt\STROKE_MIXED;
    }

    $fields = swt\Functions::getComputedFields(1, $length['time'], $length['stroke_count'], $length['stroke']);
    $table .= sprintf('<tr class="length %s"><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%.1f</td><td>%d</td></tr>'.PHP_EOL,
      strtolower($fields['stroke']), $index+1, $fields['distance'], $fields['stroke'],
      $fields['time'], $fields['pace_str'], $fields['spm'], $fields['spl']);
    if ($length['rest'] > 0)  {
      $rest = swt\Functions::formatTime($length['rest'], TRUE);
      $table .= '<tr class="rest"><td>Rest</td><td></td><td></td><td>'.$rest.'</td><td></td><td></td><td></td></tr>'.PHP_EOL;
    }
    $print_interval = FALSE;
    if ($index < $num_lengths - 1) {
      if ($length['lap'] != $activity->lengths[$index + 1]['lap']) 
        $print_interval = TRUE;
    } else if ($index == $num_lengths -1) {
      $print_interval = TRUE;
    }
    if ($print_interval) {
      $fields = swt\Functions::getComputedFields($lap_num_lengths, $lap_moving_time, $lap_stroke_count, $lap_stroke);
      $table .= sprintf('<tr class="interval"><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%.1f</td><td>%.1f</td></tr>'.PHP_EOL,
        swt\Functions::getOrdinalNumber($current_lap).' Interval', $fields['distance'], $fields['stroke'], $fields['time'], 
        $fields['pace_str'], $fields['spm'], $fields['spl']);
      $table .='</tbody><tbody>';
      $lap_num_lengths = 0;
      $lap_moving_time = 0.0;
      $lap_rest_time = 0.0;
      $lap_stroke_count = 0;
      $lap_stroke = swt\STROKE_UNKNOWN;
      $current_lap++;
    }
  }
  $table .= '</tbody></table>';
  return $table;
}

function googleChartLengthsData ()
{
  global $activity;
  $data = <<<'NOWDOC'
{cols:[
{id:'length',label:'Length',type:'string'},
{id:'interval',type:'string',role:'annotation'},
{id:'time', label: 'Time', type:'number'},
{id:'pace',label:'Pace',type:'number'},
{id:'spm', label: 'SPM', type:'number'},
{id:'spl', label: 'SPL', type:'number'},
{id:'stroke',type:'number'},
],
rows:[

NOWDOC;

  $current_interval = 0;
  foreach($activity->lengths as $index => $length) {
    $interval ='null';
    if ($current_interval != $length['lap']) {
      $current_interval = $length['lap'];
      $interval = $current_interval;
    }

    $fields = swt\Functions::getComputedFields(1, $length['time'], $length['stroke_count'], $length['stroke']);

    $data .= sprintf('{c:[{v:%s},{v:%s},{v:%.1f},{v:%.1f},{v:%.1f},{v:%.0f},{v:%d}]}', 
      $index + 1, $interval, $length['time'], $fields['pace'], $fields['spm'], $fields['spl'], $length['stroke']); 
    
    if ($length['rest'] > 0 ) {
      $data .= sprintf( ','.PHP_EOL.'{c:[{v:\'%s\'},{v:null},{v:%.1f},{v:null},{v:null},{v:null},{v:null},{v:null}]}',
        'R', $length['rest']);
    }
    if ($index < count($activity->lengths)-1)
      $data .= ','.PHP_EOL;
  }
  $data .= ']}';
  return $data;
}

function googleChartSummaryData() 
{
  global $activity;

  $data = <<<'NOWDOC'
{cols:[
{id:'stroke',type:'string'},
{id:'length',label:'time',type:'number'}
],
rows:[

NOWDOC;

  $time = $activity->summaries[swt\Activity::SUM_FREE]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Freestyle\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_BACK]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Backstroke\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_BREAST]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Breaststroke\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_FLY]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Butterfly\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;
  
  $time = $activity->summaries[swt\Activity::SUM_MIXED]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Mixed\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_UNK]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Unknown\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_DRILL]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Drill\'}, {v:'.$time.', f:\''.$time_str.'\'}]},'.PHP_EOL;

  $time = $activity->summaries[swt\Activity::SUM_REST]['time'];
  $time_str = swt\Functions::formatTime($time);
  $data .= '{c:[{v:\'Rest\'}, {v:'.$time.', f:\''.$time_str.'\'}]}'.PHP_EOL;

  $data .= ']}';
  return $data;
}

session_start();

$file_id = NULL;
$error = FALSE;

try {
  if (isset($_GET['id'])) {
    $file_id = $_GET['id'];
    $activity =  new swt\Activity($file_id);
  } else if (isset($_SESSION['file_id'])) {
    $file_id = $_SESSION['file_id'];
    $path = swt\DB::convertFileIdToPath($file_id);

    if (!(copy($path.'EDIT', $path.'DOWNLOAD'))) 
      throw new Exception('Cannot update file');

    $activity = new swt\Activity($file_id);
    $activity->saveToDatabase();
  } else {
    header('location: upload');
    exit;
  }

  swt\Functions::$pool_size = $activity->pool_size;
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  $error = TRUE;
}

\swt\Layout::header('Swimming Watch Tools - Viewer', 
\swt\Layout::TAB_VIEWER,
"Analyze your Garmin Swim Data");
if (!isset($activity)) {
  echo '<p class="warning">Unexpected error has been log, try again</p>';
} else {
?>
<div class="section">
  <h2>
    Summary
  </h2>
<table>
<caption>
<?php

try {
  $pool_size = $activity->pool_size;
  $pool_size_units = $activity->pool_size_units == swt\UNITS_STATUTE ? 'y' : 'm';

  $starts_at = $activity->date;
  $starts_at = '<script type="text/javascript">'.PHP_EOL
    .'var date = new Date(Date.UTC('
    .$starts_at->format('Y').','
    .($starts_at->format('n') - 1).','
    .$starts_at->format('j').','
    .$starts_at->format('G').','
    .$starts_at->format('i').','
    .$starts_at->format('s').'));'.PHP_EOL
    .'document.write(date.toLocaleDateString() + \' \');'.PHP_EOL
    .'document.write(\'From: \' + date.toLocaleTimeString()+ \' \');'.PHP_EOL
    .'date.setTime(date.getTime() + '.($activity->summaries[swt\Activity::SUM_TOTAL]['time'] * 1000).');'
    .'document.write(\'To: \' + date.toLocaleTimeString());'.PHP_EOL
    .'</script>';
  echo $starts_at;
  echo ' - Pool size: '.$pool_size.' '.$pool_size_units.'<br/>';
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
}
?>
</caption>
<tr><td>
<?php
try {
  echo summaryTable();
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  echo '<p class="warning">Unexpected error has been logged</p>';
}
?>
</td><td>
  <div id="summaryChart">
  </div>
</td></tr></table>
</div>
<div class="section">
  <h2>
    Best times 
  </h2>
<?php
try {
  echo bestTimesTable();
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  echo '<p class="warning">Unexpected error has been logged.</p>';
}
?>
</div>
<div class="section">
  <h2>
   Pace Analysis 
  </h2>
    <p><b>How to interpret data:</b> Speed has two components: stroke rate, 
and stroke length. Stroke rate is the number of strokes per unit of time, 
and stroke length is the distance covered with each strokes. Swimming fast 
is achieved by moving your arms fast, while covering the largest distance 
with each strokes. In the graph below, each bar is a length, bar height 
represents pace, red line is the stroke rate, and blue line is the stroke 
count. Stroke count is used instead of Stroke length because it is more 
intuitive and convenient. Both metrics are related. A large stroke length 
means you'll need less strokes to get to the other end of the pool. Looking 
at data, you should observe that fastest lengths are those with high stroke 
rate and/or low stroke count values.</p>
    <div id="paceChart">
    </div>
    <div id="paceChartSlider"></div>
<p class="centered">Use sliders to zoom in data<br/>
Click and drag area between sliders to scroll</p>
</div>
<div class="section">
  <h2>
    Stroke Analysis
  </h2>
    <p><b>How to interpret data:</b> This graph shows you stroke rate and
stroke count distribution, and how it affects pace. Each point is a length.
X-value is stroke rate, Y-value is stroke count, and color represents  pace
(yellow is slow, red is fast). As said earlier, fastest lengths are those with
high stroke rate and/or low stroke count values. Unfortunately, for most
of us, raising stroke rate has the side effect of raising stroke count.
The positive effect of raising stroke rate may be cancelled by the negative
effect of higher stroke count values and results in no increase or decrease of
pace. Best swimmers have constant stroke count, and are able to maintain this
stroke count at high stroke rate.</p>
  <p><select id="strokeChartSelect">
<?php 
if (isset($activity)) {
  if ($activity->summaries[swt\Activity::SUM_FREE]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_FREESTYLE.'">Freestyle</option>';
  if ($activity->summaries[swt\Activity::SUM_BACK]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_BACKSTROKE.'">Backstroke</option>';
  if ($activity->summaries[swt\Activity::SUM_BREAST]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_BREASTSTROKE.'">Breaststroke</option>';
  if ($activity->summaries[swt\Activity::SUM_FLY]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_BUTTERFLY.'">Butterfly</option>';
  if ($activity->summaries[swt\Activity::SUM_MIXED]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_MIXED.'">Mixed</option>';
  if ($activity->summaries[swt\Activity::SUM_UNK]['num_lengths'] > 0)
    echo '<option value="'.swt\STROKE_UNKNOWN.'">Unknown</option>';
}
?>
  </select>
  &nbspAvg stroke rate: <span id="avgSpm"></span> / Avg Stroke count: <span id="avgSpl"></span>
  </p>
  <div id='strokeChart'></div>
</div>
<div class="section">
  <h2>
    Lengths Details
  </h2>
<?php
try {
   echo lengthsTable();
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  echo '<p class="warning">Unexpected error has been logged.</p>';
}
?>
</div>
<script type="text/javascript" src="http://code.jquery.com/jquery-1.11.0.min.js"></script>
<script type="text/javascript" src="http://code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
<script type="text/javascript" src="scripts/jquery.nouislider.min.js"></script>
<script type="text/javascript" src="http://www.google.com/jsapi"></script>
<script type="text/javascript">
var lengthsData_ = 
<?php
try {
  echo googleChartLengthsData().';'.PHP_EOL;
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  echo '{};';
}
?>
var summaryChartData_ =
<?php
try {
   echo googleChartSummaryData().';'.PHP_EOL;
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
  echo '{};'.PHP_EOL;
}
?>

</script>
<script type="text/javascript" src="scripts/viewer2.js"></script>
<?php
} // end if isset($activity)
swt\Layout::footer();
