google.load('visualization', '1', {packages: ['corechart', 'controls']});

var STROKE_FREESTYLE = 0;
var STROKE_BACKSTROKE = 1;
var STROKE_BREASTSTROKE = 2;
var STROKE_BUTTERFLY = 3;
var STROKE_DRILL = 4;
var STROKE_MIXED = 5;
var STROKE_UNKNOWN = 0xff;

var LENGTH_INDEX = 0;
var LENGTH_INTERVAL = 1;
var LENGTH_TIME = 2;
var LENGTH_PACE = 3;
var LENGTH_SPM = 4;
var LENGTH_SPL = 5;
var LENGTH_STROKE = 6;

var paceChart_;
var paceChartView_;

var paceChartOptions_ = {
  seriesType: 'bars',
  series: {
    0: {},
    1: { type: 'line', targetAxisIndex: 1, color: 'red'},
    2: { type: 'line', targetAxisIndex: 1, color: 'blue'}
  },
  vAxes: [
    { title: 'Pace' },
    { title: 'Strokes', gridlines: {color: 'transparent'}, minValue:0, maxValue:40}
  ],
  hAxis: {
    title: 'Lengths',
    viewWindow: {},
  },
  annotations: {textStyle: {color: 'black', auraColor: 'transparent'}, boxStyle:{stroke:'#888', strokeWidth:1, rx:1,ry:1, gradient: {color1: 'white', color2: 'white', x1:'0%', y1:'0%', x2:'100%', y2:'100%'},}},
  legend: {position: 'none'},
  focusTarget: 'category'
};

function initialize () 
{
  initializeSummaryChart();
  initializePaceChart();
  initializeStrokeChart(STROKE_FREESTYLE);
}

function initializeSummaryChart()
{
  var options = {
    colors:['#c43dbf', '#1f8ef9', '#95de2b', '#eb3d3d', 'cadetBlue', 'cadetBlue', '#ff7c05', 'lightGray'],
    legend: {position: 'none'},
    pieHole: 0.5,
    pieSliceText: 'none',
    backgroundColor: '#eee'
  }

  summaryChartData_ = new google.visualization.DataTable(summaryChartData_);

  var summaryChart = new google.visualization.PieChart(document.getElementById('summaryChart'));
  summaryChart.draw(summaryChartData_, options);
}

