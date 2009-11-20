BEGIN {state = 0;}

#search channel

/channel [0-9]+/{

   state=0;
   if($2!=var)
      state=1;
    
    print;
}

/vol [0-9]+/{
   if(state==1)
      print "vol 0";
   else
      print;
}


/incv/{
   if(state!=1)
     print
};

/decv/{
   if(state!=1)
     print
};

/adsr .*/{
   if(state!=1)
     print;
}

/frec .*/{
   if(state!=1)
     print;
}


/.*/{
   if($1 != "channel" && $1 != "vol" &&
      $1 != "incv" && $1 != "decv" && 
      $1 != "adsr" && $1 != frec)
	print;
}


	 


