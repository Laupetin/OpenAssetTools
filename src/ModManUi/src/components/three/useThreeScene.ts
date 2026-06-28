import { createInjectionState } from "@vueuse/core";
import { type Object3D, Scene } from "three";
import {
  type ResourceTrackedType,
  ThreeResourceTracker,
} from "@/components/three/ThreeResourceTracker.ts";
import { onScopeDispose, shallowRef, watch } from "vue";

export const [provideThreeScene, useThreeScene] = createInjectionState(() => {
  const scene = new Scene();
  const resourceTracker = new ThreeResourceTracker();

  return { scene, resourceTracker };
});

export function useThreeSceneOrThrow() {
  const value = useThreeScene();
  if (!value) throw new Error("ThreeScene must be provided");
  return value;
}

export function createResourceTrackedObject<T extends ResourceTrackedType>(initialValue?: T) {
  const { resourceTracker } = useThreeSceneOrThrow();
  const refObj = shallowRef<T | undefined>(initialValue);

  watch(
    refObj,
    (newValue, oldValue) => {
      if (newValue === oldValue) return;

      if (newValue) {
        resourceTracker.ref(newValue);
      }

      if (oldValue) {
        resourceTracker.unref(oldValue);
      }
    },
    { immediate: true },
  );

  onScopeDispose(() => {
    const remainingValue = refObj.value;
    if (remainingValue) {
      resourceTracker.unref(remainingValue);
      refObj.value = undefined;
    }
  });

  return refObj;
}

export function createSceneObject<T extends Object3D>(initialValue?: T) {
  const { scene } = useThreeSceneOrThrow();
  const refObj = createResourceTrackedObject<T>(initialValue);

  watch(refObj, (newValue, oldValue) => {
    if (newValue) {
      scene.add(newValue);
    }
    if (oldValue) {
      scene.remove(oldValue);
    }
  });

  return refObj;
}
