package main

import (
	"flag"
	"fmt"
	"os/exec"
)

func main() {
	// Parse command line arguments
	string1 := flag.String("string1", "", "First string argument")
	string2 := flag.String("string2", "", "Second string argument")
	upper := flag.String("upper", "", "1")
	flag.Parse()

	// Prepare the command
	cmd := exec.Command("/Users/junjie.zhu/Desktop/Code/WasmEdge/build/tools/wasmedge/wasmedge ../UnitTest/zjj-test-wasm/slices_find_rfind.wasm", *string1, *string2, *upper)


	// Run the command
	output, err := cmd.CombinedOutput()
	if err != nil {
		fmt.Println(fmt.Sprint(err) + ": " + string(output))
		return
	}

	fmt.Println(string(output))
}