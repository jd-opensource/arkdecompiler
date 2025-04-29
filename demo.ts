// 1. CONST
// const pi: number = 3.14159;

// 2. Bin and Unary
//let c;
// let c;
// c = 1 + 1;
// let d = true;
// let e = false;
// let f = null;
// let g = undefined;
// var obj9 = NaN;
// var obj10 = Infinity;
// c = 3 * 9;
// let h = ~ 1;
// let i = 2;
// i--;
// i++;

// 3. typeof
// let j = 3;
// let h = typeof j;

// 4. instanceof
// var str = "Hello, World!";
// var b = str instanceof Number;

// 5. createemptyarray  createarraywithbuffer  createemptyobject createobjectwithbuffer
// var obj2 = {}; 
// var obj3 = []; 
// var obj4 = [1, 2, 3];
// var obj5 = {"a":1, "b":2, "c":3};


// 6. callargs callthis
// var x1 = clearTimeout();
// var x2 = parseInt("42");
// var x3 = parseInt("1010", 2); 
// var x4 = parseInt("10", 10, 0);
// var x5 = Math.random();
// var x6 = console.log("hello");

// 7. stobjbyvalue ldobjbyvalue
// var obj6 = {"a":1, "b":2, "c":3};
// var c = obj6["a"];
// obj6["d"] = 4;

// 8. isin stobjbyname 
// var obj7 = {"a":1, "b":2, "c":3};
// var obj8 = "a" in obj7;
// obj7.a = 5;


// 9. callarg1 tLoadString
// var obj11 = BigInt("1234567890123456789012345678901234567890");


// 10. NEWOBJRANGE
// const regex = new RegExp('pattern', 'flags');


// 11. COPYDATAPROPERTIES
// const obj1 = { a: 1, b: 2, c: 3 };
// const obj2 = { b: 4, d: 5 };
// var merged = { e: 6, ...obj1, ...obj2 };



// 12. NEWOBJAPPLY
// const numbers = [5, 6, 2, 3, 7];
// const max = Math.max.apply(null, numbers);


// 13. starrayspread apply
// const numbers = [10, 20, 30, 5, 15];
// const maxNumber = Math.max(1,2,3,...numbers); 

// 14. callthisrange callrange
// var x= setInterval(Math.max, 2000, "Bob", "Hi");
// const maxNumber = Math.max(1,2,3,4,5,6,7);

// DELOBJPROP
// let obj = { a: 1, b: 2 };
// let result = delete obj.c; 

// IfImm
// var x;
// var y;
// if(x){
//     y = console.log("true");
//     y = console.log("1");
// }else{
//     y = console.log("false");
// }

// const array = [10, 20, 30];
// for (const value of array.values()) {
//     console.log(value); 
// }


function add(a: number, b: number): number {
    return a + b; // 返回两个数的和
}

var x = add(2, 3);