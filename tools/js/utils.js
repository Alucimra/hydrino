var parsedData = {};

function parseConfig(logs, matcher){
  var config;
  var match;
  try {
    if(match = logs.match(matcher)){
      config = JSON.parse(match[1]);
    } else {
      config = null;
    }
  } catch(e){
    console.log(e);
  }
  return config;
}

function parseData(){
  if(parsedData.parsedAt){ return; }

  parsedData.totalTime = [];
  parsedData.power = [];
  parsedData.volts = [];
  parsedData.cycleTime = [];
  parsedData.cycles = [];
  var time = 0;
  var logs = ($('#logs').val().length == 0 ? window.rawLog : $('#logs').val());
  // printing these fields cost >4kb of program memory space!
  // have to figure out how to make use of it...
  parsedData.voltageLevels = parseConfig(logs, /:: Voltage Levels :> ({.+})/);
  parsedData.motorTimings = parseConfig(logs, /:: Motor Timing :> ({.+})/);
  parsedData.motorPowerLevels = parseConfig(logs, /:: Motor Power Levels :> ({.+})/);
  parsedData.config = parseConfig(logs, /:: Config :> ({.+})/);
  parsedData.driveConfig = parseConfig(logs, /:: Drive Config :> ({.+})/);
  parsedData.reservedBytes = parseConfig(logs, /:: Reserved Bytes :> ({.+})/);

  var x = logs.split(":) Reading leftovers");
  if(x[1]){
    logs = x[1].replace(/^[:=].+\n/gm, '') +  x[0].replace(/^[:=].+\n/gm, '');
  } else {
    logs = logs.replace(/^[:=].+\n/gm, '');
  }

  // TODO: add more checks, the first line should be an odd number


  $.each(logs.split("\n"), function(i, line){
    var data = line.split("\t");
    if(line.length == 0 || line[0] == ':' || data.length != 3){ return; }
    let pos = Number(data[0]);
    let power_or_cycle = Number(data[1]);
    let volts_or_cycleTime = Number(data[2]);

    /**
     * FIXME: power should never be 0...but if this is 0, then the next col will be 0,
     *        so it's handled by the return check below.
     */
    // if(col1 === 0 && pos % 2 == 1){ return; }

    // neither cycle_time nor voltage will ever equal 0
    if(volts_or_cycleTime === 0 || volts_or_cycleTime === NaN){ return; }

    if(pos % 2 == 1){
      // odd, read voltage
      parsedData.power.push(power_or_cycle);
      parsedData.volts.push(volts_or_cycleTime);
    } else {
      // even, read time
      parsedData.cycleTime.push(volts_or_cycleTime);
      parsedData.totalTime.push(time);
      parsedData.cycles.push(power_or_cycle);
      time += volts_or_cycleTime;
    }
  });
  parsedData.parsedAt = new Date();

  $('#x').val('time <- c(' + parsedData.totalTime.join(',') + ')');
  $('#y').val('volts <- c(' + parsedData.volts.join(',') + ')');
}
