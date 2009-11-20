#!/bin/sh
#Primer parametro cancion
#segundo parametro pista
#tercer parametro fichero salida


for i in `ls | grep .mcm` ; do
   if [ -f $i ] ; then
      echo $i
      for j in `seq 1 12` ; do
	let "cont=$j-1"
	if [[ $cont -gt 2 ]];  then
	   let "cont-=3"
	   ext=`echo '.fm'`
	else
	   ext=`echo '.ps'`
	fi
	
       export file=`echo $i| cut -d . -f 1`
      ./dis $i | awk -f channel.awk -v var=$j | ./asm > $file$ext$cont
    done
  fi
done

