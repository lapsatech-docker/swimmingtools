google.load('visualization', '1', {packages: ['corechart', 'controls']});

var chart;
var dataTable;
var selection;
var isProcessing;
var undoAllBtn;
var mergeBtn;
var splitBtn;
var changeStrokeBtn;
var changePoolLengthBtn;
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
  legend: {position: 'none'}
//  tooltip: {isHtml: true },
//  focusTarget: 'category'
};

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
  changePoolLengthBtn = document.getElementById('changePoolLengthBtn');
  changePoolLengthBtn.onclick = changePoolLengthBtn_onClick;
  deleteBtn = document.getElementById('deleteBtn');
  deleteBtn.onclick = deleteBtn_onClick;
  document.changePoolLengthForm.onsubmit = changePoolLengthForm_onSubmit;
  document.getElementById('changePoolLengthDlgCancelBtn').onclick = ChangePoolLengthDlgCancelBtn_onclick;

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

  var query = new google.visualization.Query('ajax-googlechart.php?action=undoAll');
  query.send(handleQueryResponse);
}


function mergeBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = 'Merging...';

  var messageIndex = dataTable.getRowProperty(selection.row, 'messageIndex');

  var query = new google.visualization.Query('ajax-googlechart.php?action=merge&messageIndex=' + messageIndex);
  query.send(handleQueryResponse);
}


function splitBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = 'Splitting...';

  var messageIndex = dataTable.getRowProperty(selection.row, 'messageIndex');

  var query = new google.visualization.Query('ajax-googlechart.php?action=split&messageIndex=' + messageIndex);
  query.send(handleQueryResponse);
}

function changeStrokeBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = "Changing Stroke";

  document.changeStrokeForm.reset();
  selectedLengthLbl.innerHTML = selection.row + 1;
  selectedLengthStrokeLbl.innerHTML = dataTable.getRowProperty(selection.row, 'swimStroke');
  document.changeStrokeForm.fromSwimStroke.value = dataTable.getRowProperty(selection.row, 'swimStroke');
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
  var strokeOption;

  for (var i = 0; i < document.changeStrokeForm.strokeOption.length; i++) {
    if (document.changeStrokeForm.strokeOption[i].checked) {
      strokeOption = document.changeStrokeForm.strokeOption[i].value;
      break;
    }
  }

  var fromSwimStroke = document.changeStrokeForm.fromSwimStroke.value;
  var toSwimStroke = document.changeStrokeForm.toSwimStroke.options[document.changeStrokeForm.toSwimStroke.selectedIndex].value;
  var messageIndex = dataTable.getRowProperty(selection.row, 'messageIndex');

  var query = new google.visualization.Query(
      'ajax-googlechart.php?action=changeStroke&messageIndex=' + messageIndex 
      + '&fromSwimStroke=' + fromSwimStroke 
      + '&toSwimStroke=' + toSwimStroke 
      + '&strokeOption=' + strokeOption);
  query.send(handleQueryResponse);
  document.getElementById('changeStrokeDlg').style.display = 'none';
  return false;
}

function changePoolLengthBtn_onClick() {
  isProcessing = true;
  updateToolbar(true);
  statusLbl.innerHTML = "Changing pool length...";

  document.changePoolLengthForm.reset();
  document.getElementById('changePoolLengthDlgErrorLbl').style.display = 'none';
  document.changePoolLengthForm.newPoolLength.value = dataTable.getTableProperty('poolLength');
  if (dataTable.getTableProperty('poolLengthUnits') == 'y')
    document.getElementById('statuteRdo').checked = true;
  document.getElementById('changePoolLengthDlg').style.display = 'block';
}

function ChangePoolLengthDlgCancelBtn_onclick() {
  document.getElementById('changePoolLengthDlg').style.display = 'none';
  statusLbl.innerHTML = "Changing Pool Size cancelled";
  isProcessing = false;
  updateToolbar(false);
}

function changePoolLengthForm_onSubmit() {
  statusLbl.innerHTML = 'Changing Pool Size...';

  var poolLengthUnits;

  for (var i = 0; i < document.changePoolLengthForm.poolLengthUnits.length; i++) {
    if (document.changePoolLengthForm.poolLengthUnits[i].checked) {
      poolLengthUnits = document.changePoolLengthForm.poolLengthUnits[i].value;
      break;
    }
  }

  var newPoolLength = document.changePoolLengthForm.newPoolLength.value;
  var error = false;

  if (isNaN(parseFloat(newPoolLength))) {
    error = true;
  } else if (poolLengthUnits == 0) {
    if (!(newPoolLength >= 17 && newPoolLength <= 150))
      error = true;
  } else if (poolLengthUnits == 1) {
    if (!(newPoolLength >= 18 && newPoolLength <= 150))
      error = true;
  }

  if (error) {
    document.getElementById('changePoolLengthDlgErrorLbl').style.display = 'block';
  } else {
    var query = new google.visualization.Query(
        'ajax-googlechart.php?action=changePoolLength&newPoolLength=' + newPoolLength 
        + '&poolLengthUnits=' + poolLengthUnits);
    query.send(handleQueryResponse);
    document.getElementById('changePoolLengthDlg').style.display = 'none';
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

  if (r || isDuplicate)     {
    isProcessing = true;
    updateToolbar(true);

    statusLbl.innerHTML = 'Deleting...';

    var messageIndex = dataTable.getRowProperty(selection.row, 'messageIndex');
    selection = null;
    var query = new google.visualization.Query(
        'ajax-googlechart.php?action=delete&messageIndex=' + messageIndex);
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

google.setOnLoadCallback(initialize);
