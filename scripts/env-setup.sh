#!/bin/sh

RED='\033[0;31m'
WHITE='\033[1;37m'
BLUE='\033[0;34m'
NC='\033[0m'

# This is still an early stage meaning that there isn't
# a clear idea on how the targets will be managed. For now add them in this array.
# This target concept will probably change in the future by a project or device concept...
TARGETS=(HOST RPI)

function __include_all()
{
	local SRCS_DIR=${BUILD_ROOT}/src/
	local dir="";
	local subdir="";

	pushd "${SRCS_DIR}" 1>/dev/null;

	for dir in $(find . -maxdepth 1 -mindepth 1 -type d -printf '%f ')
	do
		pushd "${dir}" 1>/dev/null;
		for subdir in $(find . -maxdepth 1 -mindepth 1 -type d -printf '%f ')
		do
			touch "${subdir}/.detect";
		done
		popd 1>/dev/null;
	done

	popd 1>/dev/null;

	return 0;
}

function __is_supported_target()
{
	local target=""

	for target in ${TARGETS[*]}
	do
		[ "${1}" == "${target}" ] && return 0;
	done

	return 1;
}

function __create_out_dirs()
{
	export OUT_DIR=${BUILD_ROOT}/out;
	# let's create the output directories
	mkdir -p "${OUT_DIR}/${1}/bin"
	mkdir -p "${OUT_DIR}/${1}/lib"
	mkdir -p "${OUT_DIR}/${1}/obj"
}

function __setup_common()
{
	__create_out_dirs "${1}"

	# handle rootdir
	if [ -n "${ROOT_DIR}" ]; then
		mkdir -p "${ROOT_DIR}/usr/bin"
	fi

	if [ -n "${ROOT_DIR}" ]; then
		mkdir -p "${ROOT_DIR}/usr/lib"
	fi
}

function __setup_compiler()
{
	hash "${1}"gcc 2>/dev/null || { echo "Make sure you have ${1}gcc on your system!!!"; return 1; }

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
	# setup compiler
	__setup_compiler;
	[ $? == 1 ] && return 1;
	# if the target is the host, then we just need
	# to call this once
	__setup_common host;
	export TARGET_BUILD=host

	return 0;
}

function __check_ownerchip()
{
	[ $# -ne 1 ] && { echo "Invalid number of parameters in __check_ownerchip" ; return 1; }
	local owner='';

	owner=$(ls -ld "$1" | awk '{print $3}');
	if [ "${owner}" == "root" ]
	then
		local opt;
		echo -en "${BLUE}Owner of \"$1\" is the root user. Are you sure you want to use this path [y]? ${NC}"
		read -r opt;
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
	[ $# -ne 2 ] && { echo "Invalid number of parameters in __process_args" ; return 1; }

	case "${1}" in
		"--rootdir")
			[ ! -d "${2}" ] && { echo -e "${RED}[ERROR]: Make sure to create rootdir=\"$2\"${NC}"; return 1; }
			__check_ownerchip "${2}";
			[ $? == 1 ] && return 1;
			export ROOT_DIR="$(realpath "${2}")/";
		;;
		*)
			echo -e "${RED}[ERROR]: Invalid key=\"${1}\"${NC}";
		;;
	esac

	return 0;
}

function __add_target_complete()
{
	local cur="${COMP_WORDS[COMP_CWORD]}";
	local last="${COMP_WORDS[COMP_CWORD-1]}";
	local opts=('--rootdir=' HOST RPI);

	# at this point we want to autocomplete with directories
	if [[ "${cur}" == "=" || "${last}" == "=" ]]
	then
		# echo "wait now for dir, cur=${cur}";
		compopt -o filenames;
		if [ "${cur}" == "=" ]; then
			COMPREPLY=( $(compgen -d) );
		else
			COMPREPLY=( $(compgen -d -- ${cur}) );
		fi

		return 0;
	fi

	[ -n "$(echo "${cur}" | egrep '^--')" ] && compopt -o nospace;
	COMPREPLY=( $(compgen -W "${opts[*]}" -- ${cur}) );
	return 0;
}

# based on aosp
# xargs 0 tells that the inputs are terminated by null char and not whitespace. Important because of -print0
function cgrep()
{
	find . -name .git -prune -o -type f \( -name '*.c' -o -name '*.h' -o -name '*.cpp' -o -name '*.cc' -o -name '*.hpp' \) -print0 | xargs -0 grep --colour -n "$@"
}

function mgrep()
{
	find . -name .git -prune -o -type f \( -name '*.mk' -o -name 'Makefile' \) -print0 | xargs -0 grep --colour -n "$@"
}
function gettop()
{
	local TOPFILE=scripts/env-setup.sh
	if [ -n "${BUILD_ROOT}" ] && [ -d "${BUILD_ROOT}/${TOPFILE}" ]
	then
		# build_root should be the physical path and not a symlink
		echo "${BUILD_ROOT}";
	else
		if [ -f "${TOPFILE}" ]
		then
			PWD= /bin/pwd;
		else
			local HERE=$PWD;
			T="";
			while [ ! -f "${TOPFILE}" ] && [ $PWD != "/" ]
			do
				cd ..;
				#-P to avoid symlinks
				T=`PWD= /bin/pwd -P`;
			done
			cd "${HERE}";
			if [ -f "${T}/${TOPFILE}" ]
			then
				echo "$T";
			fi
		fi
	fi
}

