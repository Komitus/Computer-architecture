#!bin/bash
wget -q http://thecatapi.com/api/images/get?format=xml --output-document=catXML.xml

caturl=$( cat catXML.xml | grep '<url>'| sed -e "s/<url>//" |  sed -e "s/<\/url>//" )

#echo $caturl
extension=$( echo $caturl | sed -e 's/.*\.//g')

file="kotek.$extension"
#echo $file

wget -q -O $file $caturl

catimg $file
echo $(curl -s http://api.icndb.com/jokes/random | jq .value.joke)
