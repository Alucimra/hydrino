#!env bash
echo "window.rawLog = '" > rawLog.js
cat putty.log >> rawLog.js
echo "';" >> rawLog.js
