// #22. for 
// const array = [10, 20, 30];
// var x;
// for (const value of array.values()) {
//     x = console.log(value); 
// }

// const array = [10, 20, 30];
// var x;
// for (const value of array.values()) {
//     x = console.log(value); 
// }

// #22. for - normal
// var x = 1;
// for (var i = 0; i < 5; i++) {
//     x = i;
// }
// x = 10;

// #22. for - continue
// var x = 0;
// for (var i = 0; i < 5; i++) {
//     if (i === 2) {
//         x = 1;
//         continue; 
//     }
//     x = 2;
// }
// x = 3;

// #22. for - break
// var x = 0;
// for (var i = 0; i < 10; i++) {
//     x = 1;
//     if (i === 5) {
//         x = 2;
//         break; 
//     }else{
//         x = 4;
//     }
//     x = 3;
// }
// x = 5;


// #22. for - return
// function findNumber() {
//     var x = 5;
//     var i = 0;
//     for (i = 0; i < 10; i++) {
//         if (i = x) {
//             return i; 
//         }
//     }
//     return -1;
// }


// #22. for - break-throw
// for (var i = 0; i < 10; i++) {
//     if (i === 5) {
//         throw new Error('exit');
//     }
// }

// #22. for - break-to-label
// loop1:{

// loop2:{
//     var x = 0;
//     console.log("hello");
//     for (let i = 0; i < 5; i++) {
//         if (i === 3) {
//             x = 1;
//             console.log(x);
//             break loop1;
//         }
//         x = 2;
//     }
// }
// }




// # 22. while - normal
// var x = 0;
// var i = 0;
// while (i < 5) {
//     x = i;
//     i = i + 1;
// }
// x = 10;

// # 22. while - continue
// var x = 0;
// var i = 0;
// while (i < 5) {
//     i++;
//     if (i === 3) {
//         x = i;
//         continue; 
//     }
//     x = 4;
// }
// x = 5;

// # 22. while - break
// var x = 0
// var i = 0;
// while (true) {
//     i++;
//     if (i === 5) {
//         x = i;
//         break;
//     }
//     x = 1;
// }
// x = 3;


// # 22. while - return
// function processData() {
//     var i = 0;
//     var x = 0;
//     while (i < 10) {
//         x = i + 1;
//         if (i === 5) {
//             x = 2;
//             return; 
//         }
//         i++;
//     }
//     x = 7;
// }

// # 22. while - throw
// var i = 0;
// var x = 0;
// while (i < 10) {
//     x = 1;
//     if (i === 5) {
//         x = 2;
//         throw new Error('1');
//     }
//     i++;
//     x = 3;
// }
// x = 4;

//# 22. dowhile - normal
// var i = 0;
// var x = 1;
// do {
//     x = 1
//     i++;
// } while (i < 5);
// x = 2;

//# 22. dowhile - continue
// var i = 0; 
// var x = 1; 
// do {     
//     i++;
//     if (i === 3) {
//         x = 2;
//         continue; 
//     }
//     x = i; 
// } while (i < 5); 
// x = 3;

// # 22. dowhile - break
// var i = 0; 
// var x = 1; 
// do {     
//     i++;
//     x = 2;
//     if (x >= 8) {
//         x = 3;
//         break;
//     }
// } while (i < 5); 
// x = 4;

// # 22. dowhile - return
// function example2() {
//     var i = 0; 
//     var x = 1; 
//     do {     
//         i++;
//         x = i * 10; 
        
//         if (x >= 30) {
//             x = 888; 
//             return;
//         }
//     } while (i < 5); 
    
//     x = 2;
//     return;
// }

// # 22. dowhile - throw
// var i = 0; 
// var x = 1; 
// do {     
//     i++;
//     x = i;
    
//     if (i === 3) {
//         x = 99; 
//         throw new Error("z");
//     }
// } while (i < 5); 
// x = 2; 

// while-catch
// const list = []
// for(let x of list) {
//     console.log("hi");
// }