#!env bash
shopt -s nullglob


TOOLS_DIR=$(realpath $(dirname "$0"));
LOGS_DIR=$(realpath "$TOOLS_DIR/../logs");

FILES_LIST=($LOGS_DIR/*.log);
i=0;
for file in ${FILES_LIST[@]}
do
  echo $i $(basename "$file") $(stat --printf="%s" $file);
  ((i++))
done

read -p 'Which File? ' selectedLog

SELECTED_FILE=${FILES_LIST[$selectedLog]};

echo Using $SELECTED_FILE;

# NOTE: This uses ES6's string template literal (the grave quotation mark)
echo "window.rawLog = \`" > $TOOLS_DIR/rawLog.js
cat $SELECTED_FILE >> $TOOLS_DIR/rawLog.js
echo "\`;" >> $TOOLS_DIR/rawLog.js

echo "Done. rawLog.js size: " $(stat --printf="%s" $TOOLS_DIR/rawLog.js)
