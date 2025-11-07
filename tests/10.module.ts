// run.sh module

// # 26. module var: import and export clause
// import { a, b } from "./module_foo"
// import * as c from "./module_bar"

// import add from './math'; 

// # 26.1 other imports
// import { a, b } from "./module_foo1"
// import * as c from "./module_bar"
// import { d, e } from "./module_foo2"
// import { PI as Pi, add as sum } from './math';
// import { namedExport as h } from 'module-path' assert { type: 'int' };

// # 26.2 exports
// export let i: number = 3;
// export function add(a: number, b: number): number {
//     return a + b;
// }
// export default api;

// export *  from './utils';
// export * as UserService from './userService';
// export { a as b , d as d} from './logger';


// # 26. module: LDLOCALMODULEVAR_IMM8, STMODULEVAR_IMM8, LDEXTERNALMODULEVAR_IMM8, WIDE_LDLOCALMODULEVAR_PREF_IMM16, WIDE_STMODULEVAR_PREF_IMM16, WIDE_LDEXTERNALMODULEVAR_PREF_IMM16
// import { a, b } from "./module_foo1"
// import { d, e } from "./module_foo2"
// export { a as b , d as d} from './logger';

// export let i: number = 3;
// export let f: number = 3;
// export let g: number = 4;
// export *  from './utils';
// export * as UserService from './userService';

// var x = a + b + d + e + f + Pi + sum;


// 33. ldlazymodulevar
// import lazy {a} from './a'
// import {b} from './b'
// import c from './a'
// import lazy d from './c'
// import lazy from './b'


// console.log(b);
// console.log(c);
// console.log(d);

// 34. ldsendablelocalmodulevar, ldlazysendablemodulevar and ldsendableexternalmodulevar
// import lazy {a} from './a'
// import {b} from './b'
// import c from './c'
// import lazy d from './d'
// import e from './e'

// import f from "./f"

// class B {
//   constructor() {
//       "use sendable";
//   }

//   foo() {
//       console.log(a);
//       console.log(b);
//       new f();
//       console.log(c);
//       console.log(d);
//   }
// }

// console.log(e);

// 35. DYNAMICIMPORT

// import('./moduleName').then(module => {
//   module.someFunction();
// }).catch(error => {
//   console.error("Error loading module:", error);
// });