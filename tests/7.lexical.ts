// # 25.1 newlexenv/poplexenv/stlexvar/ldlexvar
// function hello() {
//     function hi() {
//         //for (var i = 0; i++ < 10;) {
//             var x1 = 21;
//             let x2 = function () {
//                 return x1;
//             };
//        //}
//     }
// }

// # 25.2 newlexenv/poplexenv/stlexvar/ldlexvar
// (function () {
//     function f1() {
//       f3();
//     }
//     function f3() {
//     }
//     function f5() {}
//     function f6(a) {
//         let v31 = a ;

//         function f19() {
//             let v36 = 21;
//             let v37 = function () {
//                 return v36;
//             };
//             for (var v33 = 0; v33++ < 10;) {
//                 let v38 = 22;
//                 let v39 = function () { 
//                     let v40 = v31;
//                     f1();
//                     return v38;
//                 };
//             }         
//         }

//         f19();
//     }
//     for (var v1 = 0; v1 < 10; v1++) {
//       f6();
//     }
//     f1();
// })();