var name;

var asmLibraryArg = {
 "save1": 1,
 "save2": 2
};

WebAssembly.instantiate(Module["wasm"], imports).then(function(output) {
 asm = output.instance.exports;
 expD1 = asm["expD1"];
 expD2 = asm["expD2"];
 expD3 = asm["expD3"];
 expD4 = asm["expD4"];
 initRuntime(asm);
 ready();
});

expD1;

Module["expD2"];

asm["expD3"];

expI1;

Module["expI2"];

asm["expI3"];
