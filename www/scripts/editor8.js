google.load('visualization', '1', {packages: ['corechart', 'controls']});

var EDITOR_ACTION_MERGE = 1;
var EDITOR_ACTION_SPLIT = 2;
var EDITOR_ACTION_CHANGE_STROKE = 3;
var EDITOR_ACTION_DELETE = 4;
var EDITOR_ACTION_CHANGE_POOL_SIZE = 5;
var EDITOR_ACTION_UNDO_ALL = 6;

var STROKE_FREESTYLE = 0;
var STROKE_BACKSTROKE = 1;
var STROKE_BREASTSTROKE = 2;
var STROKE_BUTTERFLY = 3;
var STROKE_DRILL = 4;
var STROKE_MIXED = 5;
var STROKE_UNKNOWN = 0xff;

var chart;
var dataTable;
var selection;
var isProcessing;
var undoAllBtn;
var mergeBtn;
var splitBtn;
var changeStrokeBtn;
var changePoolSizeBtn;
var deleteBtn;
var distanceLbl;
var swimTimeLbl;
var avgPaceLbl;
var avgStrokeLbl;
var statusLbl;
var selectedLengthLbl;
var selectedLengthStrokeLbl;
var options = {
  seriesType: 'bars',
  series: {
    0: { type: 'line' }
  },
  colors: ['black'],
  vAxis: { title: 'Time (sec)' },
  hAxis: {
    title: 'Lengths',
    gridlines: {color: 'transparent'},
    ticks: []
  },
  chartArea: { left: 100, top: 25, width: 800},
  width: 800,
  height: 450,
  isStacked: true,
  legend: {position: 'none'},
  tooltip: {isHtml: false, textStyle: {fontSize: 15} },
  focusTarget: 'category'
};

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
function initialize() {
  chart = new google.visualization.ComboChart(document.getElementById('chart'));

  distanceLbl = document.getElementById('distanceLbl');
  swimTimeLbl = document.getElementById('swimTimeLbl');
  avgPaceLbl = document.getElementById('avgPaceLbl');
  abgStrokesLbl = document.getElementById('avgStrokesLbl');
  statusLbl = document.getElementById('statusLbl');
  selectedLengthLbl = document.getElementById('selectedLengthLbl');
  selectedLengthStrokeLbl = document.getElementById('selectedLengthStrokeLbl');
  undoAllBtn = document.getElementById('undoAllBtn');
  undoAllBtn.onclick = undoAllBtn_onClick;
  mergeBtn = document.getElementById('mergeBtn')
    mergeBtn.onclick = mergeBtn_onClick;
  splitBtn = document.getElementById('splitBtn')
    splitBtn.onclick = splitBtn_onClick;
  changeStrokeBtn = document.getElementById('changeStrokeBtn');
  changeStrokeBtn.onclick = changeStrokeBtn_onClick;
  document.changeStrokeForm.onsubmit = changeStrokeForm_onSubmit;
  document.getElementById('changeStrokeDlgCancelBtn').onclick = ChangeStrokeDlgCancelBtn_onclick;
  changePoolSizeBtn = document.getElementById('changePoolSizeBtn');
  changePoolSizeBtn.onclick = changePoolSizeBtn_onClick;
  deleteBtn = document.getElementById('deleteBtn');
  deleteBtn.onclick = deleteBtn_onClick;
  document.changePoolSizeForm.onsubmit = changePoolSizeForm_onSubmit;
  document.getElementById('changePoolSizeDlgCancelBtn').onclick = ChangePoolSizeDlgCancelBtn_onclick;

  google.visualization.events.addListener(chart, 'select', chart_onSelect);

  isProcessing = true;
  updateToolbar(true);
  var query = new google.visualization.Query('ajax-googlechart.php');
  query.send(handleQueryResponse);
}

function undoAllBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = 'Undoing All...';

  var query = new google.visualization.Query(
      'ajax-googlechart.php?action=' + EDITOR_ACTION_UNDO_ALL);
  query.send(handleQueryResponse);
}


function mergeBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = 'Merging...';

  var lengthIndex = dataTable.getRowProperty(selection.row, 'lengthIndex');

  var query = new google.visualization.Query(
      'ajax-googlechart.php?action=' + EDITOR_ACTION_MERGE + '&lengthIndex=' + lengthIndex);
  query.send(handleQueryResponse);
}


function splitBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = 'Splitting...';

  var lengthIndex = dataTable.getRowProperty(selection.row, 'lengthIndex');

  var query = new google.visualization.Query(
      'ajax-googlechart.php?action=' + EDITOR_ACTION_SPLIT + '&lengthIndex=' + lengthIndex);
  query.send(handleQueryResponse);
}

function changeStrokeBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = "Changing Stroke";

  selectedLengthLbl.innerHTML = selection.row + 1;
  swimStroke = dataTable.getRowProperty(selection.row, 'swimStroke');
  swimStroke = strokeLookup(swimStroke);
  selectedLengthStrokeLbl.innerHTML = swimStroke;
  document.changeStrokeForm.changeStrokeFrom.value = dataTable.getRowProperty(selection.row, 'swimStroke');
  document.getElementById('changeStrokeDlg').style.display = 'block';
}

function ChangeStrokeDlgCancelBtn_onclick() {
  document.getElementById('changeStrokeDlg').style.display = 'none';
  statusLbl.innerHTML = "Changing Stroke cancelled";
  isProcessing = false;
  updateToolbar(false);
}

function changeStrokeForm_onSubmit() {
  statusLbl.innerHTML = 'Changing Stroke...';
  var changeStrokeOption;

  for (var i = 0; i < document.changeStrokeForm.changeStrokeOption.length; i++) {
    if (document.changeStrokeForm.changeStrokeOption[i].checked) {
      changeStrokeOption = document.changeStrokeForm.changeStrokeOption[i].value;
      break;
    }
  }

  var changeStrokeFrom = document.changeStrokeForm.changeStrokeFrom.value;
  var changeStrokeTo = document.changeStrokeForm.changeStrokeTo.options[document.changeStrokeForm.changeStrokeTo.selectedIndex].value;
  var lengthIndex = dataTable.getRowProperty(selection.row, 'lengthIndex');

  var query = new google.visualization.Query(
      'ajax-googlechart.php?action=' + EDITOR_ACTION_CHANGE_STROKE + '&lengthIndex=' + lengthIndex
      + '&changeStrokeFrom=' + changeStrokeFrom
      + '&changeStrokeTo=' + changeStrokeTo
      + '&changeStrokeOption=' + changeStrokeOption);
  query.send(handleQueryResponse);
  document.getElementById('changeStrokeDlg').style.display = 'none';
  return false;
}

function changePoolSizeBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = "Changing pool size...";

  document.changePoolSizeForm.reset();
  document.getElementById('changePoolSizeDlgErrorLbl').style.display = 'none';
  document.changePoolSizeForm.newPoolSize.value = dataTable.getTableProperty('poolSize');
  if (dataTable.getTableProperty('poolSizeUnits') == 'y')
    document.getElementById('statuteRdo').checked = true;
  document.getElementById('changePoolSizeDlg').style.display = 'block';
}

function ChangePoolSizeDlgCancelBtn_onclick() {
  document.getElementById('changePoolSizeDlg').style.display = 'none';
  statusLbl.innerHTML = "Changing Pool Size cancelled";
  isProcessing = false;
  updateToolbar(false);
}

function changePoolSizeForm_onSubmit() {
  statusLbl.innerHTML = 'Changing Pool Size...';

  var newPoolSizeUnits;

  for (var i = 0; i < document.changePoolSizeForm.newPoolSizeUnits.length; i++) {
    if (document.changePoolSizeForm.newPoolSizeUnits[i].checked) {
      newPoolSizeUnits = document.changePoolSizeForm.newPoolSizeUnits[i].value;
      break;
    }
  }

  var newPoolSize = document.changePoolSizeForm.newPoolSize.value;
  var error = false;

  if (isNaN(parseFloat(newPoolSize))) {
    error = true;
  } else if (newPoolSizeUnits == 0) {
    if (!(newPoolSize >= 17 && newPoolSize <= 150))
      error = true;
  } else if (newPoolSizeUnits == 1) {
    if (!(newPoolSize >= 18 && newPoolSize <= 150))
      error = true;
  }

  if (error) {
    document.getElementById('changePoolSizeDlgErrorLbl').style.display = 'block';
  } else {
    var query = new google.visualization.Query(
        'ajax-googlechart.php?action=' + EDITOR_ACTION_CHANGE_POOL_SIZE + '&newPoolSize=' + newPoolSize
        + '&newPoolSizeUnits=' + newPoolSizeUnits);
    query.send(handleQueryResponse);
    document.getElementById('changePoolSizeDlg').style.display = 'none';
  }

  return false;
}

