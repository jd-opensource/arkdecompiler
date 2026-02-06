// #6. callargs callthis
// var x1 = clearTimeout();
// var x2 = parseInt("42");
// var x3 = parseInt("1010", 2); 
// var x4 = parseInt("10", 10, 0);
// var x5 = Math.random();
// var x6 = console.log("hello");

// #9. callarg1 tLoadString
// var obj11 = BigInt("1234567890123456789012345678901234567890");

// 14. callthisrange callrange
// var x= setInterval(Math.max, 2000, "Bob", "Hi");
// const maxNumber = Math.max(1,2,3,4,5,6,7);

// #16. function declaration and DEFINEFUNC
// function add(a: number, b: number): number {
//     return a + b;
// }
// var x = add(2, 3);


// #23. function call def-use detection
// console.log("hi");
// var x6 = console.log("hello");


// # 28. DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8
// function hello(){
//     var x = 1;
// }


// 40. getunmappedargs
// function exampleFunction(a, b) {
//     console.log(arguments.length); 
//     console.log(arguments[0]); 
//     console.log(arguments[1]);
// }


// 40. SUPERCALLARROWRANGE
// function exampleFunction(a, b) {
//     console.log(arguments.length); 
//     console.log(arguments[0]); 
//     console.log(arguments[1]);
// }



// #1 corner case: recursive calls and recursive references
// function md5(string:string): void {

//     function FF(a,b,c,d,x,s,ac) {
//         return 0;
//     };

//     function GG(a,b,c,d,x,s,ac) {
//         return 0;
//     };

//     function HH(a,b,c,d,x,s,ac) {
//         return 0;
//     };

//     function II(a,b,c,d,x,s,ac) {
//         return 0;
//     };

//     var x=Array();
//     var k,AA,BB,CC,DD,a,b,c,d;
//     var S11=7, S12=12, S13=17, S14=22;
//     var S21=5, S22=9 , S23=14, S24=20;
//     var S31=4, S32=11, S33=16, S34=23;
//     var S41=6, S42=10, S43=15, S44=21;

//     x = [1];

//     a = 0x67452301; b = 0xEFCDAB89; c = 0x98BADCFE; d = 0x10325476;

//     for (k=0;k<x.length;k+=16) {
//         AA=a; BB=b; CC=c; DD=d;
//         a=FF(a,b,c,d,x[k+0], S11,0xD76AA478);
//         d=FF(d,a,b,c,x[k+1], S12,0xE8C7B756);
//         c=FF(c,d,a,b,x[k+2], S13,0x242070DB);
//         b=FF(b,c,d,a,x[k+3], S14,0xC1BDCEEE);
//         a=FF(a,b,c,d,x[k+4], S11,0xF57C0FAF);
//         d=FF(d,a,b,c,x[k+5], S12,0x4787C62A);
//         c=FF(c,d,a,b,x[k+6], S13,0xA8304613);
//         b=FF(b,c,d,a,x[k+7], S14,0xFD469501);
//         a=FF(a,b,c,d,x[k+8], S11,0x698098D8);
//         d=FF(d,a,b,c,x[k+9], S12,0x8B44F7AF);
//     }
// }