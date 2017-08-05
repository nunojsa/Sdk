#!/bin/bash

RED='\033[0;31m'
WHITE='\033[1;37m'
BLUE='\033[0;34m'
NC='\033[0m'


function __include_all()
{
	local SRCS_DIR=${BUILD_ROOT}/src/
	local dir="";
	local subdir="";

	pushd ${SRCS_DIR} 1>/dev/null;
	
	for dir in $(find . -maxdepth 1 -mindepth 1 -type d -printf '%f ')
	do
		pushd ${dir} 1>/dev/null;
		for subdir in $(find . -maxdepth 1 -mindepth 1 -type d -printf '%f ')
		do
			touch ${subdir}/.detect;
		done
		popd 1>/dev/null;	
	done

	popd 1>/dev/null;
}

function __setup_common()
{
	#may be overriden if --outdir is given
	export OUT_DIR=${BUILD_ROOT}/out/;
}

function __setup_compiler()
{
	hash ${1}gcc 2>/dev/null || { echo "Make sure you have ${1} on your system!!!"; return 1; }

	local CROSS_COMPILER="${1}";

	export AR=${CROSS_COMPILER}ar
	export AS=${CROSS_COMPILER}as
	export LD=${CROSS_COMPILER}"ld --verbose"
	export CC=${CROSS_COMPILER}gcc
	export CXX=${CROSS_COMPILER}g++
	export CPP=${CROSS_COMPILER}cpp
	export NM=${CROSS_COMPILER}nm
	export STRIP=${CROSS_COMPILER}strip
	export SSTRIP=${CROSS_COMPILER}sstrip
	export OBJCOPY=${CROSS_COMPILER}objcopy
	export OBJDUMP=${CROSS_COMPILER}objdump
	export RANLIB=${CROSS_COMPILER}ranlib
	
	return 0;
}

function __setup_host()
{	
	#setup compiler
	__setup_compiler;
	[ $? == 1 ] && return 1;
	__setup_common;
	export TARGET_BUILD=HOST
}

function __check_ownerchip()
{
	[ $# -ne 1 ] && { echo "Invalid number of parameters in __check_ownerchip" ; return 1; }
	local owner=$(ls -ld "$1" | awk '{print $3}');
	
	if [ "${owner}" == "root" ]
	then
		local opt;
		echo -en "${BLUE}Owner of "$1" is the root user. Are you sure you want to use this path [y]? ${NC}"
		read opt;
		if [ "${opt}" == "y" ]
		then
			return 0;
		else
			return 1;
		fi
	fi
	
	return 0;
}

function __process_args()
{
	[ $# -ne 2 ] && { echo "Invalid number of parameters in __process_args" ; return; }
	
	case "${1}" in
		"--rootdir")
			[ ! -d "${2}" ] && {echo -e "${RED}[ERROR]: Invalid root dir="$2"", return };
			__check_ownerchip "${2}";		
			[ $? == 1 ] && return;
			export ROOT_DIR=$(realpath ${2})/;
		;;
		"--outdir")
			[ ! -d "${2}" ] && {echo -e "${RED}[ERROR]: Invalid out dir="$2"", return };
                        __check_ownerchip "${2}";
                        [ $? == 1 ] && return;
			export OUT_DIR=$(realpath ${2})/;		
		;;
		*)
			echo -e "${RED}[ERROR]: Invalid key=${1}${NC}";	
		;;
	esac	
}	

##based on aosp
##xargs 0 tells that the inputs are terminated by null char and not whitespace. Important because of -print0
function cgrep()
{
	find . -name .git -prune -o -type f \( -name '*.c' -o -name '*.h' -o -name '*.cpp' -o -name '*.cc' -o -name '*.hpp' \) -print0 | xargs -0 grep --colour -n "$@"
}

