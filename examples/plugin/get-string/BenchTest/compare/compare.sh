
for i in {1..1000}
do
   a=$(wasmedge --string1="1423.111aaasihd" --string2="2455.2bbb" --upper=1 ../UnitTest/zjj-test-wasm/compare_in_memory_and_stack.wasm )
done