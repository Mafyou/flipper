// getting-started.c

#include "microvium.h"
#include "stdio.h"

#define IMPORT_COUNT 2

static mvm_VM* vm;

static const uint16_t importIDs[IMPORT_COUNT] = { 1, 2 };
static struct {
  mvm_Value add;
  mvm_Value sayHello;
} imports;

// Bytes copy-pasted from command `microvium script.mvm.js --output-bytes`
static const uint8_t bytecode[] = { {0x04,0x1c,0x03,0x00,0x78,0x00,0xe6,0xdf,0x03,0x00,0x00,0x00,0x1c,0x00,0x1e,0x00,0x26,0x00,0x26,0x00,0x2a,0x00,0x2c,0x00,0x6a,0x00,0x6e,0x00,0x01,0x00,0x01,0x00,0x55,0x00,0x02,0x00,0x5d,0x00,0x09,0x00,0x6d,0x00,0x31,0x00,0x00,0x00,0x05,0x40,0x70,0x75,0x73,0x68,0x00,0x00,0x06,0x40,0x48,0x65,0x6c,0x6c,0x6f,0x00,0x02,0x60,0x00,0x00,0x0d,0x50,0x04,0x31,0x30,0x30,0x88,0x21,0x00,0x6b,0x12,0x6f,0x67,0x01,0x60,0x00,0x05,0x50,0x02,0x31,0x32,0x6c,0x60,0x00,0x0d,0x50,0x03,0x89,0x00,0x00,0x01,0x88,0x39,0x00,0x78,0x02,0x67,0x01,0x60,0x00,0x41,0x00,0x02,0x00,0x08,0xc0,0x09,0x00,0x09,0x00,0x31,0x00,0x45,0x00} };

static mvm_TeError resolveHostFunction(mvm_HostFunctionID funcID, void* context, mvm_TfHostFunction* out);
static mvm_TeError print(mvm_VM* vm, mvm_HostFunctionID funcID, mvm_Value* result, mvm_Value* args, uint8_t argCount);

// Call this at startup to create the VM
mvm_TeError init() {
  mvm_TeError err;

  // Load the virtual machine
  err = mvm_restore(&vm, (uint8_t*)&bytecode, sizeof bytecode, NULL, &resolveHostFunction);
  if (err != MVM_E_SUCCESS) return err;

  // Find the "add" function exported by the VM
  err = mvm_resolveExports(vm, importIDs, (mvm_Value*)&imports, IMPORT_COUNT);
  if (err != MVM_E_SUCCESS) return err;
}

// Call this to gracefully free the VM
void deinit() {
  // Destroy the VM
  mvm_free(vm);
}

int add(int a, int b) {
  mvm_Value args[2];
  mvm_Value result;
  mvm_TeError err;
  int r;

  // Call "add"
  args[0] = mvm_newInt32(vm, a);
  args[1] = mvm_newInt32(vm, b);
  err = mvm_call(vm, imports.add, &result, args, sizeof args / sizeof args[0]);
  if (err != MVM_E_SUCCESS) return 0;
  r = mvm_toInt32(vm, result);

  // Run a garbage collection cycle (optional)
  mvm_runGC(vm, true);

  return r;
}

void sayHello() {
  mvm_call(vm, imports.sayHello, NULL, 0, 0);
}

static mvm_TeError resolveHostFunction(mvm_HostFunctionID funcID, void* context, mvm_TfHostFunction* out) {
  // The script imports the print function using ID 1
  if (funcID == 1) {
    *out = print;
    return MVM_E_SUCCESS;
  }
  return MVM_E_UNRESOLVED_IMPORT;
}

mvm_TeError print(mvm_VM* vm, mvm_HostFunctionID funcID, mvm_Value* result, mvm_Value* args, uint8_t argCount) {
  assert(argCount == 1);
  printf("%s\n", mvm_toStringUtf8(vm, args[0], NULL));
  return MVM_E_SUCCESS;
}

int main() {
  mvm_TeError err;

  err = init();
  if (err != MVM_E_SUCCESS) return err;

  int x = add(42, 43); 
  printf("%i\n", x); // Prints 85

  sayHello(); // Should print 'Hello'

  deinit();
}