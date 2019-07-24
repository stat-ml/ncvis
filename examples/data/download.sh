#!/bin/bash

path=$(git rev-parse --show-toplevel)
cd $path/examples/data

# https://gist.github.com/iamtekeste/3cdfd0366ebfd2c0d805#gistcomment-2359248
gdrive_download() {
    # $1 = GDrive id of the file
    # $2 = location to save file
    CONFIRM=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate "https://docs.google.com/uc?export=download&id=$1" -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')
    wget --load-cookies /tmp/cookies.txt "https://docs.google.com/uc?export=download&confirm=$CONFIRM&id=$1" -O $2
    rm -rf /tmp/cookies.txt
}

download() {
    # $1 = folder name
    # $2 = url1, $3 = url2, ... 
    local dir=$1
    while [ -n "$2" ]; do
        wget -N -P $dir $2
        shift
    done
}

extract() {
    # $1 = folder name
    for f in $1/*; do
        case $f in 
            *.zip) yes N | unzip -d $1 -qq $f ;;
            *.gz)  yes n | gzip -dk $f ;;
            *) echo "$f is not compressed"
        esac
    done
}

get_data() {
    # $1 = folder name
    # $2 = url1, $3 = url2, ... 
    download "$@"
    extract $1
}

init_data() {
    case $1 in
        iris)
            # Iris Dataset
            # https://archive.ics.uci.edu/ml/datasets/Iris
            get_data iris http://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data 
            ;;
        mnist)   
            # MNIST Dataset 
            # http://yann.lecun.com/exdb/mnist/
            get_data mnist http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz 
            ;;
        pendigits)
            # Pen digits
            # https://archive.ics.uci.edu/ml/datasets/optical+recognition+of+handwritten+digits
            get_data pendigits https://archive.ics.uci.edu/ml/machine-learning-databases/optdigits/optdigits.tra https://archive.ics.uci.edu/ml/machine-learning-databases/optdigits/optdigits.tes
            ;;
        coil20)
            # COIL-20
            # http://www.cs.columbia.edu/CAVE/software/softlib/coil-20.php
            get_data coil20 http://www.cs.columbia.edu/CAVE/databases/SLAM_coil-20_coil-100/coil-20/coil-20-proc.zip
            ;;
        coil100)
            # COIL-100
            # http://www1.cs.columbia.edu/CAVE/software/softlib/coil-100.php
            get_data coil100 http://www.cs.columbia.edu/CAVE/databases/SLAM_coil-20_coil-100/coil-100/coil-100.zip
            ;;
        fmnist)
            # Fashion-MNIST
            # https://github.com/zalandoresearch/fashion-mnist
            get_data fmnist https://github.com/zalandoresearch/fashion-mnist/raw/master/data/fashion/t10k-images-idx3-ubyte.gz https://github.com/zalandoresearch/fashion-mnist/raw/master/data/fashion/t10k-labels-idx1-ubyte.gz https://github.com/zalandoresearch/fashion-mnist/raw/master/data/fashion/train-images-idx3-ubyte.gz https://github.com/zalandoresearch/fashion-mnist/raw/master/data/fashion/train-labels-idx1-ubyte.gz
            ;;
        scrna)
            # scRNA-seq
            # https://hemberg-lab.github.io/scRNA.seq.datasets/mouse/brain/
            get_data scrna ftp://ftp.ncbi.nlm.nih.gov/geo/series/GSE93nnn/GSE93374/suppl/GSE93374_Merged_all_020816_DGE.txt.gz ftp://ftp.ncbi.nlm.nih.gov/geo/series/GSE93nnn/GSE93374/suppl/GSE93374_cell_metadata.txt.gz
            ;;
        shuttle)
            # Statlog (Shuttle)
            # https://archive.ics.uci.edu/ml/datasets/Statlog+(Shuttle)
            get_data shuttle https://archive.ics.uci.edu/ml/machine-learning-databases/statlog/shuttle/shuttle.trn.Z https://archive.ics.uci.edu/ml/machine-learning-databases/statlog/shuttle/shuttle.tst
            ;;
        flow)
            # Flow cytometry
            # https://flowrepository.org/id/FR-FCM-ZZ36
            urls=(https://flowrepository.org/experiments/102/fcs_files/10672/download https://flowrepository.org/experiments/102/fcs_files/10673/download)
            names=(pbmc_luca_cd8.fcs pbmc_luca.fcs)
            local dir_name="flow/"
            for idx in ${!urls[*]}; do
                download ${dir_name} ${urls[idx]}
                mv ${dir_name}download ${dir_name}${names[idx]}
            done
            ;;
        news)
            # GoogleNews
            # https://code.google.com/archive/p/word2vec/
            mkdir -p news
            f="news/GoogleNews-vectors-negative300.bin.gz"
            if [[ -f "$f" ]]; then
                echo "$f is already present, nothing to do."
            else 
                gdrive_download 0B7XkCwpI5KDYNlNUTTlSS21pQmM $f
                yes n | gzip -dk $f
            fi
            ;;
    esac
}

if [ -z $1 ]; then
    all=(iris mnist pendigits coil20 coil100 fmnist scrna shuttle flow news)
else
    all=($1)
fi

for idx in ${!all[*]}; do
    echo "[$((idx+1))/${#all[*]}] Downloading: ${all[idx]}"
    init_data ${all[idx]}
done 