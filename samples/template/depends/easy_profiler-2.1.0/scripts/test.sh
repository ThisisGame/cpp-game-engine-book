#!/bin/bash

unamestr=`uname`
SUBDIR="./bin"
if [[ ! "$unamestr" == 'Linux' ]]; then
    SUBDIR="./bin/Release/"
fi

DISABLED_PROF=$SUBDIR/profiler_sample_disabled_profiler
ENABLED_PROF=$SUBDIR/profiler_sample

TEMP_FILE_ENABLE="enable.info"
TEMP_FILE_DISABLE="disable.info"
RESULT_FILE="result.csv"
RESULT_FILE_TMP="result.csv.tmp"

HEADER="Blocks count, dT prof enabled usec, dT prof disabled usec,delta, usec/block"

#echo "Blocks count, dT prof enabled usec, dT prof disabled usec,delta, usec/block" > $RESULT_FILE

rm -rf $RESULT_FILE

for i in {1..9} 
do 
    OBJECTS_COUNT=$(($i*100))
    for j in {10..15} 
    do 
        RENDER_COUNT=$(($j*100))
        for k in {10..15} 
        do 
            MODELLING_COUNT=$(($k*100))
            $ENABLED_PROF $OBJECTS_COUNT $RENDER_COUNT $MODELLING_COUNT > $TEMP_FILE_ENABLE
            $DISABLED_PROF $OBJECTS_COUNT $RENDER_COUNT $MODELLING_COUNT > $TEMP_FILE_DISABLE
            DT_ENA=`cat $TEMP_FILE_ENABLE | grep Elapsed| awk '{print $3}'`
            N_ENA=`cat $TEMP_FILE_ENABLE | grep Blocks| awk '{print $3}'`
            N_DIS=`cat $TEMP_FILE_DISABLE | grep Elapsed| awk '{print $3}'`
            
            DELTA=$(($DT_ENA-$N_DIS))
            USEC_BLOCK=`awk "BEGIN{print $DELTA/$N_ENA}"`
            
            echo $N_ENA,$DT_ENA,$N_DIS,$DELTA,$USEC_BLOCK >> $RESULT_FILE
        done
    done
        echo $i

done

cat $RESULT_FILE | sort > $RESULT_FILE_TMP

echo $HEADER > $RESULT_FILE
cat $RESULT_FILE_TMP >> $RESULT_FILE

rm -rf $TEMP_FILE_ENABLE
rm -rf $TEMP_FILE_DISABLE
rm -rf $RESULT_FILE_TMP

echo "See result in $RESULT_FILE"
