// class Animal {
//     name: string;

//     constructor(name: string) {
//         this.name = name;
//     }

//     move(): void {
//         console.log(`${this.name} is moving`);
//     }
// }

// interface Flyable {
//     fly(): void;
// }

// interface Swimmable {
//     swim(): void;
// }

// class Bird extends Animal implements Flyable {
//     constructor(name: string) {
//         super(name);
//     }

//     fly(): void {
//         console.log(`${this.name} is flying`);
//     }

//     move(): void {
//         console.log(`${this.name} is hopping and flying`);
//     }
// }

// class Fish extends Animal implements Swimmable {
//     constructor(name: string) {
//         super(name);
//     }

//     swim(): void {
//         console.log(`${this.name} is swimming`);
//     }

//     move(): void {
//         console.log(`${this.name} is swimming gracefully`);
//     }
// }

// class Duck extends Animal implements Flyable, Swimmable {
//     constructor(name: string) {
//         super(name);
//     }

//     fly(): void {
//         console.log(`${this.name} is flying`);
//     }

//     swim(): void {
//         console.log(`${this.name} is swimming`);
//     }

//     move(): void {
//         console.log(`${this.name} is waddling, swimming, and flying`);
//     }
// }

// const sparrow = new Bird("Sparrow");
// sparrow.move();
// sparrow.fly();

// const goldfish = new Fish("Goldfish");
// goldfish.move();
// goldfish.swim();

// const donald = new Duck("Donald");
// donald.move();
// donald.fly();
// donald.swim();


// # 28. LDPRIVATEPROPERTY createprivateproperty  defineprivateproperty 
// class XXX {
//     #yyy: number = 8;
//     #zzz: number = 9;
//     #add1() {
//       this.#yyy += 1;
//     }

//     #add2() {
//         this.#zzz += 2;
//     }

//     sub() {
//         this.#yyy -= 3;
//         this.#add1();
//         this.#add2();
//     }
// }
// var xxx = new XXX();

// class YYY {
//     #yyy1: number = 8;
//     #zzz1: number = 9;
//     #adda() {
//       this.#yyy1 += 1;
//     }

//     #addb() {
//         this.#zzz1 += 2;
//     }

//     sub() {
//         this.#yyy1 -= 3;
//         this.#adda();
//         this.#addb();
//     }
// }

// var yyy = new YYY();
// function hello() {
//     function hi() {
//         var x1 = 21;
//         let x2 = function () {
//             return x1;
//         };
//     }
// }


// 29. ldsuperbyvalue ldsuperbyname stsuperbyname stsuperbyvalue supercallforwardallargs
// class Animal {
//     constructor(name: string) {
//         this.name = name;
//     }

//     makeSound() {
//         console.log(this.name);
//     }
// }

// class Dog extends Animal {
//     makeSound() {
//         super.makeSound(); 
//         var x = super["name"];
//         console.log(x);
//     }

//     makeDance(){
//         super.name = "dance";
//         super["name"] = "dog";
//     }
// }

// const myDog = new Dog("Buddy", "Golden Retriever");
// myDog.makeSound();

// 30. ldthisbyname ldthisbyvalue stthisbyname stthisbyvalue
// class Animal {
//     constructor(name: string) {
//         this.name = name;
//     }

//     makeDance(){
//         console.log(this["hello"]);
//     }
// }

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

// 37. COPYRESTARGS SUPERCALLSPREAD
// class A {}
// class B extends A {
//     constructor(...args) {
//        super(1, ...args)
//     }
// }


// 42. definegettersetterbyvalue
// class Example {
//     constructor(value) {
//       this._value = value;
//     }
  
//     // Getter
//     get value() {
//       console.log('Getting value');
//       return this._value;
//     }
  
//     // Setter
//     set value(newValue) {
//       console.log('Setting value');
//       this._value = newValue;
//     }
// }
  
// const example = new Example(10);


// 41. stownbyvaluewithnameset topropertykey
// class A5 {
//     [a5]() {}
// }

// 42. definegettersetterbyvalue
// class Example {
//     constructor(value) {
//       this._value = value;
//     }
  
//     // Getter
//     get value() {
//       console.log('Getting value');
//       return this._value;
//     }
  
//     // Setter
//     set value(newValue) {
//       console.log('Setting value');
//       this._value = newValue;
//     }
// }
  
// const example = new Example(10);

// 37. COPYRESTARGS SUPERCALLSPREAD
// class A {}
// class B extends A {
//     constructor(...args) {
//        super(1, ...args)
//     }
// }


// 41. getunmappedargs  stownbyvaluewithnameset topropertykey
// class A5 {
//     [a5]() {}
// }

// #1. corner case
// export class C {
//     m(): this is D {
//         return this instanceof D;
//     }
// }

// class D extends C {
// }

// #2. corner case
// declare var decorator: any;
// class C1 {
//     #x
//     @decorator(  (x: C1) => x.#x  )
//     y() {}
// }

// #3. corner case
// class A {
//     a?: A
//     #b?: A;
//     getA(): A {
//         return new A();
//     }
//     constructor() {
//         this?.#b;           // Error
//         this?.a.#b;         // Error
//         this?.getA().#b;    // Error
//     }
// }

// #4. corner case
// class C {
//     static #x = 123;
  
//     static {
//       console.log(C.#x)
//     }
  
//     foo () {
//       return C.#x;
//     }
// }

// #5. corner case
// class A {
//     m() { }
// }
// class B extends A {
//     get m() { return () => 1 }
// }

// #6. corner case 
// declare function foo<T, U>(x: T, fun: () => Iterable<(x: T) => U>, fun2: (y: U) => T): T;

// foo("", function* () {
//     yield* {
//         *[Symbol.iterator]() {
//             yield x => x.length
//         }
//     }
// }, p => undefined); // T is fixed, should be string


// #7. corner case 
// class A {
//     a?: A
//     #b?: A;
//     getA(): A {
//         return new A();
//     }
//     constructor() {
//         this?.#b;           // Error
//         this?.a.#b;         // Error
//         this?.getA().#b;    // Error
//     }
// }