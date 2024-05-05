<a name="readme-top"></a>
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/hiperiondev/stack_vm">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Stack VM</h3>

  <p align="center">
    Generic Stack VM for Scripting Languages
    <br />   
  </p>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project
Stack VM is a virtual machine oriented towards scripting languages, especially glue ones, prioritizing low memory consumption and predictability.

It has very useful features for use in microcontrollers such as: Fixed stack size, gc per frame with optional shrink and management of both local variables in the stack and a generic heap.

Functions that are not essential must be implemented through FFI (example: strings) but have the minimum constructs so that the VM can handle them.
Also the use of native data types has generic options for use as well as the definition of external functions to be interpreted within the VM.
<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- GETTING STARTED -->
## Features
- [x] Fully commented code in Doxygen format
- [x] 6 bits op (easily extendable)
- [x] An indirection register with automatic increment/decrement
- [x] Heap with automatic growth and optional shrink. Allocation management by bit mask (simple and fast)
- [x] Call with frame placement and stack space reservation for local variables
- [x] Heap management per frame with automatic release at the exit of the frame
- [x] Native external data management (via custom function)
- [x] Primitives for strings
- [x] Simple external functions (FFI)
- [x] Global variables and arrays allocated in heap
  - [x] Can allocate local arrays 
- [x] Constants in the program area
- [x] Complete Assembler/Disassembler with directives easily expandable

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- USAGE -->
## Usage
See [Documentation](https://stack-vm.readthedocs.io)

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/hiperiondev/stack_vm/issues) for a full list of proposed features (and known issues).

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork it (<https://github.com/hiperiondev/stack_vm/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>

<!-- CONTACT -->
## Contact

*Emiliano Augusto Gonzalez - egonzalez.hiperion@gmail.com*

Project Link: [https://github.com/hiperiondev/stack_vm](https://github.com/hiperiondev/stack_vm)

<div align="right">
  <a href="#readme-top">
    <img src="images/backtotop.png" alt="backtotop" width="30" height="30">
  </a>
</div>
