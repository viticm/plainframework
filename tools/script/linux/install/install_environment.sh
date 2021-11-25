#!/bin/bash

current_user=`whoami`
systembit=`getconf LONG_BIT`
current_dir=`pwd`
sudo_str=""

#help text, the script desc
#@param void
#@return void
function help_text() {
  cat <<EOF
${0} ver 1.0

options:
--odbc          install unix odbc in your system
--mysqldriver   install mysql odbc driver in your system
--help          view the text, or just use -h
--full          intall unix odbc and mysql odbc driver
EOF
}

#print error message, red words
#@param string message
#@return void
function error_message() {
  local message=${1}
  echo -e "\e[0;31;1merror: ${message}\e[0m"
  exit 1
}

#print warning message, yellow words
#@param message
#@return string void
function warning_message() {
  local message=${1}
  echo -e "\e[0;33;1mwarning: ${message}\e[0m"
}

#print success message, yellow words
#@param message
#@return string void
function success_message() {
  local message=${1}
  echo -e "\e[0;32;1msuccess: ${message}\e[0m"
}

function check_privileges() {
  if [[ $current_user != "root" ]] ; then
    error_message "run this script need root privileges"
  fi
}

#install unix odb
#@param void
#@return void
function install_odbc() {
  # build 32
  # CC="gcc -m32" ./configure --target="i386-linux" --prefix=/usr/local/unixODBC32 --sysconfdir=/etc
  cd $current_dir
  local version="2.3.9"
  if [[ ! -f ./unixODBC-${version}.tar.gz ]] ; then
    wget -c ftp://ftp.unixodbc.org/pub/unixODBC/unixODBC-${version}.tar.gz
  fi
  [[ -d ./unixODBC-${version} ]] && rm -rf unixODBC-${version}
  tar -xzvf unixODBC-${version}.tar.gz
  cd unixODBC-${version}
  ./configure --prefix=/usr/local/unixODBC --sysconfdir=/etc
  make && $sudo_str make install
  if [[ 64 == $systembit ]] ; then
    $sudo_str cp /usr/local/unixODBC/lib/libodbc* /lib64/
  else
    $sudo_str cp /usr/local/unixODBC/lib/libodbc* /lib/
  fi
  [[ 0 == $? ]] && success_message "install unix odbc[$version] complete"
}

#install mysql odbc driver
#@param void
#@return void
function install_mysql_driver() {
  cd $current_dir
  local driver_so="libmyodbc5a.so"
  local odbc_ini="odbc.ini"
  local driver_dir="/usr/local/lib/mysqlodbc/"
  [[ ! -f ./${driver_so} ]] &&
    error_message "mysql driver[$driver_so] not found"
  [[ ! -f ./${odbc_ini} ]] && warning_message "${odbc_ini} not found"
  [[ ! -d $driver_dir ]] && $sudo_str mkdir -p $driver_dir
  $sudo_str cp ./${driver_so} $driver_dir
  [[ -f ./${odbc_ini} ]] && $sudo_str cp ./${odbc_ini} /etc
  [[ 0 == $? ]] && success_message "install mysql odbc driver complete"
}

#the script main function, like c/c++
function main() {
  cmd=${1}
  cmd1=${2}
  if [[ "--sudo" == $cmd1 ]] ; then
    sudo_str="sudo"
  else
    check_privileges
  fi
  case "${cmd}" in
    --help ) help_text;;
    -h) help_text;;
    --odbc) install_odbc;;
    --mysqldriver) install_mysql_driver;;
    --full) install_odbc && install_mysql_driver;;
  esac
}
if [[ "" == ${@} ]] ; then
  error_message "${0}: no commond specified.You can use <${0} --help> "
                "get parameters for this script."
else
  main "${@}"
  exit 0
fi
