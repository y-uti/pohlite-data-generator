#!/bin/bash

BASEDIR=$(cd $(dirname $0) && pwd)
DATADIR=$BASEDIR/sample_data

PROG=$BASEDIR/datagen.php

# 会社数 (n)
for n in 50 100 200 300 500 1000 2000 3000 5000 10000; do

    # 人員数   (qi) : 1 ～ 10,000 の一様分布
    # 費用     (ri) : 1 ～ 500 の一様分布 * 人員数
    # 必要人数 (m)  : n = 50 のときに 200,000 として n に比例
    m=$(($n / 50 * 200000))
    datafile=$DATADIR/data_uu_$(printf "%05d" $n).txt
    php $PROG -m $m -n $n -q u:1:10000 -r u:1:500 >$datafile

    # 人員数   (qi) : 1 ～ 10,000 の一様分布
    # 費用     (ri) : 平均 100, 標準偏差 10 の正規分布 * 人員数
    # 必要人数 (m)  : n = 50 のときに 200,000 として n に比例
    m=$(($n / 50 * 200000))
    datafile=$DATADIR/data_un_$(printf "%05d" $n).txt
    php $PROG -m $m -n $n -q u:1:10000 -r n:100:10 >$datafile

    # 人員数   (qi) : 平均 10 のポアソン分布
    # 費用     (ri) : 平均 100, 標準偏差 10 の正規分布 * 人員数
    # 必要人数 (m)  : n * 平均人員数 / 2
    m=$(($n * 10 / 2))
    datafile=$DATADIR/data_pn_$(printf "%05d" $n).txt
    php $PROG -m $m -n $n -q p:10 -r n:100:10 >$datafile

done
