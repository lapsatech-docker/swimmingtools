<?php
require 'PHPMailerAutoload.php';
spl_autoload_register('spl_autoload');
swt\Functions::registerErrorHandler();

define('OUTPUT_FORM', 1);
define('OUTPUT_MESG_SENT', 2);
define('OUTPUT_ERROR', 3);

try {

  $action = OUTPUT_FORM;
  $email_error = '';
  $message_error = '';
  $email = '';
  $message = '';
  $timestamp = time();
  $is_spam = false;
  $error = false;

  if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $email = $_POST['email'];
    $message = $_POST['message'];
    $timestamp = (int) $_POST['timestamp'];
    $is_spam = abs(time() - $timestamp) < 2;


    if (preg_match('/^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$/i', $email) != 1) {
      $email_error = 'Please provide a valid email';
      $error = true;
    }

    if (empty($message)) {
      $message_error = 'Message is empty';
      $error = true;
    }

    if (!$error && !$is_spam) {

      $body = '';
      $body .= '<p>'.htmlspecialchars($message).'</p>'; 
      $body .= 'IP: '.$_SERVER['REMOTE_ADDR'].'<br>';
      if (isset($_COOKIE['SN'])) {
        $body .= 'SN: '.$_COOKIE['SN'];
      }

      $mail = new PHPMailer();
      $mail->CharSet = 'utf-8';
      $mail->From = 'contact-forms@swimmingwatchtools.com';
      $mail->FromName = 'Contact Form';
      // put email address you want messages to be sent
      $mail->addAddress('');
      $mail->addReplyTo($email);
      $mail->isHTML(true);
      $mail->Subject = 'Swimmingwatchtools.com - Contact Form';
      $mail->Body = $body;

      if ($mail->send()) {
        $action = OUTPUT_MESG_SENT;
      } else {
        throw new Exception($mail->ErrorInfo);
      }
    }
  }
} catch (Exception $ex) {
  swt\DB::addErrorLogEntry(NULL, $ex->getFile(), $ex);
  $action = OUTPUT_ERROR;
}
swt\Layout::header('Swimming Watch Data Editor - Contact',
  swt\Layout::TAB_CONTACT,
  'Contact the developper of Swim Watch Data Editor');

switch($action) {
case OUTPUT_MESG_SENT:
  echo '<p style="font-weight: bold; font-size: 1.5em">Message has been sent. '
    .' Thanks for your feedback!</p>';
  break;

case OUTPUT_ERROR:
  echo '<p class="warning">Error sending mail, you can send message to the '
    .'following email address: swimmingwatchtools@gmail.com</p>';
  break;
case OUTPUT_FORM:
?>
<div class="section">
  <h2>
    Problems with the site, suggestions, comments? Your messages are welcomed
  </h2>
  <p>Put some text here</p>
  <form action="contact" method="post">
    <p>Email: 
      <?php if (!empty($email_error)) echo '<span class="warning">'.$email_error.'</span>';?>
    </p>
    <p><input type="text" name="email" style="width: 450px" value="<?=$email?>"></p>
    <p>Message: 
      <?php if (!empty($message_error)) echo '<span class="warning">'.$message_error.'</span>';?>
    </p>
    <textarea name="message" rows="10" 
      style="margin: auto 30px; width: 840px"><?=$message?></textarea><br />
    <br>
    <p><input type="submit"></p>
    <input type="hidden" name="timestamp" value="<?=time()?>">
  </form>
</div>
<?php
  break;
}
swt\Layout::analytics();
swt\Layout::footer();
