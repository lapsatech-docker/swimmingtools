<?php
spl_autoload_register();
\swt\Functions::registerErrorHandler();

\swt\Layout::header('Swimming Watch Data Editor',
  \swt\Layout::TAB_HOME,
  "Web Application that let you edit Garmin Swim Data");
?>
<div class="section">
  <h2>
    Training with a swimming watch?
  </h2>
  <ul >
    <li>Did you ever notice an extra or missing length, or wrong stroke
    detection? </li>
    <li>Did you ever wish you could edit the data, so that it is acccurate
    and reflect what you did?</li>
    <li>Do you wish Garmin Connect (and other web sites) would allow
    better analysis of your swimming data?</li>
    <li> Swimming watch tools includes both an editor and file viewer,</li>
    <li>The editor allows you to delete, merge, split and change Stroke
    or pool size</li>
    <li>Supported devices: Garmin Swim, Forerunner 645/735/910/920/935, fénix 2/3/5,
    Epix, VivoActive 1/HR/3, D2 Charlie, Descent MK1, Approach S60, and Tomtom Multisport</li>
    <li>Read the <a href="help">User's Guide</a>
    and start editing and analyzing your data.</li>
  </ul>
  <input type="button" id="startEditorButton" value="Start Editor/Viewer"
  onclick="window.location = 'upload';" />
</div>
<div class="section">
  <h2>
    What Swimming watches are, and how they work
  </h2>
  <p>If you are swimming competitively, or just for fun, chances are you
  probably want to swim faster. Physics tells us that speed at the pool,
  is the product of stroke length and stroke rate. </p>

  <p>Stroke length is defined as the number of yards/meters covered with
  each stroke, and stroke rate is the number of strokes per unit of time
  (usually strokes per minute). Moving you arms faster will increase you
  stroke rate and therefore your speed. Stroke length can be improved
  by working on your technique, reducing drag, increasing glide,
  changing your position and being more efficient. </p>

  <p>Let's say you're practicing a new technique and want to calculate your
  stroke rate, stroke length and speed. You can do it by using a clock and
  counting your strokes while swimming down the pool. Keeping track of
  everything on a piece of paper.</p>

  <p>But who wants to do maths while swimming? And what if you want to have
  detailed results for each length? This is how swimming watches can help.
  They work by monitoring how your arm/body moves through water using
  accelerometers.They count strokes, lengths, keep track of time, and do
  all the math, providing you with stroke length, stroke rate, speed,
  and stroke detection, for each length. You will be able to analyze you
  data, and maintain a log. You'll be able to tell if you're improving or
  not, and if all this work is paying off. </p>
  <p>Swimming watches are very accurate...most of the time. Unfortunately,
  depending on your skills and environment (crowded lanes), data will
  sometimes show some inaccuracies. If you have errors in you data, and wish
  to fix them, feel free to use this site.</p>
</div>
<?php
swt\Layout::analytics();
swt\Layout::footer();
