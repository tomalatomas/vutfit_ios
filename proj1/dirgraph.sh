#!/bin/bash
#cat dirgraph|tr -d '\r'>dirgraph.sh
POSIXLY_CORRECT=yes
NORMALIZATION=false
DIRTOSEARCH=
DIRTOIGNOREREGEX=
SUBDIRSCOUNTER=
FILESCOUNTER=


check_args(){
			#Checks if directory to be searched was specified, otherwise directory to searched is current directory	
	if [ -z "$*" ]
	then
	      #echo "\$* is empty"
	      DIRTOSEARCH=$PWD
	else
	      #echo "\$* is NOT empty"
	      DIRTOSEARCH=$*
	fi
	#Checks if directory to be searched is directory 
	if [ ! -d "$DIRTOSEARCH" ]
		then
			 echo "Directory to be searched isnt a directory" >&2
		      exit 1	
	fi
	#Converting relative path to absolute
	DIRTOSEARCH="$(cd "$(dirname "$DIRTOSEARCH")"; pwd -P)/$(basename "$DIRTOSEARCH")"
	#Ensuring directory to be ignored doesnt contain root directory
	if [ ! -z "$DIRTOIGNOREREGEX" ]
		then
			if  echo "$DIRTOSEARCH" | grep -Eqs "$DIRTOIGNOREREGEX";
				then
	        		echo "Directory to be ignored collides with root directory " >&2
					exit 1
		fi
	fi
}

init_args(){
	while getopts "i:n" o; 
	do
	    case $o in
	    i)
	        #echo "Option ’i’ found  with parameter ’$OPTARG’."
	        DIRTOIGNOREREGEX=$OPTARG
	        ;;
	    n)
	        #echo "Option ’n’ found."
	        NORMALIZATION=true
	        ;;
	    *)
	        echo "Usage: dirgraph [-i FILE_ERE] [-n] [DIR] " >&2
	        exit 1;;
	    esac
	done
	((OPTIND--))
	shift $OPTIND
	check_args "$@"

}

print_info(){
	SUBDIRSCOUNTER=`find "$DIRTOSEARCH" -type d -not -regex "$DIRTOIGNOREREGEX" |  wc -l`
	FILESCOUNTER=`find "$DIRTOSEARCH" -type f -not -regex "$DIRTOIGNOREREGEX" |  wc -l`
	echo "Root directory: $DIRTOSEARCH"
	echo "Directories: $SUBDIRSCOUNTER" 
	echo "All files: $FILESCOUNTER" 
}
init_args "$@" #Calling function with arguments
print_info