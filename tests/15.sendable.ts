// 31. sendable
// class SendableB {
//     constructor() {
//         "use sendable";
//     }

//     hello(){
//         this.name = "hello";
//         console.log("11");
//     }
// }

// function test2() {
//     var a = new SendableB();
//     var b = 1;
//     function hi() {
//         var m = a;
//         console.log(m);
//         var n = b;
//         console.log(n);
//     }
// }

// 32. ldsendableclass DEFINEPROPERTYBYNAME
// class B {
//     constructor() {
//       "use sendable";
//     }
//     static b: number = 1;
//     b: number = B.b;
//     //c: number = B["b"];
// }