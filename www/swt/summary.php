<?php
namespace swt;

class Summary {
  public static $TYPES = [
    'freestyle', 'backstroke', 'breaststroke', 'butterfly', 'drill', 'unknown', 'rest',
    'freestyle50', 'freestyle100', 'freestyle200', 'freestyle400', 'freestyle500',
    'freestyle800', 'freestyle1000', 'freestyle1500', 'freestyle1650',
    'backstroke50', 'backstroke100', 'backstroke200',
    'breaststroke50', 'breaststroke100', 'breaststroke200',
    'butterfly50', 'butterfly100', 'butterfly200'];

  public $id = null;
  public $type = null;
  public $nbr_lengths = 0;
  public $time = 0.0;
  public $stroke_count = 0;
  public $first_length = NULL;

  function __construct($type) {
    $this->type = $type;
  }

}
