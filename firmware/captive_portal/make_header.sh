#!/bin/bash

# Parameter:
# $1: Quellverzeichnis (wo index.html.gz liegt)
# $2: Ausgabedateiname
# $3: C++ Namespace
# $4: (Optional) Versionsnummer
# $5: (Optional) Ausgabepfad

# Ausgabepfad bestimmen
if [ -n "$5" ]; then
  OUTPUT_DIR="$5"
else
  OUTPUT_DIR="$1"
fi

# Ausgabeverzeichnis erstellen falls nicht vorhanden
mkdir -p "$OUTPUT_DIR"

OUTPUT_FILE="$OUTPUT_DIR/$2"

cat <<EOT >$OUTPUT_FILE
#pragma once
// Generated from https://github.com/esphome/esphome-webserver

EOT

if [ -n "$4" ]; then
  echo "#ifdef USE_WEBSERVER_LOCAL" >>$OUTPUT_FILE
  echo "#if USE_WEBSERVER_VERSION == $4" >>$OUTPUT_FILE
  echo "" >>$OUTPUT_FILE
fi

cat <<EOT >>$OUTPUT_FILE
#include "esphome/core/hal.h"

namespace esphome::$3 {

EOT
echo "const uint8_t INDEX_GZ[] PROGMEM = {" >>$OUTPUT_FILE
xxd -cols 19 -i $1/index.html.gz | sed -e '2,$!d' -e 's/^/  /' -e '$d' | sed -e '$d' | sed -e '$s/$/};/' >>$OUTPUT_FILE
cat <<EOT >>$OUTPUT_FILE

}  // namespace esphome::$3
EOT
if [ -n "$4" ]; then
  echo "" >>$OUTPUT_FILE
  echo "#endif" >>$OUTPUT_FILE
  echo "#endif" >>$OUTPUT_FILE
fi