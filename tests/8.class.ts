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