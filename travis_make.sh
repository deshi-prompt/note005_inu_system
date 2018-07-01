#~/bin/bash 

set -ev

#ext=`echo $1 | sed -e "s/^.\+\.//g"`

root=./build/index.adoc
ext=adoc

echo $ext
eno=0

if [ "$ext" = "adoc" ]; then

    bash preprocess.sh

    #a2x -f pdf -d book --fop ${root} --fop-opts='-c /usr/share/fop/conf/fop.xconf' --xsl-file=/usr/share/fop/conf/fop-VL-PGothic.xsl 
    #a2x -f pdf -d book --fop ${root} --fop-opts='-c /usr/share/fop/conf/fop.xconf' --xsl-file=/usr/share/fop/conf/fop-IPAPMincho.xsl 
    #a2x -f pdf -d book --fop ${root} --fop-opts='-c /usr/share/fop/conf/fop.xconf' --xsl-file=/usr/share/fop/conf/fop-VL-PGothic.xsl 
    
    #asciidoctor -b docbook -d article -a data-uri! ${root}
    #asciidoctor -b docbook -d article -a lang=en -a icons=font  ${root}
    #asciidoctor -b docbook5 -a lang=en -a icons=font  ${root}
    #asciidoctor -a lang=en -a bookversion=`node ./bin/bookversion` -a icons=font -b docbook -o #{output} #{src}
    #asciidoctor -a lang=en -a bookversion="2.2.1" -a icons=font -b docbook -d book -o build/steins-git-book.xml ${root}
    #asciidoctor -a lang=en -a bookversion=`node ./bin/bookversion`   -a icons=font -b docbook   -o build/steins-git-book.xml book.adoc
    
    #asciidoctor -a lang=ja -a bookversion=1.0.0   -a icons=font -b epub3 ${root}
    #asciidoctor -a lang=ja -a bookversion=1.0.0   -a icons=font -b docbook -d book ${root}
    
    asciidoctor -a lang=ja -a bookversion=1.0.0   -a icons=font -b docbook ${root}
    eno=$?
fi

if [ $eno -eq 0 ]; then
    output=`echo ${root} | sed -e "s/.adoc$/.pdf/g"`
    xml=`echo ${root} | sed -e "s/.adoc$/.xml/g"`

    git clone -b v1.0.0_C91 https://github.com/deshi-prompt/adoc_styles.git styles

    wget http://blueskis.wktk.so/AozoraMincho/archive/v0.1/aozoramincho-readme-ttf.zip
    unzip aozoramincho-readme-ttf.zip

    wget https://osdn.jp/downloads/users/8/8637/genshingothic-20150607.zip
    mkdir genshin
    cd genshin
    unzip ../genshingothic-20150607.zip
    cd ..

    #fopub $xml \
    #    -param title.font.family VL-PGothic-Regular \
    #    -param body.font.family VL-PGothic-Regular \
    #    -param sans.font.family VL-PGothic-Regular \
    #    -param monospace.font.family VL-PGothic-Regular \
    #    -param symbol.font.family VL-PGothic-Regular

    #fopub $xml \
    #    -param title.font.family IPAPMincho \
    #    -param body.font.family IPAPMincho \
    #    -param sans.font.family IPAPMincho \
    #    -param monospace.font.family IPAPMincho \
    #    -param symbol.font.family IPAPMincho

    #fopub $xml \
    #fopub -t sstyles $xml \
    #fopub -t test/steins-git/steins-git.styles $xml \
    #    -param body.font.family AozoraMinchoRegular \
    #    -param sans.font.family AozoraMinchoRegular \
    #    -param body.bold.font.family GenShinGothic-P-Heavy \
    ./asciidoctor-fopub/fopub -t styles $xml \
        -cache aozoramincho-readme-ttf.zip \
        -cache genshingothic-20150607 \
        -param body.font.family AozoraMinchoRegular \
        -param dingbat.font.family GenShinGothic-P-Normal \
        -param monospace.font.family GenShinGothic-P-Normal \
        -param sans.font.family AozoraMinchoRegular \
        -param title.font.family GenShinGothic-P-Heavy \
        -param alignment left

    eno=$?
    echo error w/ $eno
    exit $eno
    #gnome-open $output
else
    echo error w/ $eno
    exit $eno
fi

exit -1
