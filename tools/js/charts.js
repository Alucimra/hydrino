
  var $chart, chart;

  function clearChart(){
    $chart.find('svg').children().remove();
  }

  function scatterData(wrap){
    wrap || (wrap = 0);

    var scatterData = [],
        shapes = ['circle', 'square', 'triangle-up', 'triangle-down', 'diamond', 'cross'];
    scatterData.push({key: 'On Cycle', values: []});
    scatterData.push({key: 'Off Cycle', values: []});
    $.each(parsedData.totalTime, function(i, val){
      if(wrap > 0){ val = val % wrap; }
      if(parsedData.cycleTime[i] % 2 == 0 || parsedData.cycleTime[i] == 1){
        // even cycleTime or 1 means the cycle is on
        scatterData[0].values.push({x: val, y: parsedData.volts[i], size: parsedData.cycleTime[i], shape: 'circle'});
      } else {
        // odd cycle, so it's off
        scatterData[1].values.push({x: val, y: parsedData.volts[i], size: parsedData.cycleTime[i], shape: 'square'});
      }
    });

    return scatterData;
  }

  function charge_remaining_scatter(wrap){
    parseData();
    clearChart();
    // http://nvd3.org/examples/scatter.html
    var chart = nv.models.scatterChart()
                  .showDistX(false)
                  .showDistY(false)
                  .color(d3.scale.category10().range());

    chart.xAxis.tickFormat(d3.format('.3s'));
    chart.yAxis.tickFormat(d3.format('.2f'));

    d3.select('#chart svg').datum(scatterData(wrap)).call(chart);
    nv.utils.windowResize(chart.update);
    window.chart = chart;
  }

  function cycleData(wrap){
    wrap || (wrap = 0);
    var data = [{key: 'Cycle 0', values: []},
      {key: 'Cycle 1', values: []},
      {key: 'Cycle 2', values: []},
      {key: 'Cycle 3', values: []}];

    $.each(parsedData.cycles, function(i, val){
      data[val].values.push({x: Math.floor(i/4), y: parsedData.cycleTime[i]});
    });

    return data;
  }