function croot()
{
	local T=$(gettop)
	if [ -d "${T}" ]
	then
		cd "${T}" || return;
	else
		echo "${WHITE}Couldn't locate the top of the tree. $T${NC}";
	fi
}

function clean_env()
{
	export TARGET_BUILD=''
	export OUT_DIR=''
	export ROOT_DIR=''
	export AR=''
	export AS=''
	export LD=''
	export CC=''
	export CXX=''
	export CPP=''
	export NM=''
	export STRIP=''
	export SSTRIP=''
	export OBJCOPY=''
	export OBJDUMP=''
	export RANLIB=''
}

function help_me()
{
	echo -e "${BLUE}Availabe commands:${NC}"
	echo -e "${WHITE} - cgrep: Used like grep, but only greps on c and cpp files.${NC}"
	echo -e "${WHITE} - mgrep: Used like grep, but only greps in Makefiles and .mk.${NC}"
	echo -e "${WHITE} - croot: Change directory to "\"${BUILD_ROOT}\"".${NC}"
	echo -e "${WHITE} - clean_env: Clear all env variables.${NC}"
	echo -e "${WHITE} - print_env: Print all env variables.${NC}"
	echo -e "${WHITE} - add_compilable: Add a compilations unit. Expects a valid path as argument.${NC}"
	echo -e "${WHITE} - add_target_build: Add a target to build. Type tab for autocomplete.${NC}"
}

function add_compilable()
{
	local tmp="";
	local dir="";
	if [ ! -d "${1}" ]; then
		echo -e "${RED}Invalid directory ${1}${NC}";
		return 1;
	fi

	tmp=$(realpath "${1}" --relative-to="${BUILD_ROOT}" | awk -F"/" '{print $1}');

	[ "${tmp}" == ".." ] && { echo -e "${WHITE}[WARNING]: "$1" is not a directory under the Sdk root directory${NC}"; return 1; }

	dir=$(echo "${1}" | sed -e 's/\/$//');
	touch ${dir}/.detect;
}

function add_target_build()
{
	[ $# -ge 1 ] || { echo "[ERROR]: Invalid number of parameters"; return 1; }
	local arg='';
	local key='';
	local value='';
	local target='';
	local error='';

	# clean previous env
	clean_env;
	# look for passed arguments
        for arg in "$@"
        do
		__is_supported_target "${arg}";
                [ $? == "0" ] && { \
			if [ -n "${target}" ]; then
				echo -e "${RED}[ERROR]: More than one valid target passed on the argument list! Args="$@"${NC}";
				error=y;
				break;
			fi
			target="${arg}"; continue; }
                key=$(echo "${arg}" | cut -d"=" -f1);
                value=$(echo "${arg}" | cut -d"=" -f2);
                if ! __process_args "${key}" "${value}"; then
			error=y;
			break;
		fi
        done

	[ -z "${target}" ] && { echo -e "${RED}[ERROR]: No target specified...${NC}"; clean_env; return 1; }
	[ "${error}" == "y" ] && { clean_env; return 1; }

	case "${target}" in

		"HOST")
			__setup_host;
			[ $? == 1 ] && { clean_env; return 1; }
		;;
		"RPI")
			echo "RPI is not yet supportted";
		;;
		*)
			echo -e "${RED}[ERROR]: Unknown TARGET to build for:${target}!${NC}";
			clean_env;
			return 1;
		;;
	esac

	return 0
}
complete -F __add_target_complete add_target_build;

function print_env()
{
	echo "TARGET=${TARGET_BUILD}";
	echo "BUILD_ROOT=${BUILD_ROOT}";
	echo "APPS_DIR=${APPS_DIR}";
	echo "LIBS_DIR=${LIBS_DIR}";
	echo "TEST_DIR=${TEST_DIR}";
	echo "SCRIPTS=${SCRIPTS_DIR}";
	echo "CC=${CC}";
	echo "CXX=${CXX}";
	echo "LD=${LD}";
	echo "OUT_DIR=${OUT_DIR}";
	echo "ROOT_DIR=${ROOT_DIR}";
}

if [ "$UID" -eq "0" ]
then
	echo "${WHITE}WARNING: You are running as root!!${NC}"
fi

clean_env;

export BUILD_ROOT=$(gettop);
export SCRIPTS_DIR=${BUILD_ROOT}/scripts;
export LIBS_DIR=${BUILD_ROOT}/src/libs;
export APPS_DIR=${BUILD_ROOT}/src/apps;
export TEST_DIR=${BUILD_ROOT}/test;

chmod u+x -R "${SCRIPTS_DIR}/"

# by default all libraries and applications are included in the build.
# This is better tunned when a target build is selected!
__include_all;
