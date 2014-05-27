<?php
spl_autoload_register();
\swt\Functions::registerErrorHandler();

\swt\Layout::header("Swimming Watch Data Editor - User'Guide", 
  \swt\Layout::TAB_USER_GUIDE,
  "User's guide for Swim Watch Data Editor");
?>
<div class="section">
  <h2>
    Uploading Activities
  </h2>
  <p>There are two ways to upload activities depending on which Garmin 
  Software is installed on your computer. Ant Agent and the Communicator 
  plug-in have been around for a long time time, but as Garmin is 
  planning a major upgrade to Garmin Connect, the old Ant Agent/Communicator 
  will be replaced with the newer Garmin Express software 
  in the first quarter of 2014. Unfortunately, Garmin Express 
  only works with Garmin Connect and does not allow uploading files to 
  third party web sites.</p>

  <p><strong>ANT Agent/Communicator Users</strong></p>

  <p>If you use Ant Agent but don't have the Communicator Plug-in 
  installed, you can download it form Garmin. The plugin is compatible 
  with most browsers and operating systems. Visit the 
  <a href="http://software.garmin.com/gcp.html" 
    target="_blank">Garmin web site</a>
  to check the requirements and install.</p>

  <p><strong>Garmin Express Users or those who prefer to manually upload 
    files</strong></p>

  <p>Both Garmin Express and Ant agent save a copy of your activity files 
  on your computer. If you want to manually upload files to this web site, 
  you first have to locate the folder to which each software is storing your 
  files.</p>

  <ul>
    <li>Garmin Express users: 
    <a href="http://support.garmin.com/support/searchSupport/case.faces?caseId={afea30d0-9b70-11e3-d5f4-000000000000}" 
      target="_blank">Where does Garmin Express store my activity files</a></li>
    <li>Garmin ANT Agent users: 
    <a href="https://support.garmin.com/support/searchSupport/case.faces?caseId=%7B6e1aa610-6d25-11dc-6782-000000000000%7D" 
      target="_blank">Where does ANT agent store my history</a></li>
  </ul>
  <p>When you're ready, <a href="upload">Go to the upload page</a> and start editing your activities</p>
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
    <li><a id="duplicate">Deleting a length is mostly intended for FR910 users.
      It can be required when a user was moving his arms while resting 
      (treading water in the deep end). The watch will sometimes sense this 
      movement and add a length to the session. Deleting such a length, will 
      replace it with rest time. Duplicate or 'ghost' lengths are another 
      issue with  the FR910. If your file contains duplicates, you will get
      a message telling you which lengths should be deleted. Deleting a 
      duplicate length will erase it from the file.</a></li>
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
  your computer by clicking the download button.</p>
</div>
<div class="section">
  <h2>
    <a id="manualUpload">Uploading the new activity file to Garmin Connect</a>
  </h2>
  
  <p>Login to you Garmin Connect account, go to activity list, and delete 
  the version of the activity (the one with errors) that was uploaded by 
  ANT Agent or Garmin Express.</p>
  
  <p class="warning"><strong>Important:</strong> Garmin Connect doesn't 
  allow two versions of the same activity in your account. This is why
  you have to delete the original version of the activity before you upload 
  the new one. Be aware that by deleting the activity, you will lost any 
  custom information you added that is not part of the data file, like activity
  name, comments etc. Beside that, you can safely delete this activity from 
  your account. This tool create a new version of the activity file and 
  doesn't overwrite the original one. You still have the original activity 
  file on your computer. If things go wrong with the new version, you will be 
  able to restore the old version.</p>

  <p>After the original activity is deleted, click on the 'Upload' button at 
  the top right of the page,</p>

  <img src="content/gc_uploadBtn.jpg"  alt="Garmin Connect Upload button"
  style="display: block; height: 29px; width: 80px; margin: auto">
  
  <p>On the upload page, click on the 'Manual upload' link at the bottom</p>

  <img src="content/gc_manualUpload.jpg" alt="Garmin Connect Manual upload link" 
  style="display: block; height: 436px; width: 387px; margin: auto">

  <p>Select the new activity file, and click the 'Upload activity from file' 
  button</p>
  
  <img src="content/gc_manualUpload2.jpg" alt="Garmin Connect Select File dialog" 
  style="display: block; height: 398px; width: 683px; margin: auto">
</div>
@Analytics.GetGoogleHtml("UA-44807897-1")
\swt\Layout::footer()

