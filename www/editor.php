<?php
spl_autoload_register();
swt\Functions::registerErrorHandler();
session_start();

if (!isset($_SESSION['file_id'])) {
  header('location: upload');
  exit;
}
$file_id = $_SESSION['file_id'];
$product = 1; // Garmin Swim

try {
  $swim_file = new \swt\Swimfile(swt\DB::convertFileIdToPath($file_id).'EDIT');
  $time_created = new DateTime();
  $time_created->setTimezone(new DateTimeZone('UTC'));
  $time_created->setTimestamp($swim_file->getDateCreated());
  $time_created = '<script type="text/javascript">var date = new Date(Date.UTC('
    .$time_created->format('Y').','
    .($time_created->format('n') - 1).','
    .$time_created->format('j').','
    .$time_created->format('G').','
    .$time_created->format('i').','
    .$time_created->format('s').'));'
    .'document.write(date.toLocaleString());</script> ';
  $product = $swim_file->getProduct();
} catch (Exception $ex) {
  $time_created = '';
  swt\DB::addErrorLogEntry($file_id, $ex->getFile(), $ex);
}
swt\Layout::header('Swimming Watch Tools - Editor', swt\Layout::TAB_EDITOR);
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
    <li>Delete will convert the length to rest time</li>
<?php if ($product == 5) { ?>
    <li>After you're done, download the new activity file. Tomtom users
    can download the file in the Tomtom FIT format or Garmin FIT format. 
    Tomtom FIT format is not supported by some 3rd party web site. In those cases, 
    try Garmin FIT format.</li>
<?php } else { ?>
    <li>After you're done, download the new activity file, and manually 
    upload it to your 
    <a href="http://connect.garmin.com" target="_blank">Garmin Connect</a> account. 
    check the <a href="help#manualUpload" target="_blank">User's Guide</a>
    for how to proceed.</li>
<?php } ?>
  </ul>
  <div class="toolbar">
    <button id="undoAllBtn">Undo All</button>
    <button id="changePoolSizeBtn">Change Pool Size</button>
    <button id="deleteBtn" disabled="disabled">Delete</button>
    <button id="mergeBtn" disabled="disabled">Merge</button>
    <button id="splitBtn" disabled="disabled">Split</button>
    <button id="changeStrokeBtn" disabled="disabled">Change Stroke</button>
    <button id="downloadBtn" style="float: right" onclick="showDownloadPopup(event)">Done</button>
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
    <select name="changeStrokeTo">
      <option value="<?=swt\STROKE_BACKSTROKE ?>">Backstroke</option>
      <option value="<?=swt\STROKE_BREASTSTROKE?>">Breaststroke</option>
      <option value="<?=swt\STROKE_BUTTERFLY?>">Butterfly</option>
      <option value="<?=swt\STROKE_FREESTYLE?>">Freestyle</option>
    </select>
    </p>
    <p>
    Apply to:
    </p>
    <input name="changeStrokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_LENGTH_ONLY?>" 
    checked="checked">Length Only<br>
    <input name="changeStrokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_LAP?>">
    All Lengths in interval<br>
    <input name="changeStrokeOption" type="radio" value="<?=swt\CHANGE_STROKE_OPTION_ALL?>">
    All lengths<br>
    <br>
    <input type="hidden" name="changeStrokeFrom" value="">
    <button type="submit">Change Stroke</button>
  </form>
</div>
<div id="changePoolSizeDlg" class="dialog">
  <div class="dialog-overlay">
    &nbsp;
  </div>
  <form name="changePoolSizeForm" class="dialog-content" action="">
    <a id="changePoolSizeDlgCancelBtn" class="dialog-cancel-button">
      <img src="/content/modal_close.png" alt="Close dialog">
    </a>
    <p>
    Change Pool Size 
    </p>
    <p id="changePoolSizeDlgErrorLbl" class="warning" style="display: none">
    Value must be within 1-150 meters/yards
    </p>
    <input type="text" name="newPoolSize" value="">
    <br><br>
    <input id="metricRdo" name="newPoolSizeUnits" type="radio" 
    value="<?=swt\UNITS_METRIC?>" checked="checked">Meters<br>
    <input id="statuteRdo" name="newPoolSizeUnits" type="radio" 
    value="<?=swt\UNITS_STATUTE?>">Yards<br>
    <br>
    <button type="submit">Change Pool Size</button>
  </form>
</div>
<div id="downloadPopUp" onmouseleave="hideDownloadPopup()" 
style="background-color:white;padding:5px;border:1px solid black;display:none;position:absolute">
<?php if ($product == 5) {?>
  <a href="download" onclick="hideDownloadPopup()">Download Tomtom FIT</a></br>
  <a href="download?garmin=1" onclick="hideDownloadPopup()">Download Garmin FIT</a>
<?php } else { ?>
  <a href="download" onclick="hideDownloadPopup()">Download</a>
<?php } ?>
  <br/><a href="viewer" onclick="hideDownloadPopup()">Open in File Viewer</a>
</div>
<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
<script type="text/javascript" src="/scripts/editor9.js"></script>
<?php
swt\layout::analytics();
swt\Layout::footer();


