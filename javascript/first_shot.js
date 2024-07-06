// first_shot.js

// C:\Program Files\Microvium\Microvium.exe

const print = vmImport(1);
vmExport(1, add);
vmExport(2, sayHello);

function add(a, b) {
  return a + b;
}

function sayHello() {
  print('Hello');
}