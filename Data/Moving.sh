#!/bin/bash
#---- Script to move wavedump files to PMT directories
echo Which test SPE or Gain or AP or DR?
read test

#----------------------------------------------------------------------
#---------------------SPE test moving statements-----------------------
#----------------------------------------------------------------------
if [ $test = "SPE" ]
then

    echo Which tent A or B?
    read tent
    
    if [ $tent = "A" ]
    then 
	echo PMT and run of wave_0?
	read PMT0
	mv -vn wave_0.dat /home/user1/BinaryData/PMT$PMT0/Nominal/
	
	echo PMT and run of wave_1?
	read PMT1
	mv -vn wave_1.dat /home/user1/BinaryData/PMT$PMT1/Nominal/
	
	echo PMT and run of wave_2?
	read PMT2
	mv -vn wave_2.dat /home/user1/BinaryData/PMT$PMT2/Nominal/
	
	echo PMT and run of wave_3?
	read PMT3
	mv -vn wave_3.dat /home/user1/BinaryData/PMT$PMT3/Nominal/


    elif [ $tent = "B" ]
    then 

	echo PMT and run of wave_4?
	read PMT4
	mv -vn wave_0.dat /home/user1/BinaryData/PMT$PMT4/Nominal/wave_4.dat
	
	echo PMT and run of wave_5?
	read PMT5
	mv -vn wave_1.dat /home/user1/BinaryData/PMT$PMT5/Nominal/wave_5.dat
	
	echo PMT and run of wave_6?
	read PMT6
	mv -vn wave_2.dat /home/user1/BinaryData/PMT$PMT6/Nominal/wave_6.dat
	
	echo PMT and run of wave_7?
	read PMT7
	mv -vn wave_3.dat /home/user1/BinaryData/PMT$PMT7/Nominal/wave_7.dat

    else
	echo "TENT LABEL DOES NOT EXIST!"

    fi

#----------------------------------------------------------------------
#--------------------Gain test moving statements-----------------------
#----------------------------------------------------------------------
elif [ $test = "Gain" ]
then


    echo Which tent A or B?
    read tent
    
    if [ $tent = "A" ]
    then

	echo Which hv test, hv1, hv2, hv3, hv4 or hv5?
	read HV

	echo PMT and run of wave_0?
	read PMT0
	mv -vn wave_0.dat /home/user1/BinaryData/PMT$PMT0/GainTest/wave_0_$HV.dat

	echo PMT and run of wave_1?
	read PMT1
	mv -vn wave_1.dat /home/user1/BinaryData/PMT$PMT1/GainTest/wave_1_$HV.dat

	echo PMT and run of wave_2?
	read PMT2
	mv -vn wave_2.dat /home/user1/BinaryData/PMT$PMT2/GainTest/wave_2_$HV.dat

	echo PMT and run of wave_3?
	read PMT3
	mv -vn wave_3.dat /home/user1/BinaryData/PMT$PMT3/GainTest/wave_3_$HV.dat
	
    


    elif [ $tent = "B" ]
    then

	echo Which hv test, hv1, hv2, hv3, hv4 or hv5?
	read HV

	echo PMT and run of wave_4?
	read PMT4
	mv -vn wave_0.dat /home/user1/BinaryData/PMT$PMT4/GainTest/wave_4_$HV.dat

	echo PMT and run of wave_5?
	read PMT5
	mv -vn wave_1.dat /home/user1/BinaryData/PMT$PMT5/GainTest/wave_5_$HV.dat

	echo PMT and run of wave_6?
	read PMT6
	mv -vn wave_2.dat /home/user1/BinaryData/PMT$PMT6/GainTest/wave_6_$HV.dat

	echo PMT and run of wave_7?
	read PMT7
	mv -vn wave_3.dat /home/user1/BinaryData/PMT$PMT7/GainTest/wave_7_$HV.dat
	
    else
	echo "TENT LABEL DOES NOT EXIST!"
	
    
    fi




#----------------------------------------------------------------------
#---------------------AP test moving statements------------------------
#----------------------------------------------------------------------
elif [ $test = "AP" ]
then

    echo PMT and run of wave_0?
    read PMT0
    mv -vn wave_0.dat/home/user1/BinaryData/PMT$PMT0/APTest/

    echo PMT and run of wave_1?
    read PMT1
    mv -vn wave_1.dat/home/user1/BinaryData/PMT$PMT1/APTest/

    echo PMT and run of wave_2?
    read PMT2
    mv -vn wave_2.dat/home/user1/BinaryData/PMT$PMT2/APTest/

    echo PMT and run of wave_3?
    read PMT3
    mv -vn wave_3.dat/home/user1/BinaryData/PMT$PMT3/APTest/

    echo PMT and run of wave_4?
    read PMT4
    mv -vn wave_4.dat/home/user1/BinaryData/PMT$PMT4/APTest/

    echo PMT and run of wave_5?
    read PMT5
    mv -vn wave_5.dat/home/user1/BinaryData/PMT$PMT5/APTest/

    echo PMT and run of wave_6?
    read PMT6
    mv -vn wave_6.dat/home/user1/BinaryData/PMT$PMT6/APTest/

    echo PMT and run of wave_7?
    read PMT7
    mv -vn wave_7.dat/home/user1/BinaryData/PMT$PMT7/APTest/







#----------------------------------------------------------------------
#--------------------DR test moving statements-------------------------
#----------------------------------------------------------------------
elif [ $test = "DR" ]
then
  
    echo PMT and run of wave_0?
    read PMT0
    mv -vn wave_0.dat/home/user1/BinaryData/PMT$PMT0/DarkRateTest/

    echo PMT and run of wave_1?
    read PMT1
    mv -vn wave_1.dat/home/user1/BinaryData/PMT$PMT1/DarkRateTest/

    echo PMT and run of wave_2?
    read PMT2
    mv -vn wave_2.dat/home/user1/BinaryData/PMT$PMT2/DarkRateTest/

    echo PMT and run of wave_3?
    read PMT3
    mv -vn wave_3.dat/home/user1/BinaryData/PMT$PMT3/DarkRateTest/

    echo PMT and run of wave_4?
    read PMT4
    mv -vn wave_4.dat/home/user1/BinaryData/PMT$PMT4/DarkRateTest/

    echo PMT and run of wave_5?
    read PMT5
    mv -vn wave_5.dat/home/user1/BinaryData/PMT$PMT5/DarkRateTest/

    echo PMT and run of wave_6?
    read PMT6
    mv -vn wave_6.dat/home/user1/BinaryData/PMT$PMT6/DarkRateTest/

    echo PMT and run of wave_7?
    read PMT7
    mv -vn wave_7.dat/home/user1/BinaryData/PMT$PMT7/DarkRateTest/
    
    else
	echo "TEST TYPE NOT FOUND!"

    fi


if [ $(ls wave_?.dat | wc -l) -ne 0 ]
then 
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!! Number of wave files remaining is" $(ls wave_?.dat | wc -l) " !!!!!!!!!!!!!" 
    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"

fi
