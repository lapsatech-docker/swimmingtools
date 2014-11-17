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

  $error = false;

  if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $email = $_POST['email'];
    $message = $_POST['message'];

    if (preg_match('/^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}$/i', $email) != 1) {
      $email_error = 'Please provide a valid email';
      $error = true;
    }

    if (empty($message)) {
      $message_error = 'Message is empty';
      $error = true;
    }

    if (!$error) {

      $body = '';
      $body .= '<p>'.htmlspecialchars($message).'</p>'; 
      $body .= 'IP: '.$_SERVER['REMOTE_ADDR'].'<br>';
      if (isset($_COOKIE['SN'])) {
        $body .= 'SN: '.$_COOKIE['SN'];
      }

      $mail = new PHPMailer();
      $mail->From = 'contact-forms@swimmingwatchtools.com';
      $mail->FromName = 'Contact Form';
      $mail->addAddress('swimmingwatchtools@gmail.com');
      $mail->addReplyTo($email);
      $mail->isHTML(true);
      $mail->Subject = 'Contact Form - Swimming Watch Data Editor';
      $mail->Body = $body;

      if ($mail->send()) {
        $action = OUTPUT_MESG_SENT;
      } else {
        throw new Exception($mail->ErrorInfo);
      }
    }
  }
} catch (Exception $ex) {
  swt\Functions::errorLog($ex);
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
  <p>Hi, I'm Stephane, I'm a Garmin Swim User ang programmer of this 
  application. I live in Montreal, Quebec, Canada. I like to run, bike and
  swim...for fun. I'm also a fan of sport devices. I bought a Garmin swim 
  because I was frustrated with my swimming and wanted to improve. The 
  Garmin Swim is great because it allows me to tell if I am improving or 
  not. It gives me the metrics I need. Although, it has been pretty accurate,
  sometimes I would swim in a crowded lane, and would have to slow down and
  switch to breaststroke, while looking for an opportunity to pass. As a 
  result, I would get an extra length added to my session, which would have
  an effect on my average and maximum speed. As I read the support forums on
  the Garmin web site, I would see others users wishing they could edit 
  their data on Garmin Connect. To this day, Garmin has not provided this 
  feature. So I decided to give it a try, and this application is the result. 
  Bug reports, suggestions, and nice comments are welcomed.</p>
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
  </form>
</div>
<?php
  break;
}
swt\Layout::analytics();
swt\Layout::footer();
