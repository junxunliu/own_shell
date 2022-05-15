To compile this files:
    I wrote Makefile to help me test different test cases
    - add your test case in Makefile (replace simple-test to your_test_case):
        all: your_test_case
        your_test_case: your_test_case.o byos.o
            gcc -Wall $^ -o $@
        
    - use make
        for example:
            make 
            ./your_test_case
        one line:
            make && clear && ./your_test_case

Also, you can compile instead of using Makefile:
    gcc -Wall byos.c your_test_case.c -o your_test_case
    ./your_test_case