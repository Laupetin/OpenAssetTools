import { createInjectionState } from "@vueuse/core";
import { Scene } from "three";

export const [provideThreeScene, useThreeScene] = createInjectionState(() => {
  const scene = new Scene();

  return { scene };
});

export function useThreeSceneOrThrow() {
  const value = useThreeScene();
  if (!value) throw new Error("ThreeScene must be provided");
  return value;
}
