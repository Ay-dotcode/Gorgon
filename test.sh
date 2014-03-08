g++ Tests/test.cpp -lcpptest -o test
rc=$?
if [[ $rc != 0 ]] ; then
    echo "failed to compile."
    exit $rc
fi

./test
rc=$?
if [[ $rc != 0 ]] ; then
    echo "failed to test."
    exit $rc
fi
