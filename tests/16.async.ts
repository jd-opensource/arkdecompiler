// 43. asyncfunctionenter asyncfunctionresolve asyncfunctionreject  asyncfunctionawaituncaught

// async function asyncFoo() {
//     const result = await myPromise;
//     console.log(result);
// }

// 44. creategeneratorobj suspendgenerator resumegenerator getresumemode createiterresultobj
// function* generateFunc() {
//     yield 'hello';
//     yield 'world';
//     return 'done';
// }

// const generator = generateFunc();
// console.log(generator.next());

// 45. createasyncgeneratorobj asyncgeneratorresolve asyncgeneratorreject setgeneratorstate
// async function* asyncGenerateFunc(){
//     yield 'hello';
//     //return await "hello";
// }

// 46.1 iterator delegation
// class C4 {
//   async * f() {
//       const x = yield* [1];
//   }
// }

// 46.2 iterator delegation
// async function* f() {
//    const x = yield *[1];
// }
  