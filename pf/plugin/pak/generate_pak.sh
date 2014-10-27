#!/bin/bash
#the generate pak script
#@version 1.0
#@author viticm<viticm.ti@gmail.com>
#@date 2014-10-27

pak_tool="./pak_tool"
NEW_PAK_HASHTABLE_SIZE=65536

#help text, the script desc
#@param void
#@return void
function help_text() {
  cat <<EOF
${0} ver 1.0

options:
--name="test.pak" the name which you want to the pak)
--path="./"       the path which you want to the pak)
EOF
exit 0
}

#print error message, red words
#@param string message
#@return void
function error_message() {
  local message=${@}
  echo -e "\e[0;31;1merror: ${message}\e[0m"
  exit 1
}

#print warning message, yellow words
#@param message
#@return void
function warning_message() {
  local message=${@}
  echo -e "\e[0;33;1mwarning: ${message}\e[0m"
}

#format the path
#@param path
#@return void
function format_path() {
  local path=${1}
  local _path=`echo $path | sed -e 's;../;;g'`
  if [[ $path != $_path ]] ; then
    echo $path
  else 
    echo $path | sed -e 's;./;;g'
  fi
}

#generate pak file
#@param name
#@param path
#@return void
function generate_pak() {
  local name=${1}
  local path=${2}
  if [ ! -x $name ] ; then
    $pak_tool create $name $NEW_PAK_HASHTABLE_SIZE patch
  fi
  if [ ! -d $path ] ; then
    error_message "the path: ${path} not exists"
  fi
  files=`find $path -type f`
  for file in $files 
  do
    file=`format_path $file`
    echo $pak_tool add $name $file
    $pak_tool add $name $file
    if [[ $? != 0 ]] ; then
      error_message "$pak_tool add $name $file error"
    fi
  done
}

#the script main function, like c/c++
function main() {
  for arg do
    case "${arg}" in
      --help) help_text;;
      -h) help_text;;
      --name=*) name=`echo ${arg} | sed -e "s;--name=;;"`;;
      --path=*) path=`echo ${arg} | sed -e "s;--path=;;"`;;
      *) error_message "unkown option ${arg}";;
    esac
  done
  if [[ "" == $name || "" == $path ]] ; then
    error_message "--name and --path will set"
  fi
  generate_pak $name $path
}
if [[ "" == ${@} ]] ; then
  error_message "${0}: no commond specified.You can use <${0} --help> 
                get parameters for this script."
else
  main "${@}"
  exit 0
fi
