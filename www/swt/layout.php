<?php
namespace swt;

class Layout {

  const TAB_HOME = 1;
  const TAB_EDITOR = 2;
  const TAB_USER_GUIDE = 3;
  const TAB_CONTACT = 4;

  static function header($title, $active_tab, $description = '') {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=8;requiresActiveX=true">        
<?php
    if (!empty($description)) { 
?>    
    <meta name="description" content="<?=$description?>">
<?php
    }
?>
    <title><?=$title?></title>
    <meta property="og:type" content="website"> 
    <meta property="og:url" content="http://www.swimmingwatchtools.com"> 
    <meta property="og:title" content="Swimming Watch Data Editor"> 
    <meta property="og:image" content="http://www.swimmingwatchtools.com/content/garminswim.jpg"> 
    <meta property="og:description" content="Edit your Garmin Swim or Forerunner 910 Data Free Online">
    <link rel="stylesheet" type="text/css" href="/content/main.css">
    <link rel="shortcut icon" href="favicon.ico">
  </head>
  <body> 
    <div class="header">
      <div class="header-content">
        <h1>Swimming Watch Data Editor</h1>
        <ul class="tabs">
          <li><a href="index" class="<?=$active_tab == self::TAB_HOME ? "selected" : "";?>">Home</a></li>
          <li><a href="help" class="<?=$active_tab == self::TAB_USER_GUIDE ? "selected" : "";?>">User's Guide</a></li>
          <li><a href="editor" class="<?=$active_tab == self::TAB_EDITOR ? "selected" : "";?>">Editor</a></li>
          <li><a href="contact" class="<?=$active_tab == self::TAB_CONTACT ? "selected" : "";?>">Contact</a></li>

        </ul>
      </div>
    </div>
    <div class="content">
<?php 
  }

  static function footer() {
?>
    </div>
  </body>
</html>
<?php
  }

  static function analytics() {
?>
<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-44807897-1', 'swimmingwatchtools.com');
  ga('send', 'pageview');

</script>
<?php
  }
}
