#! /bin/sh

pwd=Liu8440
host=127.0.0.1
port=8011
path="`pwd`/log"
httpaddr=http://127.0.0.1:8002/hook/on_sip_record
./build/fsEventController -a $host -p $port -w $pwd -u $httpaddr -d $path
