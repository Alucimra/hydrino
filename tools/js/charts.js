
  var $chart, chart;

  function clearChart(){
    $chart.find('svg').children().remove();
  }

  function scatterData(wrap){
    wrap || (wrap = 0);

    var scatterData = [];

    $.each(parsedData.totalTime, function(i, val){
      if(wrap > 0){ val = val % wrap; }
      if(parsedData.cycleTime[i] % 2 == 0 || parsedData.cycleTime[i] == 1){
        // even value for cycleTime or 1 means the cycle is on (expectation)
        scatterData.push({x: val, y: parsedData.volts[i], r: parsedData.cycleTime[i], color: 0});
      } else {
        // odd cycle, so it's off (expectation)
        scatterData.push({x: val, y: parsedData.volts[i], r: parsedData.cycleTime[i], color: 1});
      }
    });

    return scatterData;
  }

  function charge_remaining_scatter(wrap){
    parseData();
    clearChart();

    var margin = { top: 30, right: 50, bottom: 40, left: 50 };
    var width = Math.floor($chart.width()) - margin.left - margin.right;
    var height = Math.floor($chart.height()) - margin.top - margin.bottom;
    var data = scatterData(wrap);

    var svg = d3.select("#chart svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
      .append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var xscale = d3.scaleLinear()
      .domain(d3.extent(data, function(i){ return i.x; }))
      .nice()
      .range([0,width]);

    var yscale = d3.scaleLinear()
      .domain(d3.extent(data, function(i){ return i.y; }))
      .nice()
      .range([height,0]);

    var radius = d3.scaleLinear()
      .domain(d3.extent(data, function(i){ return i.r; }))
      .range([1,6]);

    var xAxis = d3.axisBottom()
      .tickSize(-height)
      .scale(xscale);

    var yAxis = d3.axisLeft()
      .tickSize(-width)
      .scale(yscale);

    var color = d3.schemeCategory10;
    var colorMap = ['On', 'Off'];

    //data.sort(function(a,b) { return b.r - a.r; });

    svg.append("g")
      .attr("transform", "translate(0," + height + ")")
      .attr("class", "x axis")
      .call(xAxis);

    svg.append("g")
      .attr("transform", "translate(0,0)")
      .attr("class", "y axis")
      .call(yAxis);

    var group = svg.selectAll("g.bubble")
      .data(data)
      .enter().append("g")
      .attr("class", "bubble")
      .attr("transform", function(d) {
        return "translate(" + xscale(d.x) + "," + yscale(d.y) + ")"
      });

    group
      .append("circle")
      .attr("r", function(d) { return radius(d.r);  }) // radius(d.r)
      .style('opacity', 0.7)
      .style("fill", function(d) {
        return color[d.color];
      })

    svg.append("text")
      .attr("x", 6)
      .attr("y", -2)
      .attr("class", "label")
      .text("Voltage over Time");

    svg.append("text")
      .attr("x", width-2)
      .attr("y", height-6)
      .attr("text-anchor", "end")
      .attr("class", "label")
      .text("Total Time");

    var legend = svg.selectAll(".legend")
        .data(colorMap)
      .enter().append("g")
        .attr("class", "legend")
        .attr("transform", function(d, i) { return "translate(2," + i * 14 + ")"; });

    legend.append("rect")
        .attr("x", width)
        .attr("width", 12)
        .attr("height", 12)
        .style("fill", function(x, i){ return color[i]; });

    legend.append("text")
        .attr("x", width + 16)
        .attr("y", 6)
        .attr("dy", ".35em")
        .style("text-anchor", "start")
        .text(function(d) { return d; });

    legend.on("mouseover", function(type, i) {
        d3.selectAll(".legend")
          .style("opacity", 0.1);
        d3.select(this)
          .style("opacity", 1);
        d3.selectAll(".bubble")
          .style("opacity", 0.1)
          .filter(function(d) { return d.color == i; })
          .style("opacity", 1);
      })
      .on("mouseout", function(type) {
        d3.selectAll(".legend")
          .style("opacity", 1);
        d3.selectAll(".bubble")
          .style("opacity", 1);
      });
/*
    // http://nvd3.org/examples/scatter.html
    var chart = nv.models.scatterChart()
                  .showDistX(false)
                  .showDistY(false)
                  .color(d3.scale.category10().range());

    chart.xAxis.tickFormat(d3.format('.3s'));
    chart.yAxis.tickFormat(d3.format('.2f'));

    d3.select('#chart svg').datum(scatterData(wrap)).call(chart);
    nv.utils.windowResize(chart.update);

*/

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
