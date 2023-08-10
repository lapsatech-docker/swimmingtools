<?php
namespace swt;

class Layout {

  const TAB_HOME = 1;
  const TAB_UPLOAD = 2;
  const TAB_EDITOR = 3;
  const TAB_VIEWER = 4;
  const TAB_USER_GUIDE = 5;
  const TAB_CONTACT = 6;

  static function header($title, $active_tab, $description = '') {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<?php 
    if ($active_tab == self::TAB_UPLOAD) 
      echo '<meta http-equiv="X-UA-Compatible" content="IE=8;requiresActiveX=true">';

    if (!empty($description)) { 
?>    
    <meta name="description" content="<?=$description?>">
<?php
    }
?>
    <title><?=$title?></title>
    <meta property="og:type" content="website"> 
    <meta property="og:url" content="http://www.swimmingwatchtools.com"> 
    <meta property="og:title" content="Swimming Watch Tools"> 
    <meta property="og:image" content="http://www.swimmingwatchtools.com/content/garminswim.jpg"> 
    <meta property="og:description" content="Edit and analyse your swimming watch data Online">
    <link rel="stylesheet" type="text/css" href="/content/main3.css">
    <link rel="shortcut icon" href="favicon.ico">
<?php
    if ($active_tab == self::TAB_VIEWER) {
?>
    <link rel="stylesheet" href="//code.jquery.com/ui/1.11.2/themes/smoothness/jquery-ui.css">
    <link rel="stylesheet" href="content/jquery.nouislider.min.css">
<?php 
    }
?>
  </head>
  <body> 
    <div class="header">
      <div class="header-content">
        <h1>Swimming Watch tools</h1>
        <ul class="tabs">
          <li><a href="index" class="<?=$active_tab == self::TAB_HOME ? "selected" : "";?>">Home</a></li>
          <li><a href="upload" class="<?=$active_tab == self::TAB_UPLOAD ? "selected" : "";?>">Upload</a></li>
          <li><a href="editor" class="<?=$active_tab == self::TAB_EDITOR ? "selected" : "";?>">Editor</a></li>
          <li><a href="viewer" class="<?=$active_tab == self::TAB_VIEWER ? "selected" : "";?>">Viewer</a></li>
          <li><a href="help" class="<?=$active_tab == self::TAB_USER_GUIDE ? "selected" : "";?>">User's Guide</a></li>
          <li><a href="contact" class="<?=$active_tab == self::TAB_CONTACT ? "selected" : "";?>">Contact</a></li>
        </ul>
      </div>
    </div>
    <div class="content">
      <div style="color: #D8000C;background-color: #FFBABA" class="section">
        <h2 style="color:black;background:red">
        Swimmingwatchtools will close on August 31, 2023
      </h2>
      <p> This tool is 10 years old. Although
      it still performs well and still does what it was intended to, it is becoming increasingly
      hard to maintain and is due for a complete rewrite. Unfortunately, I don't have time and don't have the
      interest to maintain, support and upgrade it anymore. So I will be pulling the plug at the end of this
      summer. For those that might be interested I'm leaving a link to the 
      <a href="https://bitbucket.org/swimmingwatchtools/phpeditor">source code</a>. If you want
      to take charge, and get the domain name, leave me a message through the <a href="contact">contact</a> form. Finally,
      I want to thank all of you who have used the tool, and submitted one of the 275000 files.
      Have a great summer and keep swimming.
      </p> 
    </div>
<?php 
  }

  static function footer() {
?>
    </div>
  </body>
</html>
<?php
  }
}
