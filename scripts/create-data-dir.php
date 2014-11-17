<?php



printf('Enter full data dir: ');
fscanf(STDIN, '%s', $data_dir);
printf('enter user: ');
fscanf(STDIN, '%s', $username);
printf('enter group: ');
fscanf(STDIN, '%s', $group);


for ($i = 0; $i < 1000; ++$i) {
  $dir = sprintf("$data_dir/%03d", $i);
  if (!file_exists($dir)) {
    if (!mkdir($dir, 0770, true)) die('Cannot create '.$dir);
    if (!chown($dir, $username)) die('Cannot chown '.$username);
    if (!chgrp($dir, $group)) die('Cannot chgrg '.$group);
  } else if(file_exists($dir) && is_dir($dir)) {
    if (!chmod($dir, 0770)) die('Cannot chmod '.$dir);
    if (!chown($dir, $username)) die('Cannot chown '.$username);
    if (!chgrp($dir, $group)) die('Cannot chgrg '.$group);
  } else
    die('Cannot create '.$dir.' because a file with the same name exists');  
}
