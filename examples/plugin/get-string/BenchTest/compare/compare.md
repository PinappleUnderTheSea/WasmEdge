

# run find substring 1e8 times
- time of wasm
   10.05s

- time of native cpp
   3.964116 s 

- time of runtime cpp
    13.85

- time of nodejs
    11.55s

  


conclusion:
1. wasm is slower than native cpp
2. runtime cpp is slower than wasm

analysis:
1. wasm is slower than native cpp because :
    - wasm is a virtual machine, it needs to translate the wasm code to native code, which is slower than native code
    - wasm is a sandbox, it has some restrictions, which may slow down the execution
    - wasm is a stack machine, it needs to push and pop the stack, which may slow down the execution
 
2. runtime cpp is slower than wasm because:
    - runtime cpp is a virtual machine, it needs to translate the runtime cpp code to native code, which is slower than native code
    - runtime cpp is will not only start a wasm virtual machine, but also need to copy wasm memory into cpp struct then operate. copying is a cost of time
    - cpp global variable is a mock heap of wasm, while linear memory and function stack is defined as a struct(or class with linear store member)  