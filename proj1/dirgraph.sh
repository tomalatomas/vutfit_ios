#!/bin/sh
#cat dirgraph|tr -d '\r'>dirgraph.sh
POSIXLY_CORRECT=yes
histnormalization=false
dirtosearch=
dirtoignoreregex="/[^\w\W]/"
subdirscounter=
filescounter=
under100B=0
under1KiB=0
under10KiB=0
under100KiB=0
under1MiB=0
under10MiB=0
under100MiB=0
under1GiB=0
aboveeq1GiB=0
normalizationcoefficient=1
labelsoffset=13

check_args(){
			#Checks if directory to be searched was specified, otherwise directory to searched is current directory	
	if [ -z "$*" ]
	then
	      #echo "\$* is empty"
	      dirtosearch=$PWD
	else
	      #echo "\$* is NOT empty"
	      dirtosearch=$*
	fi
	#Checks if directory to be searched is directory 
	if [ ! -d "$dirtosearch" ]
		then
			 echo "Directory to be searched isnt a directory" >&2
		      exit 1	
	fi
	#Converting relative path to absolute
	dirtosearch="$(cd "$(dirname "$dirtosearch")"; pwd -P)/$(basename "$dirtosearch")"
	#Ensuring directory to be ignored doesnt contain root directory
	if [ ! -z "$dirtoignoreregex" ]
		then
			if  echo "$dirtosearch" | grep -Eqs "$dirtoignoreregex";
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
	        dirtoignoreregex=$OPTARG
	        ;;
	    n)
	        #echo "Option ’n’ found."
	        histnormalization=true
	        ;;
	    *)
	        echo "Usage: dirgraph [-i FILE_ERE] [-n] [DIR] " >&2
	        exit 1;;
	    esac
	done
	shift $((OPTIND-1))


	check_args "$@"

}

print_info(){
	subdirscounter=$(find "$dirtosearch" -type d -not -regex "$dirtoignoreregex" |  wc -l)
	filescounter=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" |  wc -l)
	echo "Root directory: $dirtosearch"
	echo "Directories: $subdirscounter" 
	echo "All files: $filescounter" 
}
drawHashes(){
	x=1
	while [ $x -le $1 ]
	do
	  printf "#"
	  x=$(( $x + 1 ))
	done
}
filesize_counter(){
	under100B=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size -100c | wc -l)
	under1KiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +100c -size -1024c | wc -l)
	under10KiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +1024c -size -10k | wc -l)
	under100KiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +10k -size -100k | wc -l)
	under1MiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +100k -size -1024k | wc -l)
	under10MiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +1024k -size -10M | wc -l)
	under100MiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +10M -size -100M | wc -l)
	under1GiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +100M -size -1024M | wc -l)
	aboveeq1GiB=$(find "$dirtosearch" -type f -not -regex "$dirtoignoreregex" -size +1023M | wc -l)

	#echo "$under100B"
	#echo "$under1KiB"
	#echo "$under10KiB"
	#echo "$under100KiB"
	#echo "$under1MiB"
	#echo "$under10MiB"
	#echo "$under100MiB"
	#echo "$under1GiB"
	#echo "$aboveeq1GiB"

}
getmaxcount(){
	maxCount=$(( under100B > under1KiB ? under100B : under1KiB ))
	maxCount=$(( maxCount > under10KiB ? maxCount : under10KiB ))
	maxCount=$(( maxCount > under100KiB ? maxCount : under100KiB ))
	maxCount=$(( maxCount > under1MiB ? maxCount : under1MiB ))
	maxCount=$(( maxCount > under10MiB ? maxCount : under10MiB ))
	maxCount=$(( maxCount > under100MiB ? maxCount : under100MiB ))
	maxCount=$(( maxCount > under1GiB ? maxCount : under1GiB ))
	maxCount=$(( maxCount > aboveeq1GiB ? maxCount : aboveeq1GiB ))


}
normalization(){
	terminalsize=$(tput cols)
	maxHashes=$((terminalsize-labelsoffset))
	#echo "$terminalsize"
	getmaxcount
	#echo "$maxCount"
	normalizationcoefficient=$((maxHashes / maxCount))
	under100B=$((under100B * normalizationcoefficient))
	under1KiB=$((under1KiB * normalizationcoefficient))
	under10KiB=$((under10KiB * normalizationcoefficient))
	under100KiB=$((under100KiB * normalizationcoefficient))
	under1MiB=$((under1MiB * normalizationcoefficient))
	under10MiB=$((under10MiB * normalizationcoefficient))
	under100MiB=$((under100MiB * normalizationcoefficient))
	under1GiB=$((under1GiB * normalizationcoefficient))
	aboveeq1GiB=$((aboveeq1GiB * normalizationcoefficient))
}
drawFsHist(){
	filesize_counter
	
	if $histnormalization; then
		normalization
	fi

	echo "File size histogram:"
	echo "  <100  B  : $(drawHashes $under100B)" 
	echo "  <1 KiB   : $(drawHashes $under1KiB)"
	echo "  <10 KiB  : $(drawHashes $under10KiB)"
	echo "  <100 KiB : $(drawHashes $under100KiB)"
	echo "  <1 MiB   : $(drawHashes $under1MiB)"
	echo "  <10 MiB  : $(drawHashes $under10MiB)"
	echo "  <100 MiB : $(drawHashes $under100MiB)"
	echo "  <1 GiB   : $(drawHashes $under1GiB)"
	echo "  >=1 GiB  : $(drawHashes $aboveeq1GiB)"

}
init_args "$@" #Calling function with arguments
print_info
drawFsHist
