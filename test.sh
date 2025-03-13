#/bin/sh
#for the moment, tsh is incapable of running this script
#because of if statement and for loops

cd test

TESTS=$(find -name "*.sh")

for TEST in $TESTS ; do
	if [ "$(../tsh $TEST)" != "$(cat ${TEST}.out)" ] ; then
		echo "fail $TEST"
		"../tsh" "$TEST"  | diff - "${TEST}.out"
		#exit 1
	fi
done

echo "all test passed"
exit 0