function deleteBtn_onClick() {
  var isDuplicate = false;
  if (dataTable.getRowProperty(selection.row, 'isDuplicate')) {
    isDuplicate = true;
  } else if (selection.row < dataTable.getNumberOfRows() - 1)     {
    if (dataTable.getRowProperty(selection.row + 1, 'isDuplicate')) {
      isDuplicate = true;
    }
  }

  var r = false;
  if (!isDuplicate) {
    r = confirm('Length will be converted to Rest time.\nDo you want to proceed?')
  }

  if (r || isDuplicate) {
    isProcessing = true;
    updateToolbar(true);

    statusLbl.innerHTML = 'Deleting...';

    var lengthIndex = dataTable.getRowProperty(selection.row, 'lengthIndex');
    selection = null;
    var query = new google.visualization.Query(
        'ajax-googlechart.php?action=' + EDITOR_ACTION_DELETE + '&lengthIndex=' + lengthIndex);
    query.send(handleQueryResponse);
  }
}


function handleQueryResponse(response) {
  if (response.isError()) {
    statusLbl.innerHTML = 'Unexpected error has been logged, '
      + '<a href="contact">Contact developer</a> or '
      + '<a href="upload">Upload</a> new file'
    statusLbl.className = 'warning';
  } else {
    dataTable = response.getDataTable();

    distanceLbl.innerHTML = dataTable.getTableProperty('distance');
    swimTimeLbl.innerHTML = dataTable.getTableProperty('swimTime');
    avgPaceLbl.innerHTML = dataTable.getTableProperty('avgPace');
    avgStrokesLbl.innerHTML = dataTable.getTableProperty('avgStrokes');

    var numberOfRows = dataTable.getNumberOfRows();

    var ticks = new Array();
    for (var i = 0; i < Math.floor(numberOfRows / 2); i++) ticks[i] = (i + 1) * 2;
    options.hAxis.ticks = ticks;

    if (numberOfRows <= 20) {
      options.width = 800;
      options.height = 400;
      options.chartArea.width = 600;
      options.chartArea.height = 300;
      document.getElementById('chartContainer').style.overflow = "hidden";
    } else {
      options.width = numberOfRows * 30 + 200;
      options.height = 375;
      options.chartArea.width = numberOfRows * 30;
      options.chartArea.height = 275;
      document.getElementById('chartContainer').style.overflow = "auto";
    }

    chart.draw(dataTable, options);

    if (selection != null) {
      chart.setSelection([{ row: selection.row}]);
    }

    statusLbl.className = '';
    statusLbl.innerHTML = dataTable.getTableProperty('status');
    isProcessing = false;
    updateToolbar(false);
  }
}

function updateToolbar(disabled) {
  if (selection == null || disabled == true || isProcessing == true)     {
    mergeBtn.disabled = true;
    splitBtn.disabled = true;
    changeStrokeBtn.disabled = true;
    deleteBtn.disabled = true;
  } else {
    mergeBtn.disabled = !dataTable.getRowProperty(selection.row, 'canMerge');
    splitBtn.disabled = !dataTable.getRowProperty(selection.row, 'canEdit');
    changeStrokeBtn.disabled = !dataTable.getRowProperty(selection.row, 'canEdit');
    deleteBtn.disabled = !dataTable.getRowProperty(selection.row, 'canEdit');
  }
}

function chart_onSelect() {
  if (chart.getSelection()[0] == null) {
    selection = null;
    updateToolbar(false);
  } else {
    if (chart.getSelection()[0].row != null) {
      selection = chart.getSelection()[0];
      updateToolbar(false);
    } else {
      selection = null;
      updateToolbar(false);
    }
  }
}

function showDownloadPopup(event) {
  
  var popup = document.getElementById('downloadPopUp');
  popup.style.left = event.clientX + 'px';
  popup.style.top = event.clientY + 'px';;
  popup.style.display = 'block';

}
function hideDownloadPopup() {

  var popup = document.getElementById('downloadPopUp');
  popup.style.display = 'none';
}

google.setOnLoadCallback(initialize);
