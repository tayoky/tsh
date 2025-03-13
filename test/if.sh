if false
then
	echo "first if failed"
	exit 1
fi

if false
then
	if true
	then
		echo "fail"
		exit 1
	fi
	echo "fail"
	exit 1
fi

if true
then
	echo yo
	exit
fi
exit 1
