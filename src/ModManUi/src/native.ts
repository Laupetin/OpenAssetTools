export interface NativeMethods{

    greet: (name: string) => Promise<string>;
}

// @ts-expect-error
export const nativeMethods: NativeMethods = window as NativeMethods;
