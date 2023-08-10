﻿<?php
spl_autoload_register();
\swt\Functions::registerErrorHandler();

\swt\Layout::header("Swimming Watch Data Editor - User'Guide",
  \swt\Layout::TAB_USER_GUIDE,
  "User's guide for Swim Watch Data Editor");
?>
<div class="section">
  <h2>
    <a id="upload">Getting a copy of your activity file</a>
  </h2>

  <p>The most convenient way to get a copy of your data, is to export it from
  Garmin Connect. You can do this by going to the activity page, clicking on
  the gear icon to the left, and selecting 'export as original' as shown
  below. </p>

  <img src="content/gc_export.jpg" alt="how to export from Garmin Connect"
  style="display:block; height: 268px; width:838px; margin:auto">

  <p>When you're ready, <a href="upload">Go to the upload page</a> and
  start editing your data.</p>
</div>
<div class="section">
  <h2>
    Editing Activities
  </h2>
  <p>After an activity has been uploaded, you'll see a bar chart. Each bar
  is showing how long (in seconds) you took to swim a length. The color of
  the bar indicates the stroke. The tool uses the same color scheme as Garmin
  Connect. On the same chart, you'll see a black line showing average time
  per length, with labels for intervals and rests.</p>

  <p>Above the chart is a toolbar with buttons, which should be self
  explanatory.</p>

  <img src="content/chart.jpg" alt="How to use the Editor graph"
  style="display: block; height: 530px; width: 803px; margin: auto">

  <p> At the bottom, a status bar shows a summary including total swim
  time, distance, average speed, and average stroke/length. This summary
  will be updated as you make changes to the data.</p>

  <p>If you swam a lot of lengths (more than 20) a scrollbar will appear
  at the bottom of the chart. </p>

  <p>Then, you'll have to identify inaccuracies:</p>
  <ul>
    <li>Changing pool size is sometime required for users training in pools
    with different pool sizes. Metrics provided by swimming watches depend
    on pool size, and you have to provide this value. But sometimes, you might
    forget updating it if you swim in an different pool than last time. To
    fix this kind of issue, just click the 'Change pool Size' button and
    provide the new pool size in the dialog</li>
    <li>Identifying wrong stroke is easy. If you know you only swam Freestyle
    in an interval, and see lengths with a different stroke, then it's quite
    obvious, you'll have to change stroke for those lengths</li>
    <li>Deleting a length is mostly intended for FR910 users. It can be required
    when a user was moving his arms while resting (treading water in the
    deep end). The watch will sometimes sense this movement and add a length
    to the session. Deleting a length, will replace it with
    rest time.</li>
    <li>Merging two lengths is required when the watch has inserted an extra
    length while swimming. This can happen when a user suddenly changes pace,
    stops, or switches stroke mid-length (most likely in crowded lanes). On
    the graph, you will typically see two consecutive lengths well below
    average. You will be able to tell that if you merge those two lengths,
    you will get a length near average time.</li>
    <li>Splitting a length is required when the watch has missed a turn.
    This can happen to users who fail to use a strong push-off from the
    wall with a long streamlining (glide) phase. On the graph, you will see
    a length well above average. You will be able to tell that splitting
    that length will give you two lengths with average time.</li>
  </ul>

  <p>To make a change, select a length by clicking on it. Once it is
  selected, it will change appearance. After selection is made, click on
  one of the 4 action buttons on the toolbar. Sometimes, one or more buttons
  will be grayed because the action can't be done on the selected length.</p>

  <ul>
    <li>To merge 2 lengths, you have to select the first of the 2 lengths to
    be merged,</li>
    <li>A length can't be merged if it is the last length of an interval,</li>
    <li>A length can't be merged if it is followed by a rest</li>
    <li>you cannot do anything with drills lengths.</li>
  </ul>

  <p>If you change stroke, you'll be presented with a dialog box </p>

  <img src="content/changeStrokeDlg.jpg" alt="Change Stroke Dialog"
  style="display: block; height: 282px; width: 250px; margin: auto">

  <p>Choose the new stroke and decide if you want to this new stroke applied
  to selected length only, lengths in the same interval,  or all lengths.</p>

  <p>When you are happy with the results, download the new activity file to
  your computer by clicking the done button and download link.</p>
</div>
<div class="section">
  <h2>
    <a id="manualUpload">Uploading the new activity file to Garmin Connect</a>
  </h2>

  <p>Login to your Garmin Connect account. Go to Activity list by selecting
  'Activities' from the main menu, and delete the version of the activity that was
  uploaded by Garmin Express (the one with errors).</p>

  <img src="content/gc_activities.jpg"  alt="Garmin Connect activities menu"
  style="display: block; height: 167px; width: 297px; margin: auto">

  <p class="warning"><strong>Important:</strong> Garmin Connect doesn't
  allow two versions of the same activity in your account. This is why
  you have to delete the original version of the activity before you upload
  the new one. Be aware that by deleting the activity, you will lost any
  custom information you added that is not part of the data file, like activity
  name, comments etc.</p>

  <p>After the original activity is deleted, click on the cloud icon at
  the top left of the page, and select 'Import data'</p>

  <img src="content/gc_import1.jpg"  alt="Garmin Connect import link"
  style="display: block; height: 163px; width:357px; margin: auto">

  <p>Click the 'Browse' link, select the new activity file, and click the 'Import Data'
  button.</p>

</div>
<?php
swt\Layout::footer();