function initializePaceChart() 
{
  lengthsData_ = new google.visualization.DataTable(lengthsData_);

  if (lengthsData_.getNumberOfRows() == 0) {
    paceChart_ = document.getElementById('paceChart');
    paceChart_.innerHTML = '<p class="warning">Unexpected error has been logged. '
      + '<a href="contact">Contact developper if it persists</p>';
  } else {

    var maxPace = lengthsData_.getValue(0,LENGTH_PACE);
    var minPace = lengthsData_.getValue(0,LENGTH_PACE);
    for (var i = 0; i < lengthsData_.getNumberOfRows(); i++) {
      var pace = lengthsData_.getValue(i,LENGTH_PACE);

      if (pace != null) {
        if (pace > maxPace)
          maxPace = pace;

        if (pace < minPace)
          minPace = pace;
      }
    }

    var minValue = minPace - (5 * (maxPace - minPace) / 70);  
    var maxValue = maxPace + (25 * (maxPace - minPace) / 70); 
    // Nearest multiple of 5 smaller than minValue
    var minValue = (Math.floor(minValue / 5) * 5);
    // Nearest multiple of 5 larger than maxValue
    var maxValue = (Math.floor(maxValue / 5) * 5) + 5;

    var tickInterval = 1;
    if ((maxValue - minValue) >= 4) {
       tickInterval = Math.floor((maxValue - minValue) / 4);
       // Try to make it multiple of 5 
       if (tickInterval >= 5) 
         tickInterval = Math.floor(tickInterval / 5) * 5;
    }

    paceChartOptions_.vAxes[0].ticks = new Array();
    for(var tick = minValue; tick <= maxValue; tick += tickInterval) {
      paceChartOptions_.vAxes[0].ticks.push({v:maxValue + minValue - tick, f:formatTime(tick)});
    }

    paceChartView_ = new google.visualization.DataView(lengthsData_);
    paceChartView_.setColumns([LENGTH_INDEX,LENGTH_INTERVAL, 
    {
      calc: function(table, row) {
        var tooltip = '';
        if (table.getValue(row, LENGTH_STROKE) !== null) {
          tooltip = 'Length: ' + table.getValue(row, LENGTH_INDEX) + '\n';
          tooltip += strokeLookup(table.getValue(row, LENGTH_STROKE)) + '\n';
          tooltip += 'Pace: ' + formatTime(table.getValue(row, LENGTH_PACE))+ '\n';
          tooltip += 'SPM: ' + table.getValue(row, LENGTH_SPM) + '\n';
          tooltip += 'SPL: ' + table.getValue(row, LENGTH_SPL);
        } else {
          tooltip = 'Rest: ' + formatTime(table.getValue(row, LENGTH_TIME));
        }
        return tooltip;
      },
      type: 'string', role: 'tooltip'
    },
    { 
      calc: function(table, row) {
        if (table.getValue(row,LENGTH_PACE) != null) {
          return maxValue + minValue - table.getValue(row,LENGTH_PACE);
        } else { 
          return null;
        }
      }, 
      type: 'number'
    }, 
    {
      calc: function(table, row) {
        var color = null;
        if (table.getValue(row,LENGTH_STROKE) !== null) { //not rest

          switch (table.getValue(row, LENGTH_STROKE)) {
            case STROKE_FREESTYLE:
              color = '#c43dbf';
              break;
            case STROKE_BACKSTROKE:
              color = '#1f8ef9';
              break;
            case STROKE_BREASTSTROKE:
              color = '#95de2b';
              break;
            case STROKE_BUTTERFLY:
              color = '#eb3d3d';
              break;
            case STROKE_DRILL:
              color = '#ff7c05';
              break;
            case STROKE_UNKNOWN:
            case STROKE_MIXED:
              color = 'cadetBlue';
              break;
            default:
              color = 'gray';
              break;
          }
          return color;
        }
      },
      type: 'string', role: 'style' 
    },
    {
      calc: function(table, row) {
        var spm = table.getValue(row, LENGTH_SPM);
        return spm == 0 ? null : spm;
      },
      type: 'number'
    },
    {
      calc: function(table, row) {
        var spl = table.getValue(row, LENGTH_SPL);
        return spl == 0 ? null : spl;
      },
      type: 'number'
    }]);

    paceChart_ = new google.visualization.ComboChart(document.getElementById('paceChart'));
    paceChart_.draw(paceChartView_, paceChartOptions_);

    $('#paceChartSlider').noUiSlider({
      start:[0,paceChartView_.getNumberOfRows()], 
        step:1, 
        range: {min:0, max: paceChartView_.getNumberOfRows()}, 
        connect:true, 
        behaviour: 'drag-tap'});
    $('#paceChartSlider').on({slide:paceChartSlider_onSlide});
  }
}

function initializeStrokeChart()
{
  var freeNumLengths = 0;
  var backNumLengths = 0;
  var breastNumLengths = 0;
  var flyNumLengths = 0;
  var mixedNumLengths = 0;
  var unkNumLengths = 0;

  for (var i = 0; i < lengthsData_.getNumberOfRows(); i++) {
    var stroke = lengthsData_.getValue(i, LENGTH_STROKE);
    if (stroke !== null) {
      switch (stroke) {
        case STROKE_FREESTYLE:
          freeNumLengths++;
          break;
        case STROKE_BACKSTROKE:
          backNumLengths++;
          break;
        case STROKE_BREASTSTROKE:
          breastNumLengths++;
          break;
        case STROKE_BUTTERFLY:
          flyNumLengths++;
          break;
        case STROKE_MIXED:
          mixedNumLengths++;
          break;
        case STROKE_UNKNOWN:
          unkNumLengths++;
          break;
        default:
          unkNumLengths++;
          break;
      }
    }
  }
  if (freeNumLengths > 0)
    updateStrokeChart(STROKE_FREESTYLE);
  else if (backNumLengths > 0)
    updateStrokeChart(STROKE_BACKSTROKE);
  else if (breastNumLengths > 0)
    updateStrokeChart(STROKE_BREASTSTROKE);
  else if (flyNumLengths > 0)
    updateStrokeChart(STROKE_BUTTERFLY);
  else if (mixedNumLengths > 0)
    updateStrokeChart(STROKE_MIXED);
  else if (unkNumLengths > 0)
    updateStrokeChart(STROKE_UNKNOWN);
  
  document.getElementById('strokeChartSelect').onchange = strokeChartSelect_onChange;
}


