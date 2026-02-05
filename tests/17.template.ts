// case 1
// interface Mixin1 {
//     mixinMethod(): void;
// }

// abstract class AbstractBase {
//     abstract abstractBaseMethod(): void;
// }

// function Mixin2<TBase extends abstract new (...args: any[]) => any>(baseClass: TBase) {
//     // must be `abstract` because we cannot know *all* of the possible abstract members that need to be
//     // implemented for this to be concrete.
//     abstract class MixinClass extends baseClass implements Mixin1 {
//         mixinMethod(): void {}
//         static staticMixinMethod(): void {}
//     }
//     return MixinClass;
// }

// class DerivedFromAbstract2 extends Mixin2(AbstractBase) {
//     abstractBaseMethod() {}
// }

// case 2
// interface IData {
//   bar: boolean
// }

// declare function test<TDependencies>(
//   getter: (deps: TDependencies, data: IData) => any,
//   deps: TDependencies,
// ): any 

// const DEPS = {
//   foo: 1
// }

// test(
//   (deps, data) => {
//       return {
//          fn1(a) { return deps.foo + a },
//       }
//   },
//   DEPS
// );