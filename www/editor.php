<?php
spl_autoload_register();
\swt\Functions::registerErrorHandler();
session_start();

if (!isset($_SESSION['internal_filename'])) {
  header('location: upload');
  exit;
}

try {
  $swim_file = new \swt\Swimfile(realpath(swt\Functions::EDIT_DIR.$_SESSION['internal_filename']));
  $time_created = new DateTime('31DEC89');
  $time_created->setTimestamp($time_created->getTimestamp() + $swim_file->getDateCreated());
  $time_created = '<script type="text/javascript">var date = new Date(Date.UTC('
    .$time_created->format('Y').','
    .($time_created->format('n') - 1).','
    .$time_created->format('j').','
    .$time_created->format('G').','
    .$time_created->format('i').','
    .$time_created->format('s').'));'
    .'document.write(date.toLocaleString());</script> ';
} catch (Exception $ex) {
  $time_created = '';
  swt\Functions::errorLog($ex);
  echo swt\Functions::EDIT_DIR.$_SESSION['internal_filename'];
}
\swt\Layout::header('Swimming Watch Data Editor', \swt\Layout::TAB_EDITOR);
?>
<div class="section">
  <h2>
    Editing activity from <?=$time_created?> - <a href="upload">Upload new file</a> 
  </h2>
  <ul>
    <li>Select a length by clicking on it, and click a button in the toolbar,</li>
    <li>To merge, select the first of the two lengths to be merged, </li>
    <li>Last length of an interval cannot be merged,</li>
    <li>Drill lengths cannot be edited,</li>
    <li>Delete will convert the length to rest time unless length is a 
    <a href="help#duplicate" target="_blank">duplicate</a></li>
    <li>After you're done, download the new activity file, and manually 
    upload it to your 
    <a href="http://connect.garmin.com" target="_blank">Garmin Connect</a> account. 
    check the <a href="help#manualUpload" target="_blank">User's Guide</a>
    for how to proceed.</li>
  </ul>
  <div class="toolbar">
    <button id="undoAllBtn">Undo All</button>
    <button id="changePoolLengthBtn">Change Pool Length</button>
    <button id="deleteBtn" disabled="disabled">Delete</button>
    <button id="mergeBtn" disabled="disabled">Merge</button>
    <button id="splitBtn" disabled="disabled">Split</button>
    <button id="changeStrokeBtn" disabled="disabled">Change Stroke</button>
    <button id="downloadBtn" style="float: right" onclick="window.location='download'">
      Download</button>
  </div>
  <div class="status-label-container">
    <span id="statusLbl">Loading chart...</span></div>
  <div id="chartContainer">
    <div id="chart">
    </div>
  </div>
  <ul class="statusbar">
    <li>Distance: <span id="distanceLbl"></span></li>
    <li>Swim Time: <span id="swimTimeLbl"></span></li>
    <li>Avg Pace: <span id="avgPaceLbl"></span></li>
    <li>Avg Strokes: <span id="avgStrokesLbl"></span></li>
  </ul>
</div>
<div id="changeStrokeDlg" class="dialog">
  <div class="dialog-overlay">
    &nbsp;</div>
  <form name="changeStrokeForm" class="dialog-content" action="">
    <a id="changeStrokeDlgCancelBtn" class="dialog-cancel-button">
      <img src="/content/modal_close.png" alt="Close dialog">
    </a>
    <p>
    Length <span id="selectedLengthLbl"></span>: <span id="selectedLengthStrokeLbl"></span>
    </p>
    <p>
    Change to:
    <select name="toSwimStroke">
      <option value="<?=swt\STROKE_BACKSTROKE ?>">Backstroke</option>
      <option value="<?=swt\STROKE_BREASTSTROKE?>">Breaststroke</option>
      <option value="<?=swt\STROKE_BUTTERFLY?>">Butterfly</option>
      <option value="<?=swt\STROKE_FREESTYLE?>">Freestyle</option>
    </select>
    </p>
    <p>
    Apply to:
    </p>
    <input name="strokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_LENGTH_ONLY?>" 
    checked="checked">Length Only<br>
    <input name="strokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_LAP?>">
    All Lengths in interval<br>
    <input name="strokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_ALL?>">
    All lengths<br>
    <br>
    <input type="hidden" name="fromSwimStroke" value="">
    <button type="submit">Change Stroke</button>
  </form>
</div>
<div id="changePoolLengthDlg" class="dialog">
  <div class="dialog-overlay">
    &nbsp;
  </div>
  <form name="changePoolLengthForm" class="dialog-content" action="">
    <a id="changePoolLengthDlgCancelBtn" class="dialog-cancel-button">
      <img src="/content/modal_close.png" alt="Close dialog">
    </a>
    <p>
    Change Pool Length
    </p>
    <p id="changePoolLengthDlgErrorLbl" class="warning" style="display: none">
    Value must be within 17-150 meters or 18-150 yards
    </p>
    <input type="text" name="newPoolLength" value="">
    <br><br>
    <input id="metricRdo" name="poolLengthUnits" type="radio" 
    value="<?=swt\UNITS_METRIC?>" checked="checked">Meters<br>
    <input id="statuteRdo" name="poolLengthUnits" type="radio" 
    value="<?=swt\UNITS_STATUTE?>">Yards<br>
    <br>
    <button type="submit">Change Pool Length</button>
  </form>
</div>
<script type="text/javascript" src="http://www.google.com/jsapi"></script>
<script type="text/javascript" src="/scripts/editor2.js"></script>
<?php
swt\layout::analytics();
swt\Layout::footer();