function updateStrokeChart(stroke) 
{
  var strokeChartOptions = {legend: {position: 'none'}, 
  hAxis: {title: 'Stroke rate (strokes/min)'},
  vAxis: {title: 'Stroke count'}};

  var maxPace = lengthsData_.getValue(0, LENGTH_PACE);
  var minPace = lengthsData_.getValue(0, LENGTH_PACE);
  var numLengths = 0;
  var splTotal = 0;
  var timeTotal = 0;

  for (var i = 0; i < lengthsData_.getNumberOfRows(); i++) {
    if (lengthsData_.getValue(i, LENGTH_STROKE) === stroke) {
      var pace = lengthsData_.getValue(i, LENGTH_PACE);
      numLengths++;
      timeTotal += lengthsData_.getValue(i, LENGTH_TIME)
      splTotal += lengthsData_.getValue(i, LENGTH_SPL);

      if (pace != null) {
        if (pace > maxPace)
          maxPace = pace;

        if (pace < minPace)
          minPace = pace;
      }
    }
  }

  var strokeChartView = new google.visualization.DataView(lengthsData_);
  strokeChartView.setColumns([LENGTH_SPM, LENGTH_SPL, {
    calc: function(table, row) {
      var value = (16711680 + (256 * Math.round((table.getValue(row,LENGTH_PACE) - minPace) * 255 / (maxPace - minPace)))).toString(16);
      return value;
    },
    type: 'string',
    role: 'style'
  },
  {
    calc: function(table, row) {
      var tooltip = '';
      if (table.getValue(row, LENGTH_STROKE) !== null) {
        tooltip = 'Length: ' + table.getValue(row, LENGTH_INDEX) + '\n';
        tooltip += 'SPM: ' + table.getValue(row, LENGTH_SPM) + '\n';
        tooltip += 'SPL: ' + table.getValue(row, LENGTH_SPL) + '\n';
        tooltip += 'Pace: ' + formatTime(table.getValue(row, LENGTH_PACE));
      }
      return tooltip;
    },
    type: 'string', role: 'tooltip'
  }]);


  strokeChartView.setRows(lengthsData_.getFilteredRows([{
    column: LENGTH_STROKE, 
    test: function(value, row, col, table) {
      if (value === stroke
        && table.getValue(row, LENGTH_SPM) > 0 
        && table.getValue(row, LENGTH_SPL) > 0) 
        return true; 
      else 
        return false;;
    }
  }])); 

  strokeChart = new google.visualization.ScatterChart(document.getElementById('strokeChart'));
  strokeChart.draw(strokeChartView, strokeChartOptions);
  document.getElementById('avgSpl').innerHTML = Math.round(splTotal / numLengths * 10) / 10;
  document.getElementById('avgSpm').innerHTML = Math.round(60 * splTotal / timeTotal * 10) / 10;
}

function formatTime(timeInSeconds) 
{
  var timeStr;
  var seconds = Math.floor(timeInSeconds);
  var minutes = Math.floor(seconds / 60);
  var hours = Math.floor(minutes / 60);

  seconds = seconds % 60;
  minutes = minutes % 60;
  seconds = '00' + seconds.toString();
  seconds = seconds.substr(seconds.length -2, 2);

  if (hours > 0) {
    minutes = '00' + minutes.toString();
    minutes = minutes.substr(minutes.length -2, 2);
    timeStr = hours.toString() + ':' + minutes + ':' + seconds;
  } else {
    timeStr = minutes.toString() + ':' + seconds;
  }
  return timeStr;
}

function strokeLookup(stroke) {
  var returnValue = 'Unknown';
  switch(stroke) {
    case STROKE_FREESTYLE:
      returnValue = 'Freestyle';
      break;
    case STROKE_BACKSTROKE:
      returnValue = 'Backstroke';
      break;
    case STROKE_BREASTSTROKE:
      returnValue = 'Breaststroke';
      break;
    case STROKE_BUTTERFLY:
      returnValue = 'Butterfly';
      break;
    case STROKE_DRILL:
      returnValue = 'Drill';
      break;
    case STROKE_MIXED:
      returnValue = 'Mixed';
      break;
    default:
      returnValue = 'Unknown';
  }
  return returnValue;
}

function paceChartSlider_onSlide() 
{
  paceChartOptions_.hAxis.viewWindow.min = $(this).val()[0];
  paceChartOptions_.hAxis.viewWindow.max = $(this).val()[1];
  paceChart_.draw(paceChartView_, paceChartOptions_);
}

function strokeChartSelect_onChange()
{
  var strokeChartSelect = document.getElementById('strokeChartSelect');
  var stroke = strokeChartSelect.options[strokeChartSelect.selectedIndex].value;
  stroke = parseInt(stroke);

  updateStrokeChart(stroke);
}

google.setOnLoadCallback(initialize);
