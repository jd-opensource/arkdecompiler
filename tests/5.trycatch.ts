// #20.1 try-catch
// var x;
// try {
//   x = 1;
// } catch (error) {
//   x = 2;
// }  
// x= 5;

// #20.2 try-catch with catchphi
// var x;
// try {
//   x = 1;
// } catch (error) {
//   x = console.log(error);
// }  
// x= 5;

// #20.3 try-catch with if-else
// var k;
// var x;
// try {
//     x = 1;
// } catch (error1) {
//     if(k){
//         x = 2;
//     }else{
//         x = 3;
//     }
//     x = 4;
// }
// x= 5;

// #20.4 try-catch
// var x;
// try {
//     x = 1;
// } catch (error) {
//     x = 2;
// }
// finally{
//   x = 3;
// }
// x= 4;


// #20.5 try-catch 
// var k;
// var x;
// try {
//     x = 1;
// } catch (error1) {
//     if(k){
//         x = 2;
//     }else{
//         x = 3;
//     }
//     x = 4;
// }
// finally{
//     x = 5;
// }
// x= 6;


// corner case 1: The try basic block contains many predecessors.
// declare const g1: Generator<number, void, string>;
// [..._] = g1; // error
// for (_ of g1); // error