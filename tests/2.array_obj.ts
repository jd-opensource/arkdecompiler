// 5. createemptyarray  createarraywithbuffer  createemptyobject createobjectwithbuffer
// var obj2 = {}; 
// var obj3 = []; 
// var obj4 = [1, 2, 3];
// var obj5 = {"a":1, "b":2, "c":3};


// #7. stobjbyvalue ldobjbyvalue
// var obj6 = {"a":1, "b":2, "c":3};
// var c = obj6["a"];
// obj6["d"] = 4;


// 8. isin stobjbyname 
// var obj7 = {"a":1, "b":2, "c":3};
// var obj8 = "a" in obj7;
// obj7.a = 5


// #15. DELOBJPROP
// let obj = { a: 1, b: 2 };
// let result = delete obj.c; 

// #10. NEWOBJRANGE
// const regex = new RegExp('pattern', 'flags');

// 11. COPYDATAPROPERTIES
// const obj1 = { a: 1, b: 2, c: 3 };
// const obj2 = { b: 4, d: 5 };
// var merged = { e: 6, ...obj1, ...obj2 };


// 12. createarraywithbuffer
// const numbers = [5, 6, 2, 3, 7];
// const max = Math.max.apply(null, numbers);


// #13. starrayspread apply
// const numbers = [10, 20, 30, 5, 15];
// const maxNumber = Math.max(1,2,3,...numbers); 

// # 24. stownbyvalue
// const arr1 = [1, 2, 3];
// const arr2 = [...arr1, 4, 5]; 

// # 25. stownbyindex
// var r1;
// var r2;
// var r3 = [1, r1, ...r2];

// 36. NEWOBJAPPLY_
// const countryInfo = ['USA', 'The Star-Spangled Banner', 'John Stafford Smith'];
// const usa = new Country(...countryInfo);


// 39. CREATEOBJECTWITHEXCLUDEDKEYS
// function foo()
// {
//     let {a, b, c, ...d} = {a:1, b:2, c:3, f:4, e:5}
// }

// const sourceObject = { a: 1, b: 2, c: 3, f: 4, e: 5 };

// let { a: alpha, b: beta, c: gamma, ...rest } = sourceObject;


// console.log(alpha); 
// console.log(beta); 
// console.log(gamma);


// 38. CREATEREGEXPWITHLITERAL
// var x = 0x1; 
// var y = new RegExp("hello", x);


// 36. NEWOBJAPPLY_
// const countryInfo = ['USA', 'The Star-Spangled Banner', 'John Stafford Smith'];
// const usa = new Country(...countryInfo);

// 38. CREATEREGEXPWITHLITERAL
// var x = 0x1; 
// var y = new RegExp("hello", x);

// 39. CREATEOBJECTWITHEXCLUDEDKEYS
// function foo()
// {
//     let {a, b, c, ...d} = {a:1, b:2, c:3, f:4, e:5}
// }

// const sourceObject = { a: 1, b: 2, c: 3, f: 4, e: 5 };

// let { a: alpha, b: beta, c: gamma, ...rest } = sourceObject;


// console.log(alpha); 
// console.log(beta); 
// console.log(gamma); 


// corner case 1
// declare function dec<T>(target: any, propertyKey: string, descriptor: TypedPropertyDescriptor<T>): TypedPropertyDescriptor<T>;

// class C {
//     @dec ["1"]() { }
//     @dec ["b"]() { }
// }

// corner case 2
// var a0 = [,, 2, 3, 4];

// corner case 3
// var a0 = [,, 2, 3, 4]
// var a2 = [, , , ...a0, "hello"];

// corner case 4
// const membersOfI = [];
// const membersOfI2 = [];
// const membersOfIAndI2 = [...membersOfI, ...membersOfI2];

// corner case 5
// let x;
// [x = 0xaa] = [91];
// [x = 0xaa] = [99];
