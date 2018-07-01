#!/bin/bash

ROOT_SRC=index.adoc

childs=`(find src | grep \.adoc$)`

adocs=(${ROOT_SRC} ${childs[@]})

mkdir -p ./build

for adoc in ${adocs[@]}; do

  echo ---- ${adoc}

  # image:http://aaa/bbb/ccc.jpg --> http://aaa/bbb/ccc.jpg
  # ^^^^^^
  urls=(`cat ${adoc}  | grep ^image:http | sed -e "s/^image://" | sed -e "s/\[.*\]$//g"`)
  # http://aaa/bbb/ccc.jpg --> http_img/aaa/bbb/ccc.jpg
  #     ^                          ^^^^
  lfiles=(`echo ${urls[@]} | xargs -n1 echo | sed -e "s%^http.*:/%http_img%g"`)
  # http_img//aaa/bbb/ccc.jpg --> http_img//aaa/bbb/
  #                   ^^^^^^^
  dirs=(`echo ${lfiles[@]} | xargs -n1 echo | sed -e "s%[^/]*\$%%g"`)
  
  echo -- urls
  echo ${urls[@]}
  echo -- lfiles
  echo ${lfiles[@]}
  echo -- dirs
  echo ${dirs[@]}
  
  for ((i = 0; i < ${#urls[@]}; i++)) do
      url=${urls[i]}
      echo "for images:${url}"
  
      #dir=images/`echo ${url} | sed -e "s%[^/]*\$%%g"`
      #dir=(`echo ${url} | sed -e "s%^http.*:/%http_img%g"`)
  
      dir=images/${dirs[i]}
  
      echo " -- mkdir ${dir}"
      mkdir -p ${dir}
  
      echo " -- wget -P ${dir} ${url}"
      #| xargs -n1 -i{} mkdir -p images/{}
      wget -q -P ${dir} ${url}
  
  done

  echo " ${adoc} --> ./build/${adoc}"
  # http_img//aaa/bbb/ccc.jpg --> http_img//aaa/bbb/
  #                   ^^^^^^^
  adoc_dir=(`echo ./build/${adoc} | sed -e "s%[^/]*\$%%g"`)
  mkdir -p ${adoc_dir}
  sed -e "s%^image:http.*:/%image:http_img%g" ${adoc} > ./build/${adoc}

  #if [ ${adoc} != "${ROOT_SRC}" ]; then
  #    echo " -- ${adoc} --> ${adoc}.adoc"
  #    sed -i -e "s%${adoc}%${adoc}.adoc%g" index.adoc.adoc
  #fi

done

cp -rf images ./build/
sed -i -e "s%^image:http.*:/%image:http_img%g"  ./build/${ROOT_SRC}
#sed -i -e "s%^include::src%include::build/src%g" ./build/${ROOT_SRC}
