
SAMPLETAG=$1
JOBTAG=$2
OUTPUTFILE=fulleff_${SAMPLETAG}.root

if [ -z $1 ]; then
    echo "Job tag information for the plotter is not provided"
fi

# rm -rf plots_${SAMPLETAG}
# rm -rf results/${SAMPLETAG}${JOBTAG}/plots_${SAMPLETAG}

cd results/${SAMPLETAG}_${JOBTAG}/
python ../../plot.py 1 ${OUTPUTFILE} ${SAMPLETAG} > plot_1.log &
python ../../plot.py 2 ${OUTPUTFILE} ${SAMPLETAG} > plot_2.log &
python ../../plot.py 3 ${OUTPUTFILE} ${SAMPLETAG} > plot_3.log &
python ../../plot.py 4 ${OUTPUTFILE} ${SAMPLETAG} > plot_4.log &
sleep 1
echo "<== Submitted parallel jobs ..."
wait

# cp -r plots_${SAMPLETAG}/ results/${SAMPLETAG}_${JOBTAG}/plots_${SAMPLETAG}
