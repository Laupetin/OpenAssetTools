export interface NativeMethods {
  greet: (name: string) => Promise<string>;
}

// @ts-expect-error Typescript expects this to be an error, it is not here though
export const nativeMethods: NativeMethods = window as NativeMethods;
