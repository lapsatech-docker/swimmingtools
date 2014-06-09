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
    and reflect what you did.</li>
    <li>This is what Swimming Watch Data Editor is about. Post-editing 
    your data by deleting, merging or splitting length, 
    and changing Stroke, or pool size</li>
    <li>Supported devices: Garmin Swim, Forerunner 910 and fénix 2.
    <li>Read the <a href="help">User's Guide</a>
    and start editing your data.</li>
  </ul>
  <input type="button" id="startEditorButton" value="Start Editor" 
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
  sometimes show some inaccuracies. Here is Advice from Garmin on how 
  to improve accuracy and reduce errors in the data</p>
  <ul >
    <li><a href="http://support.garmin.com/support/searchSupport/case.faces?supportPage=Garmin%20Swim&caseId={41c35e70-facc-11e0-73d0-000000000000}&locale=en_US" 
      target="_blank">
      Why am I experiencing errors with my pool swimming distance?</a></li>
    <li><a href="http://support.garmin.com/support/searchSupport/case.faces?caseId={08519320-0271-11e2-e8a7-000000000000}" 
      target="_blank">Can traffic from other swimmers in my lane cause 
      distance errors when Pool Swimming?</a></li>
  </ul>
  <p>If you follow this advice, you data will be fine most of the time. 
  If you still have errors in you data, feel free to use this site. At 
  this time, it is only compatible with the Garmin Swim, forerunner 910
  and Fénix 2 devices, but if there is enough interest. Support could be
  expanded to other devices.</p>

  <p>If you don't train with a swimming watch yet, and considering buying
  one, here are links to manufacturers and excellent independent reviews form 
  <a href="http://www.dcrainmaker.com/" target="_blank">DC Rainmaker</a></p>

  <p>Swimming watches</p>

  <ul>
    <li><a href="http://www.finisinc.com/equipment/electronics/swimsense.html" 
      target="_blank">Finis SwimSense </a> / 
    <a href="http://www.dcrainmaker.com/2010/12/hands-on-look-at-finis-swimsense.html" 
      target="_blank">Review</a></li>
    <li><a href="http://sites.garmin.com/swim/" target="_blank">Garmin Swim</a> / 
    <a href="http://www.dcrainmaker.com/2012/06/garmin-swim-watch-in-depth-review.html"
      target="_blank">Review</a></li>
    <li><a href="http://swimovateusa.com/product/poolmate-pro/" 
      target="_blank">Swimovate PoolMatePro</a> and 
    <a href="http://swimovateusa.com/product/swimovate-poolmate-heart-rate-watch/" 
      target="_blank">PoolMatePro HR</a> / 
    <a href="http://www.dcrainmaker.com/2011/02/hands-on-look-at-swimovate-pool-mate.html" 
      target="_blank">Review</a></li>
  </ul>
  <p>Multi-Sport (Triathlon) watches with swimming mode</p>
  <ul >
    <li><a href="http://sites.garmin.com/forerunner910xt/" 
      target="_blank">Garmin Forerunner 910</a> / 
    <a href="http://www.dcrainmaker.com/2011/10/garmin-forerunner-910xt-in-depth-review.html" 
      target="_blank">Review</a></li>
    <li><a href="https://buy.garmin.com/en-US/US/prod159116.html" 
      target="_blank">Garmin Fénix 2</a> / 
    <a href="http://www.dcrainmaker.com/2014/03/garmin-fenix2-multisport.html" 
      target="_blank">Review</a></li>
    <li><a href="http://www.suunto.com/ambit/" target="_blank">Suunto Ambit 2 </a> / 
    <a href="http://www.dcrainmaker.com/2013/04/suunto-ambit2-review.html" 
      target="_blank">Review</a></li>
    <li><a href="http://sports.tomtom.com/en_us/" target="_blank">TomTom Multisport</a> / 
    <a href="http://www.dcrainmaker.com/2014/04/tomtom-cardio.html" 
      target="_blank">Review</a></li>
  </ul>
</div>
<?php
swt\Layout::analytics();
swt\Layout::footer();
