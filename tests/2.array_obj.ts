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


// 12. NEWOBJAPPLY
// const numbers = [5, 6, 2, 3, 7];
// const max = Math.max.apply(null, numbers);


// #13. starrayspread apply
// const numbers = [10, 20, 30, 5, 15];
// const maxNumber = Math.max(1,2,3,...numbers); 