function mgrep()
{
	find . -name .git -prune -o -type f \( -name 'make.*' -o -name 'Makefile' \) -print0 | xargs -0 grep --colour -n "$@"
}
function gettop()
{
	local TOPFILE=scripts/env-setup.sh
	if [ -n "${BUILD_ROOT}" -a "${BUILD_ROOT}/${TOPFILE}" ]
	then
		##build_root should be the physical path and not a symlink
		echo "${BUILD_ROOT}";
	else
		if [ -f "${TOPFILE}" ]
		then
			PWD= /bin/pwd;
		else
			local HERE=$PWD;
			T="";
			while [ ! -f "${TOPFILE}" -a $PWD != "/" ]
			do
				cd ..;
				#-P to avoid symlinks
				T=`PWD= /bin/pwd -P`;
			done
			cd ${HERE};
			if [ -f "${T}/${TOPFILE}" ]
			then
				echo $T;
			fi
		fi
	fi	
}

function croot()
{
	local T=$(gettop)
	if [ -d ${T} ]
	then
		cd ${T};
	else
		echo "${WHITE}Couldn't locate the top of the tree. $T${NC}";
	fi	
}

function clean_env()
{
	export TARGET_BUILD=""
	export BUILD_ROOT=""
	export OUT_DIR=""
	export ROOT_DIR=""
	export SCRIPTS_DIR=""
	export AR=""
	export AS=""
	export LD=""
	export CC=""
	export CXX=""
	export CPP=""
	export NM=""
	export STRIP=""
	export SSTRIP=""
	export OBJCOPY=""
	export OBJDUMP=""
	export RANLIB=""
}

function help_me()
{
	echo "no help for you yet!"
}

#TODO: Add autocomplete functionality
function add_target_build()
{
	[ $# -ge 1 ] || { echo "[ERROR}: Invalid number of parameters"; return 1; }
	local arg="";
	local key="";
	local value="";
	#For now is assumed that the target is alwas the last argument
	local target='${'$#'}';
	eval target=${target};


	case "${target}" in 
	
		"HOST")
			__setup_host;
			[ $? == 1 ] && return 1;
		;;
		"RPI")
			echo "RPI is not yet supportted";
		;;	
		*)
			echo -e "${RED}[ERROR]: Unknown TARGET to build for:${1}!${NC}";
			return 1;
		;;
	esac
	
	###look for passed arguments
	for arg in $@
	do
		[ "${arg}" == "${target}" ] && break;
		key=$(echo "${arg}" | cut -d"=" -f1);
		value=$(echo "${arg}" | cut -d"=" -f2);
		__process_args "${key}" "${value}";
	done
	
	return 0;
}

function print_env()
{
	echo "TARGET=${TARGET_BUILD}";
	echo "BUILD_ROOT=${BUILD_ROOT}";
	echo "SCRIPTS=${SCRIPTS_DIR}";
	echo "CC=${CC}";
	echo "CXX=${CXX}";
	echo "LD=${LD}";
	echo "OUT_DIR=${OUT_DIR}";
	echo "ROOT_DIR=${ROOT_DIR}";
	if [ -n "${INSTALL_LIB_DIR}" ]; then
		echo "INSTALL_LIB_DIR=${INSTALL_LIB_DIR}"	
	elif [ -n "${ROOT_DIR}" ]; then
		echo "INSTALL_LIB_DIR=${ROOT_DIR}usr/lib";
	fi

	if [ -n "${INSTALL_BIN_DIR}" ]; then
		echo "INSTALL_BIN_DIR=${INSTALL_BIN_DIR}";
        elif [ -n "${ROOT_DIR}" ]; then
		echo "INSTALL_BIN_DIR=${ROOT_DIR}usr/bin";
        fi	
}

if [ "$UID" == "0" ]
then
	echo "${WHITE}WARNING: You are running as root!!${NC}"
fi

clean_env;

export BUILD_ROOT=$(gettop);
export SCRIPTS_DIR=${BUILD_ROOT}/scripts;
chmod u+x -R ${SCRIPTS_DIR}/

#by default all libraries and applications are included in the build. This is better tunned when a target build is selected!
__include_all;
