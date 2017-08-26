#!env bash
echo "window.rawLog = \`" > rawLog.js
cat current.log >> rawLog.js
echo "\`;" >> rawLog.js
