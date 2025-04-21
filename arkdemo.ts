merged = undefined;
let obj1 = {"a" : 1 , "b" : 2 , "c" : 3};
let obj2 = {"b" : 4 , "d" : 5};
merged = {"e" : 6 , ...obj1 , ...obj2};
return undefined;
